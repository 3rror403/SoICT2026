# SoICT2026 Passenger and Parcel Transportation

This repository contains a solver for the SoICT2026 Passenger and Parcel
Transportation problem. It assigns optional passenger and parcel requests to a
fleet of taxis to maximize service revenue minus travel costs. Each route must
respect pickup and delivery rules, time windows, and the taxi's parcel capacity.
Passengers travel directly from pickup to drop-off. Parcels may remain on board
while the taxi serves other requests.

The solver uses LP-guided route recombination (LPRR). Bounded depth-first search
builds a bank of feasible routes for each taxi. LP-guided branch-and-bound
selects a fleet from those banks while ensuring each request is served at most
once. Fleet sampling, request pricing, and local search generate additional
routes for later selection rounds.

## Build and run

The contest submission is a single C++17 source file with an embedded LP solver.
It reads the instance from standard input and writes the solution to standard
output. Run these commands from the repository root. Allow about five minutes
for the sample run.

```bash
g++ -std=c++17 -O2 -DNDEBUG submission/submission.cpp -o submission_solver
./submission_solver < data/probation/r4_p2_m2_s1.txt > sample-submission.sol
```

For more detail on the problem and algorithm, refer to our
[technical report, *LP-Guided Route Recombination for Passenger and Parcel Transportation*](technical_report.pdf).
