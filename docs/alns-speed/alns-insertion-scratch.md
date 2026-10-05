# Fixed insertion workspace

Implemented on 2026-10-05 after the r240 insertion diagnostic. This change only
replaces temporary scan vectors with reusable storage; it preserves request and
taxi order, position enumeration, cost formulas, feasibility checks, gain
thresholds, tie rules and random-number use.

## Storage and lifetime

`InsertionScratch` owns one contiguous vector of `4 × (2R + 2)` doubles, where
`R = passengers + parcels`. Four slices hold pickup deltas, delivery deltas,
adjacent-pair deltas and parcel suffix minima. A valid route has at most `2R`
service nodes; the extra two slots cover insertion gaps and the suffix sentinel.
For r240 this is **15,424 bytes** (about 15.1 KiB).

The initial `SearchState` allocates the block before the first insertion scan.
Candidate and best states share it through a `shared_ptr`; their solution/cache
copies do not copy scratch values or allocate another scratch block. Final
route-pool reconstruction explicitly reuses the same handle. `copy_from` retains
the destination's block, while `swap` transfers it with the instance and routes.
Separately constructed states get independent blocks unless supplied a handle.

The vector is never resized. Every scan overwrites the used delta entries. A
parcel scan first writes its infinity sentinel, then overwrites its used suffix
entries backwards. The former per-scan zero/infinity initialization is therefore
unnecessary. Returned choices own numeric values and retain no buffer pointers.
Capacity overflow or an incompatible supplied workspace is rejected rather than
growing the storage.

Insertion scans sharing a block must be sequential, as they currently are within
one solve. Independent solver invocations have independent blocks. Parallel
scanning of copied states would require separate workspaces. Other solver data
structures can still allocate; this change removes insertion **scan scratch**
allocations specifically. The `shared_ptr` object/control metadata is separate
from the single contiguous allocation holding the four buffers.

## Validation

The new `soict_insertion_scratch_test` counts allocation calls: constructing the
four-buffer block performs one allocation, and scanning already prepared states
performs zero allocations. It exercises direct route scanning, all-taxi best
insertion and top-two insertion options after long/short/empty/regrown routes,
alternates passenger and parcel requests, poisons scratch with NaNs, and compares
direct choices against exhaustive concrete solutions checked by the full
evaluator. It also covers capacity limits, overflow, shared state copies,
destination storage retention, reconstruction and swaps between different
instances. Existing insertion, repair, replacement, swap and route-pool tests
remain part of the full suite.

Baseline: 10/10 CTest tests pass. Changed version: 11/11 pass. The temporary
snapshot initially lacked the three data fixtures needed by Python I/O tests;
copying those unchanged fixtures resolved that setup failure. An initial new-test
compile error called a private node helper; the final test uses the public
instance's node mapping instead.

All five focused tests also pass with AddressSanitizer and UndefinedBehaviorSanitizer:
insertion scratch, ALNS, repair operators, request replacement and route-pool
repair. The active `cmake-build-debug` workspace was rebuilt, and its full suite
passes **12/12**, including the CPLEX MIP test. Its solver sources match the tested
after snapshot; its generated compile rules retain `-O2` and assertions. The
benchmark binaries disable CPLEX, while the workspace binary includes it; the
timing result above comes from the matched benchmark pair.

## Matched runtime evaluation

One instance and one seed, as requested: `r240_p120_m120_s5`, seed 1. Both
uninstrumented variants use GNU 11.4, Debug assertions and effective `-O2`, normal
per-iteration logging, the configured temperature 200, CLI time limit 300
(299 seconds inside ALNS, plus the final route-pool reserve), one sequential
solver process, CPU affinity 2. Each source version was freshly built from a
snapshot. The unchanged baseline's full checker evaluates both output solutions
outside the measured solve.

| Version | Solver seconds | Process wall seconds | Iterations | Benefit |
|---|---:|---:|---:|---:|
| Before | 244.419379 | 244.506387 | 60,000 | 13,279 |
| Fixed scratch | 190.318568 | 190.392434 | 60,000 | 13,279 |

The changed version uses **22.13% less solver time**, saving 54.10 seconds
(1.2843 times the baseline iteration throughput). Both runs stop at temperature;
neither deadline truncates the workload. User CPU time also falls from 244.048356
to 189.967771 seconds.

All **60,001** logged iteration/objective/temperature records match after removing
timestamps, including iteration zero. Final solution bytes are identical:
SHA-256 `d49bf6d9fffbba6113efb671b86afbd2e6b0d3b2eae1abad632cca73877110ac`.
The unchanged baseline full checker confirms benefit 13,279 for both. Route-pool
columns (2,454), explored nodes (2,472,498), final gain (139), and local-search
statistics also agree.

A single ordered before/after pair is a runtime diagnostic for this instance/seed;
it does not estimate run-to-run variability or performance on other instances.
CPU affinity pins execution but does not reserve the CPU or control frequency.
No compilation or other solver experiment was launched during the timed pair.

Artifacts: insertion-scratch-20261005 (`results/diagnostics/insertion-scratch-20261005/`).
These include the before/after source archives and hashes, the patch relative to
the actual pre-change workspace, compiler commands, test logs, benchmark runner,
commands, raw iteration logs, solutions, full-checker output and comparison
metadata. Earlier insertion findings are in
r240_insertion_deep_20261005 (`results/profiling/r240_insertion_deep_20261005/report.md`).
