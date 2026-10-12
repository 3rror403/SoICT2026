# Generation and fleet selection on large synthetic instances

The current solver exceeds the original BKS targets on all nine synthetic instances in **27/27 runs across seeds 1–3 within 90 seconds**. The slowest process takes **87.18 seconds**. Twelve longer runs also satisfy the 180-second limit; the three largest instances gain **1,728–2,479 points** over matched 90-second runs. Every fleet passes the complete C++ checker and an independent Python simulation. Probation is excluded from this cohort.

Detailed seed-level results, updated BKS records, executable hashes, resource limits, and reproduction commands are in [SYNTHETIC_RESULTS.md](SYNTHETIC_RESULTS.md). Comparisons use the frozen original targets in `results/diagnostics/synthetic-speed-20261012/targets.csv`, rather than the stronger live BKS ledger.

## Candidate generation

The search generates route alternatives before deciding the fleet. All accepted runs use zero outer adaptive ALNS iterations. Generation still uses lightweight destroy/repair and local refinement; it is a heuristic search rather than exhaustive route enumeration.

The first pass enumerates feasible forward prefixes independently for each taxi, under a shared two-second deadline and at most one million prefixes per taxi. Completed positive-profit routes are stored by taxi and served-request set, retaining the best sequence for each set. The raw enumeration bank keeps up to 400 routes per taxi. This cap limits alternatives even when enumeration finishes.

A reusable pickup index avoids rebuilding candidate lists. Optimistic parcel-delivery bounds use completed shortest-path landmark searches on the original directed travel graph. These remain safe on directed, nonmetric inputs: shortest paths underestimate direct travel, while service, precedence, and capacity restrictions are omitted from the bound. Incomplete landmark computations are discarded. Route profits always use original travel costs and revenues.

For larger banks or incomplete enumeration, generation alternates three procedures:

1. Perturb the best feasible fleet, then repair it using cached positive insertions. Fixed-point completion revisits requests that become profitable after later insertions. Occasional local refinement supplies further route alternatives.
2. Recombine the routes using a short fleet-master call, then feed the selected fleet back into generation.
3. Generate routes using request prices from the full-bank LP. Alternate one-taxi insertion/removal/replacement search with diversified forward DFS. Vary prices and event ordering to expose complementary routes; store these alternatives with their true profits outside the raw top-profit cap.

Insertion choices refresh only when a request is examined; unchanged route versions reuse their cached choices. Default sampling refinement runs three cycles every two proposals, using removal, insertion, clean-cut suffix exchange, and replacement. Relocation and swap remain available in the outer ALNS fallback, final polishing, and `--full-fleet-refinement` ablations.

Short fixed reserves leave time for final selection and polishing. Refinement diagnostics include recovery of profit lost during perturbation, so their summed gains are not the final objective improvement.

Priced DFS does not solve the unrestricted single-taxi pricing problem to optimality when its prefix or time limit stops the search. One-taxi replacements validate the completed move directly: an infeasible removal-only intermediate route must not reject a feasible replacement on a nonmetric graph.

## Fleet master and bounds

The master is weighted set packing: choose at most one route per taxi and serve each request at most once. Taxis may remain unused. Each selection is checked for resource exclusivity and evaluated as a complete fleet.

Small banks retain native request-price branch and bound. The native LP master reuses child bases; large native instances also have coordinated local master neighborhoods. CPLEX is an optional backend for full-bank request prices and larger masters. It uses one thread, a feasible incumbent start, and the remaining wall-clock budget.

Dense full-bank solves often spent their allowance at the root. Ranking each taxi's routes by profit minus request prices made smaller masters easier. The progressive master preserves incumbent routes, tries 32 alternatives per taxi, widens to 64, and considers the complete bank if time permits. Short feedback calls stop after the first tier to leave more time for generation.

An optimum of a ranked subset certifies only that subset. `route_pool_complete=1` requires completion of the whole current bank master; it never certifies every feasible route of the original problem. A time limit or heuristic neighborhood does not establish bank optimality. Final polishing can also add routes after a master call.

Any nonnegative request prices give the independently checkable full-bank upper bound:

```text
sum(request prices) + sum over taxis max(0, maximum reduced route profit).
```

This bound can overestimate the best conflict-free fleet in the bank; that is the intended relaxation and permits safe pruning. It need not bound the original problem when feasible routes are missing from the bank. Restricted-master bounds are never treated as full-bank bounds.

A cached price vector is reused only while the bank is unchanged; adding, improving, or removing a column invalidates it. Native pruning adds numerical guards and rounds down only when profits are exactly integral. Global optimality remains unproved because generation and retention are bounded.

## What worked and what failed

- **Fleet feedback and price-guided generation worked.** Useful complementary routes mattered more than individually profitable routes. Cheap beam and insertion-only generators sometimes produced banks whose own upper bound was below BKS.
- **Selective masters worked.** Full-bank root solves and parameter tuning consumed time with little quality return. Progressive ranked banks returned useful fleets quickly.
- **Less repeated computation worked.** Lazy insertion refresh, reusable generation indexes, lighter refinement, and buffered input made the 90-second target attainable. Shortening sampling to two–four seconds alone lost quality; compiler improvements alone were insufficient.
- **More generation improved the largest cases.** A 170-second generation allowance within a 180-second CLI budget improved all three largest objectives and reduced their observed memory peaks compared with the 160-second trials. A longer budget did not improve every medium-instance run.

## Measured outcomes

The build uses GNU C++ 11.4.0, Debug with effective O3, strict floating-point semantics, link-time optimization, and CPLEX. Whole-process time includes parsing, bank export, solution output, and cleanup; independent validation follows. The solver reads only the instance and configuration, without BKS solutions or precomputed banks.

The 90-second acceptance covers all nine inputs and seeds 1–3, with peak solver RSS **636.33 MiB**. Matched seed-1 trials on the largest cases give:

| Instance | 90-second objective | 180-second objective with 170s generation | Gain | Process seconds |
| --- | ---: | ---: | ---: | ---: |
| r2000_p1000_m1000_s1 | 132,744 | 135,071 | +2,327 | 175.32 |
| r2000_p1000_m1000_s2 | 134,468 | 136,196 | +1,728 | 176.61 |
| r2000_p1000_m1000_s3 | 132,815 | 135,294 | +2,479 | 176.04 |

These three longer trials stay below 775 MiB RSS. The full nine-instance longer cohort uses 160 seconds of generation and takes at most **178.45s**, with peak RSS **1,260.73 MiB**. Its r1000 s1 result falls from 62,457 in the shorter run to 62,438. BKS export retains the strongest checked fleet rather than assuming a longer run is better.

## Build and run

CPLEX is required for the measured large-instance performance configuration; the portable native backend remains available. Build the current configuration with O3 and link-time optimization:

```sh
cmake -S . -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug \
  -DSOICT_CPLEX_ROUTE_POOL=ON -DSOICT_AGGRESSIVE_OPTIMIZATION=ON \
  -DCPLEX_ROOT=/opt/ibm/ILOG/CPLEX_Studio2211 \
  -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON
cmake --build cmake-build-debug --target SoICT2026 -j2
./cmake-build-debug/SoICT2026 --instance data/synthetic/r2000_p1000_m1000_s1.txt \
  --seed 1 --time-limit 90 --temp 200
```

For more candidate generation on the three largest cases, use `--time-limit 180 --generation-seconds 170`. The full nine-instance longer cohort uses `--generation-seconds 160`.

`--generation-seconds`, `--enumeration-seconds`, `--path-prefix-limit`, and route-pool caps control generation. `--disable-candidate-sampling` restores the outer ALNS fallback when enumeration is incomplete. `--disable-path-generation` selects ALNS before final fleet repair. The CLI accounts for parsing and reserves output/cleanup time. Deadlines remain cooperative, so whole-process limits are also checked externally.

## Validation and artifacts

Both independent checkers accepted all 39 final search runs. The 13 regression targets and focused AddressSanitizer/UndefinedBehaviorSanitizer checks pass. Fixtures cover directed/nonmetric route generation, exhaustive insertion endpoints, cache restoration, and abstract fleet packing. Parser comparisons matched 68,757,264 matrix entries across two large instances.

Validation uses two shared solver slots, each running a solver process, with address-space caps, CPU-time caps, external timeouts, and available-memory checks before launch. The independent Python checker streams into compact numeric arrays, peaking at 276 MiB on the largest instance.

The acceptance audit records executable/source identity, input and solution hashes, both checker outputs, CPU affinity, measured process limits, and master completion. Evidence is retained in `results/diagnostics/synthetic-speed-20261012/`; retired executable copies and route banks were removed, and historical traces were losslessly compressed.

All nine synthetic BKS records were refreshed after independent checks, preserving input hashes and `is_optimal=0`. Producing settings and verification hashes remain in [provenance.json](data/synthetic/best-known/provenance.json). These results establish measured performance on the specified instances, seeds, build, and machine; global optimality and unseen-instance performance remain unproved.
