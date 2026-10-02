# Clean-cut suffix exchange

`--clean-cut-2opt-star` adds an optional fifth local search move after remove,
add, passenger relocation, and parcel relocation. It adapts the mechanism from
`origin/nguyen_test_02` at `c16b2b6`; it is disabled by default.

The move exchanges two active taxis' suffixes. A cut must leave no request
crossing the boundary: it cannot split a passenger pair or an open parcel.
Open parcels are counted explicitly. Directed and nonmetric matrices are
supported; both new routes are checked for timing and capacity before applying
an exchange. Unchanged suffixes retain their order and internal arcs, so the
candidate cost gain depends only on the two connector arcs. End-of-route cuts
respect the problem's absence of a return trip.

The move finds the best exchange for the first improving taxi pair and repeats
until no active pair improves. Application validates request closure again,
checks both completed routes, and recomputes their cost change. Both old request
assignments are cleared before either new route is assigned. Route versions are
refreshed, active taxi IDs stay sorted, and an emptied taxi retains an empty
cache. Failed exchanges do not modify the state. `--check` retains the independent
full-solution check.

```sh
./cmake-build-debug/SoICT2026 --instance data/probation/r95_p47_m48_s7.txt --seed 1 --temp 200 --time-limit 60 --clean-cut-2opt-star --out solution.sol
```

The evaluator accepts `--solver-temp 200` and
`--solver-clean-cut-2opt-star`. End-of-run `clean_cut_calls`, `clean_cut_moves`,
and `clean_cut_gain` diagnostics describe local search activity in proposed
candidates, including candidates that ALNS later rejects. They do not measure
incumbent improvements.

## Initial comparison

On `r95_p47_m48_s7.txt`, matched seeds 1–10 with initial temperature 200 gave:

| Metric | Baseline | With clean cut |
| --- | ---: | ---: |
| Mean benefit | 5553.7 | 5596.2 |
| Best benefit | 5651 | 5682 |
| Mean wall time (s) | 33.69 | 34.07 |
| Iterations per second | 1780.97 | 1760.96 |
| Feasible runs | 10/10 | 10/10 |

The candidate won seven matched seeds and lost three. Mean benefit increased
by 0.77%; measured mean runtime increased by 1.14%. Both variants used the same
Release binary, logging, cooling, and local search interval, with ten
single-threaded processes per batch. Every run finished 60,000 iterations
before the 60-second hard timeout. The reference solution was checked at benefit
5632 and was not used as a warm start or overwritten.

This is evidence on one selected instance. The paired bootstrap 95% percentile
interval for mean benefit change is [-2.5, 87.1], so the ten seeds do not firmly
establish a positive expected gain. Keep the mechanism opt-in until a broader
comparison supports a default change.

The [experiment report](../results/clean-cut-r95-t200-20261003/report.md) records
all seed pairs, source and binary hashes, settings, logs, isolated best routes,
and the analysis script. Correctness validation covered 11,400 exchanges over
120 directed/nonmetric fixtures against independent full solution evaluation,
alongside the existing ALNS and probation I/O tests.
