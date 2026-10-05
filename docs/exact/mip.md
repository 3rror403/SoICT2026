# CPLEX MIP for small instances

The solver implementation lives in `src/search/mip`, with its public interface in `include/search/mip`. This keeps exact and heuristic solver methods under the same `search` area.

Build with the installed CPLEX Studio:

```sh
cmake -S . -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build cmake-build-debug
ctest --test-dir cmake-build-debug --output-on-failure
./cmake-build-debug/SoICT2026 solve-mip data/probation/r10_p5_m5_s2.txt --time-limit 300
```

CMake checks headers and libraries before enabling the MIP target. It uses `CPLEX_STUDIO_DIR2211` when that environment variable is set; otherwise its default root is `/opt/ibm/ILOG/CPLEX_Studio2211` on Linux or `C:/Program Files/IBM/ILOG/CPLEX_Studio2211` on Windows. Override the `CPLEX_ROOT` CMake cache option for another location. Windows builds use the MSVC 64-bit libraries in `stat_mdd` for Debug and `stat_mda` for other configurations; CMake also copies the CPLEX DLL beside the executable and MIP test. If the required files are absent or the platform has no supported CPLEX library layout, the core executable still builds without `solve-mip` and without the MIP test.

The command accepts at most 10 total requests. It reports `optimal` only when CPLEX proves optimality; `feasible` means an incumbent exists without a proof. It prints the objective, best bound, and each nonempty route as zero-based node IDs. Use `--out FILE` to save the route in the [solution format](../evaluation/evaluation.md). Its default time limit is 300 seconds and it uses one CPLEX thread. A nonoptimal solve returns exit code 3.

A separate exhaustive-route certificate proves `r240_p120_m120_s5.txt` optimal
at benefit **13,279**. Two complete enumerators agree on all 21,461 retained
route columns, and CPLEX solves the resulting fleet master with matching
incumbent and upper bound. The ordinary `solve-mip` command still has its
ten-request limit. See [the r240 study](../alns-operators/alns-enhancement.md#exact-certificate)
for the model, provenance and independent checks.

## Formulation

Let `D` be the set of `2(N+M)` service nodes. Binary `z_r` selects request `r`; binary `y[k,v]` selects the first service node `v` for taxi `k`; binary `x[u,v]` selects a consecutive pair of service nodes; and binary `f[v]` ends a route at `v`. Every selected service node has exactly one incoming link (taxi start or service arc) and one outgoing link (service arc or route end). Each taxi has at most one start link. Pickup and drop-off share the same `z_r`, so requests are served completely or skipped.

For each passenger, `x[pickup,drop-off] = z_r`; all other arcs out of that pickup and into that drop-off are disabled. For each parcel, a route-order variable puts delivery after pickup. A propagated taxi-label variable ensures that both endpoints are on the same taxi. Increasing route order on every selected arc excludes disconnected cycles.

Arrival and service-start variables propagate travel and service time. At a windowed point, arrival must be no later than the latest time and service starts no earlier than either arrival or the earliest time. Passenger drop-off starts service immediately upon arrival and has no window. Time constraints permit a later symbolic service start, but any route they admit is also feasible under the contest's earliest-start rule: moving service earlier cannot make a latest-arrival constraint fail. The extracted route is checked again by `Solution::evaluate`, which computes the exact earliest-start schedule and waiting.

Parcel load and taxi capacity are propagated from the selected taxi origin along selected arcs. Load is increased at parcel pickup, decreased at delivery, and bounded by the carried taxi capacity at every stop. Passenger service does not change parcel load. The objective maximizes selected-request revenue minus selected travel-link costs; there is no return-to-origin link.

The implementation uses finite upper bounds derived from the largest window endpoint, travel time, service duration, and number of service nodes for its conditional constraints. The exhaustive small-case test compares its optimum with enumerated feasible routes, including a taxi with zero parcel capacity. `src/eval/record_mip_optima.py` solves every manifest case with at most 10 requests and records routes only after an exact proof and independent route validation.
