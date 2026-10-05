# ALNS speed priorities 1–3, 2026-10-04

Implemented only the three requested mechanisms, preserving the pre-existing
uncommitted source and artifact changes. No submission files were regenerated.

1. `ParcelRelocationContext::pickup_prefix` and repair's `scan_route` reject an
   infeasible fixed pickup prefix before considering delivery positions. Later
   nodes cannot undo lateness or a load violation already encountered. Relocation
   within the source route still permits an infeasible removal-only route.
2. `SearchState::copy_from` skips a route cache when its version already matches
   the same taxi in the same instance. Rebuild versions are globally unique;
   cleared routes use version zero. Active taxis, request locations, removal
   caches, the empty-taxi index and benefit are still copied.
3. Accepted `relocate_within_route` and `replace_request` commit the independently
   validated scratch cache directly. A private helper updates request locations,
   active taxis and benefit, retains the validated version for cache invalidation,
   and swaps the old route buffers back into the scratch cache for reuse.

The implementation changes are in `SearchState.hpp`, `SearchState.cpp`,
`ParcelRelocate.cpp` and `InsertionSearch.cpp`. Focused coverage was added to
`tests/ALNSTest.cpp`; `/tests/` is ignored by this checkout's `.gitignore`, so that
test change is visible on disk and in the saved baseline-relative patch, rather
than ordinary `git diff` output.

## Validation

The isolated Release build passed all 10 available CTest tests, including ALNS,
repair, replacement, swap, CleanCut, evaluator and official probation I/O checks.
CPLEX was disabled in both matched builds; no MIP validation is claimed.

```bash
cmake -S . -B /tmp/soict-speed-1-3/after-build -DCMAKE_BUILD_TYPE=Release -DCPLEX_ROOT=/tmp/no-cplex -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build /tmp/soict-speed-1-3/after-build -j 4
ctest --test-dir /tmp/soict-speed-1-3/after-build --output-on-failure -j 4
ctest --test-dir /tmp/soict-speed-1-3/before-build --output-on-failure -R 'soict_(alns|replace_request|repair_operators)_test'
```

The unchanged baseline passed the three focused tests. Added relocation checks
compare every same-route candidate in the directed/nonmetric fixture against
`Solution::evaluate`, verify accepted/rejected mutation and route versions,
exercise populated removal-cache invalidation and repeated restoration, and
explicitly cover overload and lateness at pickup. Existing replacement tests
checked 17,280 candidates, including 1,327 accepted replacements and 48 separated
parcel insertions. Existing nonmetric removal-only traps remain covered.

## Fixed-work measurements

Seven sequential repetitions used the supplied probation cases
`r100_p50_m50_s4` and `r240_p120_m120_s5`, their current saved best-known solutions,
seed `20261004`, GNU C++ 11.4.0, and identical Release flags
`-O3 -DNDEBUG -std=gnu++20 -O2` (effective optimization: `-O2`). Five matched
binaries contain the baseline, each priority individually, or all three changes.
Their order rotates between repetitions. Values below are median milliseconds;
percentages describe this workload only.

| Workload and variant | r100 baseline → variant | r240 baseline → variant |
| --- | ---: | ---: |
| Fixed operator workload, priority 1 only | 252.71 → 238.25 (5.7% less) | 1054.49 → 969.86 (8.0% less) |
| 400,000 state copies, priority 2 only | 252.68 → 40.86 (83.8% less) | 716.92 → 85.15 (88.1% less) |
| 48,000 accepted identity relocations, priority 3 only | 10.53 → 8.38 (20.4% less) | 19.37 → 13.34 (31.1% less) |
| 160,000 accepted replacements, priority 3 only | 42.61 → 33.33 (21.8% less) | 55.75 → 39.44 (29.3% less) |
| Fixed operator workload, all three | 252.71 → 240.54 (4.8% less) | 1054.49 → 998.36 (5.3% less) |

The fixed operator workload performs 1,000 restore/destroy/greedy-repair/
passenger-relocation/parcel-relocation/replacement cycles. Independent checks
and phase-state hashing are inside its timer. It is **not a full ALNS run**.
The copy workload mainly copies unchanged states, with a one-route candidate
and restoration every eighth copy. It includes a served-count scan and hashing.
The direct relocation benchmark reinserts a served request at its current
positions; the direct replacement benchmark repeats improving descriptors from
16 independently prepared partial states. Restoration is outside these direct
mutation timers, which include validation and commit.

Across all five variants and seven repetitions, phase-state hashes, sampled RNG
states, replacement descriptors and final solution bytes matched exactly:
`17326365761389249288` for r100 and `4782609761438423216` for r240. The hash observes
states after entire passenger/parcel relocation phases, not each move accepted
inside those phases, so this does not establish complete internal move-sequence
equality. Independent exhaustive candidate tests provide the complementary
correctness evidence.

Priorities 2 and 3 alone did not improve the mixed workload's median:
priority 2 was 254.22/1060.46 ms and priority 3 was 261.28/1066.71 ms for r100/r240,
versus baseline 252.71/1054.49 ms. Targeted routine gains therefore do not imply
standalone total-runtime gains. The mixed workload performs relatively few state
copies, and shared-machine scheduling, unpinned CPU execution, frequency changes
and code-layout effects limit interpretation of these small differences. This
bounded diagnostic provides no final-objective quality claim or production
time-limit speedup claim.

## Reproduction artifacts

The pre-edit source snapshot is `/tmp/soict-speed-1-3/baseline`; before/after
builds and five benchmark executables are in the same temporary directory.
The durable result folder is
`results/diagnostics/speed-priorities-1-3-20261004/`.
It contains the benchmark source and build/runner scripts, exact compiler
commands, raw timings and medians, source/input/library/binary SHA-256 records,
metadata, final solutions, and `baseline-to-after.patch`. The patch isolates this
task's changes from the earlier uncommitted work, including the ignored test file.
Temporary snapshots and executables may be removed later; the hashes and patch
identify the comparison independently of Git HEAD.

With the recorded temporary baseline/builds still present, rerun from the project
root using:

```bash
python3 results/diagnostics/speed-priorities-1-3-20261004/build_variants.py
python3 -u results/diagnostics/speed-priorities-1-3-20261004/run_benchmark.py
```

There are no known unresolved correctness issues from this scoped change.
