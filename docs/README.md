# Algorithm goals

The official input/output syntax is in [probation_state_problem.txt](probation_state_problem.txt). These are development targets; the [problem statement](Problem%20description.pdf) does not specify instance sizes or a runtime limit. Size means total requests, `N + M`.

| Stage | Target size | Goal |
| --- | ---: | --- |
| Exact check | Small instances (up to 20) | Match the MIP objective whenever the MIP proves optimality; independently verify every route. |
| Initial solver | Up to 100 | Produce feasible passenger and parcel routes and improve contest benefit over a simple insertion baseline. Record benefit and runtime. |
| Scale-up | 200-500, then 1,000 | Measure benefit and runtime while preserving feasibility. |
| Final target | 2,000 | Produce a feasible solution and maximize benefit within a provisional 5-minute budget. |

Revise the scale and runtime targets when official limits become available.

The active [probation set](../data/probation/README.md) contains eight supplied instances. The official input/output format is documented in [input-output.md](input-output.md).
The exact CPLEX solver for 10-request cases is documented in [mip.md](mip.md).
Heuristic run records and best-known routes are documented in [evaluation.md](evaluation.md).

The single-file contest packager is documented in [submission.md](submission.md).
