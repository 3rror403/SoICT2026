# Documentation

| Directory | Contents |
| --- | --- |
| [Problem specification](problem/README.md) | Official statement, mathematical specification, and input/output. |
| [ALNS operators](alns-operators/README.md) | Destroy, repair, local search, and the route-pool walkthrough. |
| [ALNS speed](alns-speed/README.md) | Profiling, insertion optimizations, and runtime experiments. |
| [Evaluation](evaluation/README.md) | Experiment protocol, operator trials, and dataset distribution. |
| [Exact methods](exact/README.md) | CPLEX formulation and the r240 optimality proof. |
| [Submission](submission/README.md) | Contest packaging and upload instructions. |
| [Reference papers](paper/README.md) | Supporting literature. |

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j2
```

CPLEX is optional. The public checkout builds without the private, ignored
`tests/` directory; local regression targets are registered when those test
sources are present. Failed route-generation diagnostic executables are excluded
from the default build. See [the submission guide](submission/submission.md)
for generating and checking the standalone C++17 CMS file.

Raw experiment outputs and regression sources remain local under ignored
`results/` and `tests/`. Paths to those files in the documents are provenance
references; they are not part of the public checkout.

## Algorithm goals

The official input/output syntax is in [probation_state_problem.txt](problem/probation_state_problem.txt). These are development targets; the [problem statement](problem/Problem%20description.pdf) does not specify instance sizes or a runtime limit. Size means total requests, `N + M`.

| Stage | Target size | Goal |
| --- | ---: | --- |
| Exact check | Small instances (up to 20) | Match the MIP objective whenever the MIP proves optimality; independently verify every route. |
| Initial solver | Up to 100 | Produce feasible passenger and parcel routes and improve contest benefit over a simple insertion baseline. Record benefit and runtime. |
| Scale-up | 200-500, then 1,000 | Measure benefit and runtime while preserving feasibility. |
| Final target | 2,000 | Produce a feasible solution and maximize benefit within a provisional 5-minute budget. |

Revise the scale and runtime targets when official limits become available.

The active [probation set](../data/probation/README.md) contains eight supplied instances. The official input/output format is documented in [input-output.md](problem/input-output.md).
The exact CPLEX solver for 10-request cases is documented in [mip.md](exact/mip.md).
Heuristic run records and best-known routes are documented in [evaluation.md](evaluation/evaluation.md).

Open [route-pool-tree.html](alns-operators/route-pool-tree.html) in a browser for an offline,
interactive RoutePoolRepair walkthrough. Step through taxi-route choices,
occupied requests, upper bounds, pruning, and backtracking, or compare bounding
policies. It uses small illustrative pools rather than a recorded r240 run;
keep the accompanying `route-pool-tree.js` beside the HTML file.

The single-file contest packager is documented in [submission.md](submission/submission.md).
