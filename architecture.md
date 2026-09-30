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

*   **`Instance`** (`src/model/Instance.hpp`): Defines the immutable parameters of a problem instance. It stores arrays of `Taxi` (fleet with capacity), `Passenger`, and `Parcel`. It holds a precalculated, flattened distance/time matrix (`travel_time`, `travel_cost`).
    *   *Nodes Layout*: `0` to `K-1` for taxi origins; `K` to `K+N-1` for passenger pickups; `K+N` to `K+2N-1` for passenger drop-offs; similarly for parcels. This continuous indexing enables fast array-based lookups for matrices.
*   **`Solution`**: Represents a completed sequence of node visits for each taxi.
*   **`State`**: Used during the search to represent a partially constructed solution. It keeps track of which requests are `unassigned` and provides fast query methods to check constraints and evaluate objective value changes.

### 2.2 ALNS Solver (`soict_alns`)
This library implements the Adaptive Large Neighborhood Search (ALNS) meta-heuristic. This is the primary engine for finding high-quality solutions efficiently.

*   **`ALNSSolver` & `ALNS`**: The main execution loop of the algorithm. It alternates between destroying a portion of the current solution and repairing it.
*   **`SearchState`**: A more advanced representation of `State` optimized for the ALNS search process. It relies on caching mechanisms (like `RouteCache`) and tracking active taxis.

#### 2.2.1 Advanced Route Evaluation (`RouteCache` & `FastRouteCheck`)
Evaluating whether an insertion (e.g., picking up a parcel at node A and delivering at node B) is feasible under strict time window and capacity constraints is computationally expensive. The project addresses this with highly optimized data structures:

*   **`Segment` (`RouteCache.hpp`)**: A struct summarizing a sequence of visited nodes. It records:
    *   `duration`, `cost`
    *   `ready`: The earliest time the vehicle can leave the last node of the segment.
    *   `latest_input`: The latest time the vehicle can arrive at the first node without violating any time windows within the segment.
    *   `load_change`, `min_load`, `max_load`: Changes and extrema of parcel weight.
*   **`RouteCache`**: 
    *   Uses a **Disjoint Sparse Table (DST)** to cache the `Segment` properties for all power-of-two contiguous sub-routes.
    *   *Logic*: The DST allows querying the combined `Segment` statistics of *any* arbitrary range `[first, last]` in $O(1)$ time. 
*   **`FastRouteCheck`**: Uses the properties of `Segment`s to quickly check if inserting a node into a route maintains feasibility (time windows and capacity) by accumulating segment bounds left-to-right without iterating through individual nodes.

#### 2.2.2 Destroy Operators (`src/search/alns/operators/Destroy/`)
These operators take a complete `SearchState` and remove a configurable number of requests (`q`), putting them into an unassigned pool.

*   **`RandomRemoval`**: Randomly picks `q` assigned requests and removes them. Ensures diversity.
*   **`WorstRemoval`**: Calculates the *removal gain* (how much cost is saved / benefit gained by removing the request) for all assigned requests. Removes the `q` requests with the highest removal gain (i.e., those that are most expensive to serve).
*   **`RelatedRemoval`** (Shaw Removal): Removes a cluster of requests that are "similar". 
    *   *Logic*: It selects a random anchor request. Then, it removes requests that are geographically and temporally close to the anchor. Distance is measured by combining travel time between pickup nodes and the difference in their earliest time windows.

#### 2.2.3 Repair Operators (`src/search/alns/operators/Repair/`)
These operators take the unassigned requests and re-insert them into the routes heuristically. They rely heavily on the `InsertionSearch` subsystem.

*   **`InsertionSearch`**: The core logic that iterates over all taxis and all possible insertion positions `(pickup_pos, dropoff_pos)` to find the best valid insertion for a request.
    *   *Optimization*: It pre-filters insertions by computing a lower bound of the cost increase using just the replaced adjacent arcs. If this lower bound exceeds the current best found insertion, the heavy constraint check (`FastRouteCheck`) is skipped entirely.
    *   It utilizes `BestInsertionCache` and `RepairInsertionCache` so that if a taxi's route hasn't changed since the last iteration, its previously calculated best insertion positions can be reused instantly.
*   **`GreedyInsertion`**: At each step, evaluates the best valid insertion for *each* unassigned request. It then inserts the request that provides the highest immediate gain.
*   **`RegretInsertion`**: 
    *   *Logic*: For each unassigned request, it calculates the *regret value*: the difference in gain between its best insertion and its second-best insertion.
    *   It inserts the request with the highest regret. This prioritizes requests that have one good insertion option but very poor alternative options, preventing them from being left out.

#### 2.2.4 Local Search (`src/search/alns/operators/LocalSearch/`)
Applied periodically during the ALNS loop to find local optima.

*   **`AddRequest` / `RemoveRequest`**: Standard moves to toggle request assignments.
*   **`PassengerRelocate` / `ParcelRelocate`**: Takes a request currently assigned to a route and attempts to move it to a *better* position within the same route or a different taxi's route. This relies heavily on the fast evaluation capabilities of `RouteCache`.

#### 2.2.5 Adaptive Mechanism (`Balance`)
*   **Roulette-wheel Selection**: At each ALNS iteration, one Destroy and one Repair operator are chosen randomly based on their weights.
*   **Scoring & Decay**: 
    *   If the operators find a new global best solution, they receive a high score (e.g., `score_new_best = 33`).
    *   If they find a better solution than the current one: `score_better = 9`.
    *   If they find a worse solution that is still accepted by Simulated Annealing: `score_accepted = 13`.
    *   Weights are updated via exponential smoothing: `new_weight = (1 - decay) * old_weight + decay * score`.
*   **Thompson Sampling**: The `Balance` class can optionally use Thompson Sampling instead of raw weights to balance exploration and exploitation of the operator space.

### 2.3 Mixed Integer Programming Solver (`soict_mip`)
*   **`MIPSolver`**: If IBM ILOG CPLEX is installed, this module provides an exact mathematical formulation of the problem. It translates constraints (time windows, capacities, network flow) into linear constraints and solves it using CPLEX's branch-and-cut algorithm. It's typically used for proving optimality on small instances or providing tight lower/upper bounds.

---

## 3. Main Algorithm Flow (ALNS)

1.  **Initialization**:
    *   Create an empty `SearchState`.
    *   Generate a random permutation of all requests.
    *   Greedily insert requests into the state one by one.
2.  **Simulated Annealing Loop**:
    *   Initialize `temperature = config.start_temperature`.
    *   While `temperature > minimum_temperature`:
        *   **Select** a pair of (Destroy, Repair) operators based on historical weights (`Balance`).
        *   **Apply Destroy**: Remove `q` requests from the `SearchState`.
        *   **Apply Repair**: Re-insert the `q` requests to create a candidate state.
        *   **Local Search (Optional)**: Every `ls_interval` iterations, apply `PassengerRelocate` and `ParcelRelocate` until a local minimum is reached.
        *   **Acceptance**:
            *   Calculate $\Delta = \text{Candidate Benefit} - \text{Current Benefit}$.
            *   If $\Delta \ge 0$, accept the candidate.
            *   If $\Delta < 0$, accept with probability $P = \exp(\Delta / \text{temperature})$.
        *   **Update**: Update operator weights based on the outcome (New Best / Better / Accepted / Rejected).
        *   **Cooling**: `temperature = temperature * config.cooling_rate`.
3.  **Termination**: Return the best solution found during the search.
