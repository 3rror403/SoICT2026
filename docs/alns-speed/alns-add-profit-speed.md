# Shared AddRequest / ProfitInsertion speed work

## Retained mechanism: immutable request table

Both operators call `BestInsertionCache::best`. It previously constructed pickup
and delivery segments, revenue and parcel weight on every request query, including
queries satisfied by route-cache hits. The earlier r240 seed-1 diagnostic counted
117,952,615 such constructions in ProfitInsertion and 34,021,602 in AddRequest.

`EmptyTaxiIndex` now precomputes one fixed-size table of `RequestSegments` at
construction. `BestInsertionCache::best` reads a const reference to the current
state's table entry. The table contains passenger/parcel type, both single-node
segments, revenue and weight. It has exactly one record per request and is never
resized or rebuilt after construction. Existing shared ownership of the index
means incumbent, candidate and best states reuse the same allocation.

On this 64-bit build, each record is 184 bytes: **44,160 bytes for r240**, in one
additional fixed allocation. The existing four-buffer insertion scratch remains
15,424 bytes and is also reused without reallocation. These payload sizes exclude
object/control metadata and other solver caches. Independent solves have their
own storage. As already required by precomputed empty-taxi choices, the instance
must remain unchanged while its index is in use.

The table is used only by the indexed best-insertion cache shared by AddRequest
and ProfitInsertion. Callers without an index retain the original fallback.
Direct GreedyInsertion, replacement and top-two scanners retain their original
request-data builder. Active-taxi traversal, route-version and threshold cache
invalidation, route scoring, position enumeration, feasibility checks, ties and
RNG use are unchanged.

The retained code changes are in `InsertionSearch.hpp` and `InsertionSearch.cpp`.
The regression test in `ALNSTest.cpp` checks stable table addresses, all-active and
empty-taxi transitions, lazy index completion, restoration, and one cache reused
with a different instance whose incoming parcel is overweight. The no-CPLEX CTest
suite passes 11/11, including independent full-evaluator request-choice and
ProfitInsertion trajectory comparisons.

All five focused AddressSanitizer/UndefinedBehaviorSanitizer tests pass (ALNS,
insertion scratch, repair operators, replacement, route-pool repair). The active
workspace executable was rebuilt, and its full suite passes 12/12 including MIP.
The benchmark/profile builds disable CPLEX; the workspace build includes it.

## Runtime evidence

| Variant | Solver seconds | Iterations | Benefit | Retained |
|---|---:|---:|---:|---|
| Fixed-scratch control | 196.402077 | 60,000 | 13,279 | Baseline |
| Empty-taxi guard only | 196.756448 | 60,000 | 13,279 | No |
| Immutable request table only | 188.977597 | 60,000 | 13,279 | Yes |

The request-table run uses **3.78% less time** than the control, a saving of
7.42 seconds. Its user CPU time is 188.601468 versus 196.028609 seconds. The guard
is absent from the retained variant: these are two separate mechanisms, not a
combined treatment.

All 60,001 iteration/objective/temperature records, the complete stderr after
stripping timing fields, and final solution bytes match the control. The unchanged
full checker confirms benefit 13,279. Solution SHA-256:
`d49bf6d9fffbba6113efb671b86afbd2e6b0d3b2eae1abad632cca73877110ac`.

Only `r240_p120_m120_s5`, seed 1 was evaluated. Builds use GNU 11.4, Debug
assertions, effective `-O2`, CPLEX disabled, configured temperature 200, CLI limit
300 (internal 299), normal logging, one solver process at a time, and CPU affinity
2. The fresh control collected for the guard was reused for the subsequent
request-table run; their timing runs were not adjacent because the guard trial
and compilation intervened. This is an observed single-run difference, not an
estimate of variability or a controlled per-component causal effect. No compiler
work or other solver experiment was launched during a timed solve.

Retained-variant artifacts:
add-profit-request-data-20261005 (`results/diagnostics/add-profit-request-data-20261005/`).

The updated operator distribution is recorded separately in
r240_add_profit_request_data_20261005 (`results/profiling/r240_add_profit_request_data_20261005/report.md`).
Its coarse timers are in a temporary source copy; instrumented phase seconds
are descriptive and are not substituted for the uninstrumented runtime above.

| Component | Instrumented seconds | Share |
|---|---:|---:|
| AddRequest | 54.800 | 28.24% |
| ProfitInsertion | 48.455 | 24.97% |

The full instrumented run takes 194.035 seconds. These raw component times exceed
the preceding fixed-scratch profile's 53.891 / 47.887 seconds, whose total was
188.591 seconds; separately collected profiles therefore do not establish a
per-operator speedup. The two components still account for 53.21% of runtime.
The retained change removes repeated descriptor construction, but its observed
end-to-end improvement is modest and not a robust speed estimate across runs.

## Rejected mechanism: all taxis active

`AddRequest` and `ProfitInsertion` both call `BestInsertionCache::best`. Before
checking active-route cache entries, it calls `add_empty_taxis`, which walks a
request's precomputed empty-taxi choices and discards choices whose taxi is
active. When every taxi is active, all entries are discarded.

The trial guard compared `active_taxis().size()` with `taxi_count()` and skipped that
choice-list traversal when they are equal. Lazy `EmptyTaxiIndex::ensure` still
runs, and the helper still returns the same indexed/fallback status. With at
least one empty taxi, the existing sorted-choice traversal is used unchanged.
All active-route scans, route-version/threshold cache rules, insertion scoring,
pickup/delivery order, tie rules and RNG use are retained.

The trial changed only `InsertionSearch.cpp` and its regression test. It used the
existing active-taxi list and added no allocation or persistent storage.

The earlier exact diagnostic counted 431,270,541 empty-choice visits in
AddRequest; at least 431,124,821 occurred while all 20 taxis were active. This
identifies avoidable work, rather than a measured timing breakdown of the guard.
ProfitInsertion also shares the path, but its repair states can have empty taxis,
so its choice-list traversal remains necessary in those states.

The guard's complete matched run preserved behavior but was 0.18% slower than the
control. This small difference does not establish a slowdown, and it provides no
measurable speed benefit. The guard was removed before testing the immutable table.
The useful all-active/lazy-index transition regression remains in the test suite.

## Guard validation and artifacts

The new regression covers a fully active state with an incomplete empty-taxi
index, verifies lazy completion, removes complete requests to create an empty
taxi, compares indexed choices against the unindexed scanner, and restores the
fully active state. The full no-CPLEX CTest suite passes 11/11, including the
independent full-evaluator repair comparisons and ProfitInsertion trajectories.

The evaluation uses only `r240_p120_m120_s5`, seed 1. A fresh before/after pair
uses GNU 11.4, Debug assertions, effective `-O2`, CPLEX disabled, configured
temperature 200, CLI time limit 300 (internal 299), normal logging, one solver
process at a time, and CPU affinity 2. The baseline is the verified fixed-scratch
binary from the preceding task. All other solver sources match its snapshot.
The checker runs outside the timed solves.

The rejected guard's source, patch and full measurements remain in
add-profit-empty-taxi-20261005 (`results/diagnostics/add-profit-empty-taxi-20261005/`).
Source archives, source/binary hashes, exact compiler commands, the patch relative
to the actual pre-task source, test logs, commands, trajectories, solution bytes
and full-checker logs are preserved.

This is a single-instance, single-seed speed diagnostic. CPU affinity does not
reserve a CPU or control frequency; an ordered pair does not measure variability.

## Subsequent speed campaign

The later authorized caching, exact pruning and granular-neighborhood trials
are recorded in [the speed campaign](alns-speed-campaign.md). Its retained
implementation and fresh comparison supersede the runtime distribution above;
the earlier measurements remain historical evidence for the request-table change.
