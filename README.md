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

## Build

The main solver requires CMake 3.20 or later and a C++20 compiler. It builds
with a native route-selection backend, so IBM ILOG CPLEX is optional.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
cmake --build build --parallel
```

## Solve and check an instance

From the repository root, run the solver on a bundled sample and validate the
result. The checker prints the solution benefit if the routes are feasible.

```bash
./build/SoICT2026 solve-stdio --seed 1 --time-limit 10 \
  < data/probation/r4_p2_m2_s1.txt > sample.sol
./build/SoICT2026 check-solution data/probation/r4_p2_m2_s1.txt sample.sol
```

`--time-limit` is in seconds. Replace the sample path with your own instance or
run `./build/SoICT2026 --help` to see the available options.

## Build and run the standalone submission

The contest submission is a single C++17 source file with an embedded LP solver.
It reads the instance from standard input and writes the solution to standard
output. Allow about five minutes for the standalone run, even on the sample.

```bash
g++ -std=c++17 -O2 -DNDEBUG submission/submission.cpp -o submission_solver
./submission_solver < data/probation/r4_p2_m2_s1.txt > sample-submission.sol
./build/SoICT2026 check-solution data/probation/r4_p2_m2_s1.txt sample-submission.sol
```

For more detail on the problem and algorithm, refer to our
[technical report, *LP-Guided Route Recombination for Passenger and Parcel Transportation*](technical_report.pdf).
