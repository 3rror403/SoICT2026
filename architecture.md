# Architecture and Algorithms - SoICT2026 (Detailed Version)

## 1. Overview

The `SoICT2026` project solves a complex vehicle routing and scheduling problem with pickups and deliveries for mixed requests (Passengers and Parcels). The software is implemented in C++20, segmented into `soict_core` (domain logic), `soict_alns` (meta-heuristic search), and `soict_mip` (exact formulation).

---

## 2. Advanced Route Evaluation (`RouteCache` & `FastRouteCheck`)

*   **`RouteCache` & `Segment`**
    *   **Purpose**: Caches the temporal and capacity statistics of any contiguous sequence of nodes in $O(1)$ time using a Disjoint Sparse Table (DST).
    *   **Input**: A sequence of nodes (sub-route).
    *   **Output**: A `Segment` struct containing `duration`, `cost`, `ready_time`, `latest_input`, and `load` extrema.
    *   **Mechanism**: Uses a globally atomic `version_` tracker. Operators check if `route.version() == cache.version()` to instantly bypass redundant constraint checks.

---

## 3. ALNS Operators Detailed Pseudocode

### 3.1 Destroy Operators

*   **`RandomRemoval`**
    *   **Purpose**: Randomly removes requests to ensure pure stochastic diversity, helping escape local minima.
    *   **Input**: Current `SearchState`, `q` (number of requests to remove).
    *   **Output**: Modified `SearchState` with `q` unassigned requests.
    *   **Pseudocode**:
        ```python
        def RandomRemoval(state, q):
            assigned_list = get_all_assigned_requests(state)
            shuffle(assigned_list)
            for i = 0 to q - 1:
                state.unassign(assigned_list[i])
        ```

*   **`WorstRemoval` (Randomized Ropke-Pisinger)**
    *   **Purpose**: Removes requests that incur the highest cost in their current routes, using a randomized exponential distribution ($p=3$) to avoid deterministic cycling.
    *   **Input**: Current `SearchState`, `q`.
    *   **Output**: Modified `SearchState`.
    *   **Pseudocode**:
        ```python
        def WorstRemoval(state, q):
            L = []
            for request r in state.assigned_requests:
                cost_savings = state.cost(r.route) - state.cost(r.route \ {r})
                L.append( (r, cost_savings) )
            
            sort L descending by cost_savings # In code: uses std::nth_element for O(N) performance
            
            while q > 0:
                y = Random.Uniform(0.0, 1.0)
                idx = floor(y^3 * len(L))
                state.unassign(L[idx].request)
                remove L[idx] from L
                q -= 1
        ```

*   **`RelatedRemoval` (Shaw Removal)**
    *   **Purpose**: Removes a cluster of requests that are physically and temporally close, as they likely compete for the same vehicle capacity.
    *   **Input**: Current `SearchState`, `q`.
    *   **Output**: Modified `SearchState`.
    *   **Pseudocode**:
        ```python
        def RelatedRemoval(state, q):
            anchor = select_random_assigned_request(state)
            L = []
            for request r in state.assigned_requests:
                if r == anchor: continue
                # Calculate geographical & temporal distance
                dist = travel_time(anchor.pickup, r.pickup) 
                       + alpha * abs(anchor.early_tw - r.early_tw)
                L.append( (r, dist) )
            
            sort L ascending by dist
            
            state.unassign(anchor)
            q -= 1
            while q > 0:
                y = Random.Uniform(0.0, 1.0)
                idx = floor(y^3 * len(L))
                state.unassign(L[idx].request)
                remove L[idx] from L
                q -= 1
        ```

*   **`RouteRemoval`**
    *   **Purpose**: Completely empties an entire taxi route. Targets short/inefficient routes to minimize the active fleet size.
    *   **Input**: Current `SearchState`.
    *   **Output**: Modified `SearchState` with one route fully emptied.
    *   **Pseudocode**:
        ```python
        def RouteRemoval(state, q):
            while q > 0 and has_active_routes(state):
                # Weight inversely proportional to route size
                weights = [ 1.0 / len(route) for route in state.routes ]
                target_route = Random.RouletteWheel(state.routes, weights)
                
                for request r in target_route:
                    state.unassign(r)
                    q -= 1
        ```

*   **`DelayRemoval`**
    *   **Purpose**: Removes requests that are scheduled very close to their late deadline, targeting tight constraints to relieve localized scheduling congestion.
    *   **Input**: Current `SearchState`, `q`.
    *   **Output**: Modified `SearchState`.
    *   **Pseudocode**:
        ```python
        def DelayRemoval(state, q):
            L = []
            for request r in state.assigned_requests:
                # How close is the vehicle arriving compared to the late deadline?
                delay_risk = r.late_tw - state.get_arrival_time(r.pickup)
                L.append((r, delay_risk))
            
            sort L ascending by delay_risk (smallest margin = highest risk)
            
            while q > 0:
                y = Random.Uniform(0.0, 1.0)
                idx = floor(y^3 * len(L))
                state.unassign(L[idx].request)
                remove L[idx] from L
                q -= 1
        ```

*   **`TimeBucketRemoval`**
    *   **Purpose**: Removes requests that fall into the same temporal bucket (e.g., peak hour) to disrupt dense blocks of time where vehicles are overloaded.
    *   **Input**: Current `SearchState`, `q`.
    *   **Output**: Modified `SearchState`.
    *   **Pseudocode**:
        ```python
        def TimeBucketRemoval(state, q):
            bucket_size = config.time_limit / 10 # Example chunk size
            target_bucket = Random.Integer(0, 9)
            
            L = []
            for request r in state.assigned_requests:
                if r.early_tw falls inside target_bucket:
                    L.append(r)
                    
            shuffle(L)
            for i = 0 to min(q, len(L)) - 1:
                state.unassign(L[i])
        ```

---

### 3.2 Repair Operators

*   **`GreedyInsertion`**
    *   **Purpose**: Inserts the single unassigned request that yields the highest immediate profit (or lowest insertion cost) across all possible positions.
    *   **Input**: Current `SearchState`.
    *   **Output**: Modified `SearchState` with unassigned requests re-inserted.
    *   **Pseudocode**:
        ```python
        def GreedyInsertion(state):
            while not state.unassigned_pool.is_empty():
                best_request = null
                best_gain = -INFINITY
                best_insertion_pos = null
                
                for request u in state.unassigned_pool:
                    pos, gain = InsertionSearch.find_best_insertion(state, u)
                    if gain > best_gain:
                        best_gain = gain
                        best_request = u
                        best_insertion_pos = pos
                
                if best_request is null:
                    break # No feasible insertion exists for any request
                    
                state.insert(best_request, best_insertion_pos)
        ```

*   **`PairAwareGreedy` (`SharedGapPairRepair`)**
    *   **Purpose**: Inserts pairs of unassigned requests as a **contiguous 4-node block** into a single taxi, exploiting synergy between co-located requests that a single-request greedy cannot capture. Unlike `GreedyInsertion`, which always inserts one request at a time, `PairAwareGreedy` will commit a pair when doing so yields strictly positive marginal synergy over inserting each request independently.
    *   **Key concept — Synergy**: For a candidate pair $(u, v)$:
        $$Syn(u,v) = G_{uv} - \max(0, g_u^*) - \max(0, g_v^*)$$
        where $G_{uv}$ = gain of co-inserting the pair as a block, and $g_u^*, g_v^*$ = best single-insertion gain for each request independently in the **same** state.
    *   **Conflict-aware selection rule**: Pair insertion is only preferred over the best single when:
        1. No profitable single exists, **or**
        2. The best single request is one of the pair members, **or**
        3. The best single and best pair share the same taxi (route conflict).
        Otherwise, the profitable single is applied first and the loop repeats.
    *   **Block patterns**: All valid interleavings for the 4 nodes $(u_p, u_d, v_p, v_d)$ are generated per type combination (2 patterns for pass+pass, 3 for pass+parcel, 6 for parcel+parcel).
    *   **Complexity**: $O(U^2 \cdot P \cdot T \cdot L)$ per outer loop step, where $U$ = unassigned count ($\le 40$), $P$ = patterns (2–6), $T$ = taxi count, $L$ = max route length. Full $O(U^2)$ scan with 100% coverage (no partner bank).
    *   **Termination**: Stops when neither a profitable single $(g > \epsilon)$ nor a positive-gain pair exists. Unprofitable requests are left unassigned.
    *   **Input**: `SearchState`, `removed` span from Destroy operator.
    *   **Output**: Modified `SearchState` with as many unassigned requests re-inserted as profitable.
    *   **Benchmark result (r100, 20s, seed=1)**:
        - BKS improved on 3/5 instances within 20s.
        - Best-pair synergy confirmed positive (`+85.7`, `+30.8`, `+23.7`) after `RelatedRemoval` and `DelayRemoval`.
        - `TimeBucketRemoval` yields negative synergy — operator learns lower weight for that pair via Thompson Sampling.
    *   **Pseudocode**:
        ```python
        def PairAwareGreedy(state, removed):
            unassigned = [r for r in removed if r not assigned]
            EPS = 1e-8
            while True:
                # Phase 1: best single insertion per unassigned request
                single_gain = {}
                best_single = None
                for u in unassigned:
                    pos, gain = find_best_insertion(state, u)
                    single_gain[u] = gain
                    if gain > EPS and (not best_single or gain > best_single.gain):
                        best_single = SingleDesc(u, pos, gain)

                # Phase 2: full O(U^2) pair scan
                best_pair = None
                for u, v in combinations(unassigned, 2):
                    if single_gain[u] == -INF or single_gain[v] == -INF: continue
                    for pattern in get_valid_patterns(u, v):  # 2-6 patterns
                        block = create_4node_block(pattern)
                        if not block.feasible: continue
                        for taxi in all_taxis:
                            for gap in all_gaps(taxi):
                                gain = evaluate_block(block, taxi, gap)
                                if gain > EPS:
                                    synergy = gain - max(0, single_gain[u]) - max(0, single_gain[v])
                                    if synergy > (best_pair.synergy if best_pair else -INF):
                                        best_pair = PairDesc(u, v, ...)

                # Phase 3: conflict-aware selection
                if not best_single and not best_pair: break
                if best_pair and best_pair.synergy > EPS:
                    conflict = best_single and best_single.taxi == best_pair.taxi
                    if not best_single or best_single.req in (best_pair.u, best_pair.v) or conflict:
                        apply_pair(state, best_pair); continue
                if best_single:
                    apply_single(state, best_single); continue
                if best_pair and best_pair.gain > EPS:
                    apply_pair(state, best_pair); continue
                break
        ```

*   **`RegretInsertion` (V1, V2, RouteRegret3)**
    *   **Purpose**: Looks ahead to prevent hard-to-insert requests from being stranded by prioritizing requests with the largest difference between their best and alternative insertion options.
    *   **Input**: Current `SearchState`.
    *   **Output**: Modified `SearchState`.
    *   **Pseudocode**:
        ```python
        def RegretInsertion(state, variant):
            while not state.unassigned_pool.is_empty():
                best_request = null
                max_regret = -INFINITY
                best_insertion_pos = null
                best_gain = -INFINITY
                
                for request u in state.unassigned_pool:
                    # Find top insertions globally OR across unique routes
                    choices = InsertionSearch.find_top_k_insertions(state, u, k=3, route_unique=(variant=="RouteRegret3"))
                    
                    gain1 = choices[0].gain
                    gain2 = choices[1].gain if len(choices) > 1 else -INFINITY
                    
                    if variant == "V1":
                        regret = gain1 - max(gain2, 0.0) # Assume 0 gain if second is infeasible
                    elif variant == "V2":
                        if len(choices) == 1:
                            regret = INFINITY # Absolute priority if only 1 option exists
                        else:
                            regret = gain1 - gain2
                    elif variant == "RouteRegret3":
                        gain3 = choices[2].gain if len(choices) > 2 else -INFINITY
                        regret = gain1 - gain3 # Pushes to distribute across multiple vehicles
                        
                    if regret > max_regret or (regret == max_regret and gain1 > best_gain):
                        max_regret = regret
                        best_gain = gain1
                        best_request = u
                        best_insertion_pos = choices[0].pos
                
                if best_request is null:
                    break
                    
                state.insert(best_request, best_insertion_pos)
        ```

---

### 3.3 Local Search Operators (VND)

*   **`PassengerRelocate` / `ParcelRelocate`**
    *   **Purpose**: Ejects an assigned request and inserts it into a better position (inter-route or intra-route). Uses `thread_local` caching and Best-First $O(1)$ pruning.
    *   **Input**: `SearchState`.
    *   **Output**: `boolean` indicating if an improvement was found and applied.
    *   **Pseudocode**:
        ```python
        thread_local Array<InsertionBound> bounds_cache;

        def Relocate(state):
            for request v in state.assigned_requests:
                origin_route = state.route(v)
                savings = state.cost(origin_route) - state.cost(origin_route \ {v})
                
                bounds_cache.clear()
                
                for route in state.routes:
                    for pos in route.possible_insertions:
                        # O(1) Arc replacement calculation
                        arc_cost = Cost(A -> Pickup) + Cost(Pickup -> Dropoff) + Cost(Dropoff -> B) - Cost(A -> B)
                        net_gain_bound = savings - arc_cost
                        bounds_cache.append( {route, pos, net_gain_bound} )
                
                sort bounds_cache descending by net_gain_bound
                
                best_actual_gain = 0
                best_pos = null
                
                for bound in bounds_cache:
                    if bound.net_gain_bound <= best_actual_gain:
                        break # PRUNING: No subsequent bound can beat the actual gain
                        
                    if FastRouteCheck.is_feasible(bound.route, bound.pos, v):
                        actual_gain = savings - FastRouteCheck.evaluate_cost(bound.route, bound.pos, v)
                        if actual_gain > best_actual_gain:
                            best_actual_gain = actual_gain
                            best_pos = bound.pos
                            
                if best_actual_gain > 0:
                    state.unassign(v)
                    state.insert(v, best_pos)
                    return TRUE
            return FALSE
        ```

*   **`ReplaceRequest`**
    *   **Purpose**: Forcefully ejects an assigned request $v$ to make room for an unassigned request $u$ in a single atomic transaction. Enforces `boundary_allowed` for contiguous passenger rides.
    *   **Input**: `SearchState`.
    *   **Output**: `boolean` indicating if an improvement was found.
    *   **Pseudocode**:
        ```python
        def ReplaceRequest(state):
            for request u in state.unassigned_pool:
                for route in state.routes:
                    for request v in route.assigned_requests:
                        route_without_v = route \ {v}
                        savings_v = state.cost(route) - state.cost(route_without_v) - Revenue(v)
                        
                        for pos in route_without_v.possible_insertions:
                            # CRITICAL CONSTRAINT: Parcels cannot break passenger contiguous boundaries
                            if u is Parcel and not route_without_v.boundary_allowed(u, pos):
                                continue 
                                
                            arc_cost = calculate_O1_arc_cost(route_without_v, pos, u)
                            net_gain_bound = savings_v + Revenue(u) - arc_cost
                            
                            if net_gain_bound <= 0:
                                continue # Pruning
                                
                            if FastRouteCheck.is_feasible(route_without_v, pos, u):
                                actual_gain = savings_v + Revenue(u) - FastRouteCheck.evaluate_cost(...)
                                if actual_gain > 0:
                                    state.unassign(v)
                                    state.insert_into_route(route, u, pos)
                                    return TRUE
            return FALSE
        ```

*   **`SwapRequest` (SWAP* with Descriptor Pools)**
    *   **Purpose**: Exchanges two assigned requests $r_1$ and $r_2$ between two different routes. Extremely heavy $O(N^4)$, optimized using incremental $O(1)$ pool updates and Branch and Bound.
    *   **Input**: `SearchState`.
    *   **Output**: `boolean` indicating if an improvement was found.
    *   **Pseudocode**:
        ```python
        global SwapPool pool # Preserved across iterations

        def SwapRequest(state):
            # 1. Precompute MaxSavings and MinInsertCost for all requests/routes
            for request r in state.assigned_requests:
                r.MaxSavings = state.cost(r.route) - state.cost(r.route \ {r})
            for route in state.routes:
                for request r in state.assigned_requests:
                    route.MinInsertCost[r] = find_top_1_insert_cost_O1(route, r)

            # 2. Update Pool (Incremental Evaluation)
            dirty_routes = get_modified_routes_since_last_swap()
            pool.remove_descriptors_involving(dirty_routes)
            
            for request r1 in dirty_routes.requests:
                for request r2 in state.assigned_requests:
                    if r1.route == r2.route: continue
                    
                    # Branch and Bound Pruning
                    upper_bound = r1.MaxSavings + r2.MaxSavings 
                                  - r1.route.MinInsertCost[r2] 
                                  - r2.route.MinInsertCost[r1]
                                  
                    if upper_bound <= 0:
                        continue # Pruned in O(1)
                        
                    # Heavy exact evaluation
                    route1_new = (r1.route \ {r1}) U {r2}
                    route2_new = (r2.route \ {r2}) U {r1}
                    if FastRouteCheck.is_feasible(route1_new) and FastRouteCheck.is_feasible(route2_new):
                        actual_gain = (Cost(r1.route) + Cost(r2.route)) - (Cost(route1_new) + Cost(route2_new))
                        if actual_gain > 0:
                            pool.add( SwapDescriptor(r1, r2, actual_gain, positions...) )

            # 3. Apply Best Swap
            best_swap = pool.get_max_gain_descriptor()
            if best_swap is not null:
                state.apply_swap(best_swap)
                mark_routes_as_dirty(best_swap.route1, best_swap.route2)
                return TRUE
                
            return FALSE
        ```

---

## 4. Main Algorithm Flow (Simulated Annealing)

*   **`ALNS Main Loop`**
    *   **Purpose**: Orchestrates the Destroy/Repair and Local Search using a thermodynamic cooling schedule to probabilistically accept worse states and escape local optima.
    *   **Pseudocode**:
        ```python
        def ALNSSolve(instance, config):
            state = generate_initial_solution(instance)
            best_state = state
            temperature = config.start_temperature
            
            # Progress controls termination (Time-based or Iteration-based)
            while True:
                progress = get_progress() # elapsed_time / time_limit
                if progress >= 1.0:
                    break
                
                # 1. Roulette-Wheel Selection (Thompson Sampling / UCB)
                # Destroy pool (7): RandomRemoval, WorstRemoval, WorstMarginalBenefit,
                #                   DelayRemoval, TimeBucketRemoval, RelatedRemoval, RouteRemoval
                # Repair pool (5) : GreedyInsertion, PairAwareGreedy,
                #                   RegretV1, RegretV2, RouteRegret3
                # -> 35 Destroy x Repair couples total, each tracked by Thompson Sampling.
                # PairAwareGreedy synergy is empirically highest after RelatedRemoval/DelayRemoval.
                destroy_op = select_random(DestroyOperators, weights)
                repair_op  = select_random(RepairOperators, weights)
                
                # 2. Apply Perturbation
                candidate_state = state.clone()
                destroy_op(candidate_state)
                repair_op(candidate_state)
                
                # 3. Multi-Tier Local Search
                if iterations % config.ls_interval == 0:
                    # Gating Mechanism: Calculate Gap to Best Solution
                    gap = (best_state.benefit - candidate_state.benefit) / abs(best_state.benefit)
                    
                    # Tier 1 (Fast moves: Relocate, Add/Remove)
                    if gap <= config.ls_tier1_threshold:
                        apply_local_search_tier(candidate_state, tier=1)
                        
                    # Tier 2 (Expensive moves: Swap, 2-Opt*)
                    if gap <= config.ls_tier2_threshold:
                        apply_local_search_tier(candidate_state, tier=2)
                
                # 4. Simulated Annealing Acceptance
                delta = candidate_state.benefit - state.benefit
                if delta >= 0:
                    state = candidate_state
                    if state.benefit > best_state.benefit:
                        best_state = state
                else:
                    probability = exp(delta / temperature)
                    if Random.Uniform(0, 1) < probability:
                        state = candidate_state # Accept worse solution
                
                # 5. Update Weights & Cool down
                update_operator_weights(outcome)
                
                # Zero-Waste CPU: Clamp temperature, never exit early based on T
                temperature = max(config.min_temperature, temperature * config.cooling_rate)
                
            return best_state
        ```

## 7. Performance Profiling and Next Optimization Phases

Based on the latest static profiling of the ALNS solver on a complex `r100` instance, the runtime characteristics are:
- `PairAwareGreedy` (`SharedGapPairRepair`) takes **~65%** of total runtime.
- `Regret` variants take **~8%** each.
- Local Search (`Swap`, `Relocate`, `2Opt`) are heavily optimized and take **<2%** each.
- Low-level route checking (`FastRouteCheck`) evaluates **1.4 million** candidates in just **0.04s** (30ns/call).

### Optimization Strategy
Because route checking is nearly instantaneous, the solver bottleneck lies exclusively in combinatorial explosion and heap allocations within operators.
1. **Remove Heap Allocations in PairAware**: Replace dynamic `std::vector` usage for pattern generation with `std::array<Pattern, 6>` or static scratch buffers.
2. **PairAware Hybrid Bank**: Skip full insertion evaluation for pairs of requests that are trivially incompatible based on time windows.
3. **Regret Insertion Caching**: Implement `RepairInsertionCache` so that Regret doesn't re-evaluate $O(U \times K)$ insertions after a single taxi route changes. Instead, invalidate and update only the modified taxi in $O(U)$.
4. **Precompute Node Segments**: Since a single node's `Segment` properties never change, cache them as an array `std::vector<Segment> node_segments` and return them by reference, stripping branching and math out of the hottest loops.
