# Architecture and Algorithms - SoICT2026 (Detailed Version)

## 1. Overview

The `SoICT2026` project is designed to solve a complex vehicle routing and scheduling problem with pickups and deliveries. Specifically, the problem includes mixed requests:
*   **Passengers**: Require direct transport from a pickup location to a drop-off location within specified time windows.
*   **Parcels**: Require transport from a pickup to a delivery location, but unlike passengers, a taxi can carry multiple parcels simultaneously up to its `parcel_capacity`.

The software is implemented in C++20 and separated into core domain logic (`soict_core`), meta-heuristic search (`soict_alns`), and exact mathematical programming (`soict_mip`).

---

## 2. Project Architecture and Class Responsibilities

### 2.1 Core Library (`soict_core`)
This library provides the foundational data structures for the problem definition and solution state.

*   **`Instance` (`src/model/Instance.hpp`)**: Defines the immutable parameters of a problem instance. It stores arrays of `Taxi` (fleet with capacity), `Passenger`, and `Parcel`. It holds a precalculated, flattened distance/time matrix (`travel_time`, `travel_cost`).
    *   *Nodes Layout*: `0` to `K-1` for taxi origins; `K` to `K+N-1` for passenger pickups; `K+N` to `K+2N-1` for passenger drop-offs; similarly for parcels. This continuous indexing enables fast array-based lookups for matrices without hash maps.
*   **`Solution`**: Represents a completed sequence of node visits for each taxi.
*   **`State`**: Used during the search to represent a partially constructed solution. It keeps track of which requests are `unassigned` and provides fast query methods to check constraints and evaluate objective value changes.

### 2.2 ALNS Solver (`soict_alns`)
This library implements the Adaptive Large Neighborhood Search (ALNS) meta-heuristic. This is the primary engine for finding high-quality solutions efficiently.

#### 2.2.1 Advanced Route Evaluation (`RouteCache` & `FastRouteCheck`)
Evaluating whether an insertion (e.g., picking up a parcel at node A and delivering at node B) is feasible under strict time window and capacity constraints is computationally expensive. The project addresses this with highly optimized data structures:

*   **`Segment` (`RouteCache.hpp`)**: A struct summarizing a sequence of visited nodes. It records:
    *   `duration`, `cost`
    *   `ready`: The earliest time the vehicle can leave the last node of the segment.
    *   `latest_input`: The latest time the vehicle can arrive at the first node without violating any time windows within the segment.
    *   `load_change`, `min_load`, `max_load`: Changes and extrema of parcel weight.
*   **`RouteCache`**: 
    *   Uses a **Disjoint Sparse Table (DST)** to cache the `Segment` properties for all power-of-two contiguous sub-routes.
    *   *Logic*: The DST allows querying the combined `Segment` statistics of *any* arbitrary range `[first, last]` in $O(1)$ time, making sub-route evaluations incredibly fast without re-iterating over nodes.
*   **`FastRouteCheck`**: Uses the properties of `Segment`s to quickly check if inserting a node into a route maintains feasibility (time windows and capacity) by accumulating segment bounds left-to-right.

#### 2.2.2 Destroy Operators (`src/search/alns/operators/Destroy/`)
These operators take a complete `SearchState` and remove a configurable number of requests (`q`), putting them into an unassigned pool.

*   **`RandomRemoval.cpp`**: Randomly picks `q` assigned requests and removes them. Ensures pure diversity.
*   **`WorstRemoval.cpp`**: Calculates the *removal gain* (how much cost is saved by removing the request) for all assigned requests. It uses a **Randomized Ropke-Pisinger algorithm**: instead of strictly removing the absolute worst, it selects elements based on $idx = y^p \cdot N$ where $y \sim U(0,1)$ and $p=3$. This ensures heavy-cost requests are highly likely to be removed, but maintains stochastic variance. To maximize performance, it uses `std::nth_element` for $O(N)$ selection instead of $O(N \log N)$ sorting.
*   **`RelatedRemoval.cpp`** (Shaw Removal): Removes a cluster of requests that are "similar". It selects a random anchor request. Then, it removes requests that are geographically and temporally close to the anchor. Distance is measured by combining travel time between pickup nodes and the difference in their earliest time windows.
*   **`RouteRemoval.cpp`**: Selects a taxi route randomly and completely empties it. The selection probability is inversely proportional to the number of requests in the route ($W_i = 1 / |R_i|$). This targets short, inefficient routes and forces the repair operators to pack those requests into other, denser routes.

#### 2.2.3 Repair Operators (`src/search/alns/operators/Repair/`)
These operators take the unassigned requests and re-insert them heuristically.

*   **`InsertionSearch.cpp`**: The core logic that iterates over all taxis and all possible insertion positions `(pickup_pos, dropoff_pos)` to find the best valid insertion.
    *   *Optimization*: It pre-filters insertions by computing a lower bound of the cost increase using just the replaced adjacent arcs. If this lower bound exceeds the current best found insertion, the heavy constraint check (`FastRouteCheck`) is skipped entirely.
*   **`GreedyInsertion.cpp`**: Evaluates the best valid insertion for *each* unassigned request and inserts the one providing the highest immediate gain.
*   **`RegretInsertion.cpp`**: 
    *   *Logic*: Calculates the *regret value* for each request (difference in gain between its best insertion and its second-best insertion). Inserts the request with the highest regret to prevent hard-to-insert requests from being left out.
    *   *Variants*: Includes V0 (default `best - second`), V1 (`best - max(second, 0)`), and V2 (absolute priority if only 1 option exists).

#### 2.2.4 Local Search (`src/search/alns/operators/LocalSearch/`)
Applied periodically (every `ls_interval` iterations) during ALNS to find local optima. **Trade-off Note**: The operators below significantly increase the computational cost per ALNS iteration, but they dramatically improve the quality of each move, accelerating convergence to the Best Known Solution (BKS).

*   **`LocalSearch.cpp` (VND Controller)**: Uses a Variable Neighborhood Descent (VND) loop. It applies operators sequentially. If an operator finds an improvement, the search restarts. It includes a dynamic cap limit (`4 * moves.size()`) to prevent infinite loops and `no_improve = 1` logic to skip re-evaluating operators that just converged.
*   **`AddRequest` / `RemoveRequest`**: Standard $O(N)$ moves to toggle single request assignments.
*   **`PassengerRelocate.cpp` & `ParcelRelocate.cpp`**: Evaluates moving a request to a better position. 
    *   *Algorithm (Best-First)*: Instead of checking `FastRouteCheck` sequentially, it first gathers all $O(1)$ arc-cost bounds, sorts them, and strictly evaluates the highest potential gains first. It uses `thread_local` memory arrays to eliminate dynamic allocations during the intensive scan.
*   **`ReplaceRequest.cpp`**: Attempts to remove one assigned request and insert one unassigned request into the newly freed route in a single atomic operation. 
    *   *Logic*: Computes `net_gain = [cost_saved(r_out) - revenue(r_out)] + [revenue(r_in) - cost_insert(r_in)]`. It heavily reuses the arc-cost bound infrastructure from Relocate to aggressively prune invalid replacements.
*   **`SwapRequest.cpp` (SWAP*)**: Evaluates inter-route exchanges by swapping requests between two different taxis. This is the heaviest but most powerful operator.
    *   *Algorithm*: Employs a Branch and Bound strategy. For each request and taxi, it caches the `Top-K` (K=3) best insertion positions and their approximate costs. When considering swapping $r_1$ and $r_2$, it computes an absolute upper bound: `upper_bound = savings(r_1) + savings(r_2)`. If `upper_bound - min_insert(r_1) - min_insert(r_2) > best_gain`, it proceeds to the expensive exact evaluation; otherwise, the pair is pruned in $O(1)$ time.

#### 2.2.5 Adaptive Mechanism (`Balance`)
*   **Roulette-wheel Selection**: At each iteration, one Destroy and one Repair operator are chosen randomly based on their weights.
*   **Scoring & Decay**: Weights are updated via exponential smoothing: `new_weight = (1 - decay) * old_weight + decay * score` based on outcomes (New Best / Better / Accepted / Rejected).

### 2.3 Mixed Integer Programming Solver (`soict_mip`)
*   **`MIPSolver`**: Translates constraints (time windows, capacities) into linear constraints and solves using CPLEX's branch-and-cut algorithm to prove optimality or provide tight bounds.

---

## 3. Main Algorithm Flow (ALNS)

The main ALNS logic in `ALNS.cpp` is strictly bound to a **Time-Based Simulated Annealing** schedule, optimizing the available CPU time to the very last millisecond.

1.  **Initialization**:
    *   Create an empty `SearchState`.
    *   Generate a random permutation of all requests and greedily insert them.
2.  **Simulated Annealing Loop (Time-Based)**:
    *   Initialize `anchor_temp = config.start_temperature`.
    *   The loop tracks `progress = elapsed_time / time_limit`. It guarantees termination exactly when `progress >= 1.0`.
    *   While `progress < 1.0`:
        *   **Select & Apply**: Apply Destroy and Repair operators.
        *   **Local Search (Optional)**: Every `ls_interval` iterations (e.g. 10), apply the VND local search.
        *   **Acceptance**:
            *   Calculate $\Delta = \text{Candidate Benefit} - \text{Current Benefit}$.
            *   Accept if $\Delta \ge 0$, else accept with probability $P = \exp(\Delta / \text{temperature})$.
        *   **Update**: Update operator weights based on the outcome.
        *   **Reheating (Stagnation Recovery)**: If the search fails to find a new global best solution after `5%` of the total time (e.g., `progress - last_improvement_progress > 0.05`) and we are not in the final 20% of the run, the temperature is forcefully "reheated". `anchor_temp` spikes back to `0.2 * T0`, `anchor_progress` is reset, and the state reverts to `best`. This ensures the algorithm systematically escapes deep local optima without requiring manual temperature tuning.
        *   **Dynamic Cooling**: `temperature` is exponentially decayed using a relative progress window mapped between Reheating anchors. Equation: $T(t) = T_{anchor} \cdot (T_{min} / T_{anchor})^p$ where $p$ is the relative progress.
3.  **Termination**: Return the best solution found.
