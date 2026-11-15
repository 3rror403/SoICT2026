# Algorithm history, experiments, and V5 parcel-bundle integration

This note separates the repository's V1–V5 releases from the local research
labels E1/H1/A1/P1/P2/P3. They are different version sequences. Repository
history was read from Git; historical experiments below reuse their retained
records and were not all rerun in this session.

The selected change is **V5 + the conservative P2 parcel-bundle neighborhood**.
It improves one certified 100-parcel counterexample from 0 to 1,999.801 while
tying V5 on the two ordinary inputs tested. This is a specific demonstrated
improvement, not an estimate of hidden-test win rate.

## 1. Repository releases through V5

| Release | Commit | Main methods/change | Evidence and qualification |
| --- | --- | --- | --- |
| V1, 2026-11-07 | `7c0fa842fd446d1dae27c1eac539721380937313` | DFS route generation; a capped bank keyed by served-request sets; fleet rebuilding and local improvement; priced alternatives; LP-based route-set packing with native branching, ranked and group masters. | Inspected historical source. No fresh V1-versus-V5 benchmark was run. |
| V2, 2026-11-09 | `1a068aa368ad6be83d48f3c28f8c9698370857bf` | Invalidate replacement-cache versions before timed refresh and on interruption; change group-master and long-generation dispatch thresholds from 96 to 64 taxis. | Verified the V1→V2 diff. These are taxi thresholds, not customer limits. |
| V3, 2026-11-13 | `e6645d63664cd8c9d6c349ac4ac53e416cb30474` | Readability/restructuring, descriptive state and routine names, named numerical/search constants, and explicit generation/selection/pricing phases. The route-bank/master architecture remains the base of the local research. | Commit title and source inspection. Do not interpret the large textual rewrite as a measured score increase. |
| V4, 2026-11-14 | `f99d42e517c6f83be3cf8a450faaa4f6d17f403e` | Embedded HSOL relaxations and stronger bounds; parcel-pair route generation; retention of useful parcel routes; multiple starting fleets; memory corrections and removal of duplicate route/matrix storage. | Git commit description and source. These features were not independently ablated in the present experiment. |
| V5, 2026-11-15 | `ee67c9f0fa741055e9b189f50cfdf16b9de631d1` | Parcel relocation and swap inside ALNS, route refill, deadline-aware request prices, compact storage and memory guards. Removes the independent multistart wrapper and global parcel-pair pool; keeps parcel delivery refinement and atomic two-parcel insertion in final refinement. | Exact frozen source used as the new benchmark baseline. Preserves embedded HSOL/C++17 packaging. |

V5 distinguishes a genuinely complete uncapped route probe from stopped or
truncated generation. On inputs where this certifies the bank, its driver can
skip extended rebuilding. The supplied 200+200 input took 14.20 s with V5 versus
114.98 s with local P2 in an earlier serial pair, both scoring 32,012. This is
largely a scheduling/certification observation; it does not isolate HSOL's speed.
Hard inputs can still use nearly the entire 298-second V5 budget.

## 2. Local research before the V5 merge

All algorithm experiments were isolated before comparison. Component results,
short search-only screens, and actual standalone-driver scores are distinguished.
The final supplied scope is at most **250 passengers and 250 parcels separately**;
early 600–1,000-request results are historical stress tests outside this scope.

| Direction | Mechanism | Observed result | Decision for this V5 integration |
| --- | --- | --- | --- |
| Indexed bank eviction + pair bootstrap | Ordered index replaces repeated capped-bank minimum scans; a bounded complete two-request seed escapes negative singleton starts. | Synthetic fixed-work eviction workload: 6.76× at cap 400 and 12.91× at cap 1,000. Fifteen full-driver probation pairs had identical solutions; overall runtime changes were mixed. | Retain as historical evidence only; do not copy the old bank implementation over V5. |
| Fast input reader | Reduce token/parsing overhead while preserving input semantics. | Covered by parser/regression checks; later profiling made master selection and route diversity higher priorities. | Keep V5's reader; no new parser change. |
| E1 | Build and retain a feasible greedy packing before potentially expiring LP work. | A 12,000-column component retained 19.016 versus 0 under 5/20/50-ms LP allowances. Extended full-driver stress cohort: one input win, two ties, two losses; sum of input-mean deltas −243. Those larger inputs exceed the final scope. Supplied in-range 200+200: both 32,012. | A timeout safeguard is demonstrated; a general scoring gain is not. Do not transplant the old master. |
| E2 / H2 | Avoid full-bank price LP before the K49–64 restricted master; use cheaper request prices and widen subsets progressively. | Early K50 screen 29,951→31,965; full supplied 200+200 run 32,012→32,012. | No measured full-driver score gain on its tested input. |
| E3 / H1 | Repair diversity after capped-bank eviction despite completed enumeration; short selection, pricing, and priced re-enumeration. | Certified 13-request trap 1,020→2,019; metric 41-request trap 1,019.978→16,004.933. Ordinary K48 +192, fresh K40 holdout −473. | Failure mechanism is real; unconditional repair is rejected. V5 has a different completeness/bank design. |
| H3 | Apply the extra E1 warm start only at K≤64. | Ordinary K65 full run +23 under one seed; short screens mixed. | Insufficient evidence for promotion. |
| G1 | Additional greedy ordering by profit divided by square root of request count. | Synthetic packing 30→60; weak routing screens. | Not shortlisted. |
| G2 | Insert a bank route, eject conflicts, refill, accept only fleet-profit gains; 25-ms allowance. | Synthetic packing 20→32. Actual ordinary runs 33,782→33,782 and fresh 30,045→30,045. | No demonstrated native score gain on the tested inputs. |
| G3 | Regret-2 rebuilding with the versioned insertion cache. | Forty extra rebuilding states passed feasibility/exact-bound checks; supplied 400-request full run 32,012→32,012. | Diagnostic improvements alone do not establish a native gain. |
| A1 | Gate H1 on actual eviction, completed enumeration, 2–48 taxis, ≥80% bank concentration on one request, and poor greedy taxi utilization. | Reaches both certified retention optima. Three ordinary controls tie: 33,782, 28,819, 24,725. Avoids the observed H1 loss but gives up its +192 ordinary gain. | Useful mechanism-specific adaptation; not imported into the changed V5 bank. |
| P1 | Bounded idle-taxi seed for complete pickup-first parcel batches, even with loss-making singleton/pair prefixes. | Additional actual runs: ordinary parcel-heavy and passenger-heavy controls tie; certified capacity-30 case 141→1,141. Fifteen component checks cover size/permutation/rejection boundaries. | Donor of the batch construction used by P2. |
| P2 | Preserve P1; if all taxis are busy, try at most two weakest occupied routes, with originally unserved parcels and strict net fleet-profit acceptance. | Actual one-taxi 100-parcel case 291→2,000; fresh 70-parcel case 201→1,000; ordinary mixed control 31,340 ties, passenger-heavy 13,645 ties. | Selected conservative neighborhood to add to V5. |
| P3 | Permit occupied-route trials even when another taxi is idle. | Heterogeneous-capacity cases 291→2,290 and 201→1,199, both certified optima. Ordinary pure 250-parcel input: seed deltas −48 and −17, mean −32.5. Mixed control ties at 30,141. | Rejected for this merge because it has observed ordinary-case losses. V5 itself already solves the 2,290 case. |
| P3C | Permit that broader fallback only with a capacity-shortage gate. | Prepared hypothesis; no completed experiment. | Not tested, not shipped, no claimed gain. |

Historical validation grew from 14 to 16 regression groups, including 100 random
exact routing cases, exact small masters, route-bank/insertion/cache comparisons,
and independent policy checks. P2 additionally passed 40 seeded-fleet exact-bound
checks, including rejection of costly displaced routes. Those old-backend group
counts must not be represented as a V5 test run.

## 3. What the selected V5 + P2 change actually does

The hook runs after ordinary profitable insertion in the first round of each
V5 fleet-rebuilding phase. All seed attempts share a deadline of at most 50 ms,
clipped to the caller deadline. The greedy batch tries at most eight idle taxis
and six parcel starts; it grows feasible pickups, tries descending prefixes of
at least three parcels, and greedily completes deliveries. The existing complete
route evaluator decides feasibility and true profit.

```text
TRY_BUNDLE_MOVE(state, caller_deadline):
    deadline = min(caller_deadline, now + 50 ms)
    if SEED_COMPLETE_BUNDLE_ON_IDLE_TAXI(state, deadline):
        return improved
    if deadline expired or any taxi is idle:
        return unchanged

    eligible = parcels unserved in the original state
    if fewer than 3 eligible parcels: return unchanged
    for taxi in at most 2 occupied taxis with lowest route profit:
        trial = copy(state)
        remove taxi's route from trial
        if SEED_COMPLETE_BUNDLE_ON_IDLE_TAXI(trial, deadline,
                                            eligible, save_to_bank=false):
            if trial.total_profit > state.total_profit + epsilon:
                state = trial
                save accepted fleet routes to the bank
                return improved
    return unchanged
```

A positive bundle is insufficient if the evicted route was more valuable.
Only whole-fleet improvement is accepted. Rejected trial routes never pollute
the bank. Normal route updates preserve cache-version invalidation. Existing
HSOL code/license, compact storage, allocation guards, pricing deadlines, V5
parcel local search, and final selection reserves remain intact.

The conservative idle-taxi gate has a known boundary: an idle undersized taxi
can prevent trying an occupied larger taxi. The batch is greedy, pickup-first,
and bounded; it can miss interleaved or differently ordered profitable groups.
It is a complementary neighborhood, not an exact general parcel optimizer.

## 4. Fresh V5 versus hybrid results

Both programs used WSL Ubuntu 22.04, GCC 11.4, C++17,
`-O2 -DNDEBUG -pedantic-errors`, solver seed 1, unchanged 298-second internal
budgets, and a 320-second external watchdog. Processes ran serially with
alternating order. All six outputs were independently checked and revalidated
after the run; source/binary/input/output hashes were checked.

| Input | V5 profit | Hybrid profit | Gain | V5 / hybrid seconds | V5 / hybrid peak MiB |
| --- | ---: | ---: | ---: | ---: | ---: |
| Supplied 200 passengers + 200 parcels, K50 | 32,012 | 32,012 | 0 | 12.56 / 11.85 | 136.89 / 136.76 |
| Generated 250 + 250, K49, generator seed349 | 33,399 | 33,399 | 0 | 298.12 / 298.12 | 106.80 / 106.48 |
| Constructed 100-parcel consolidation, K1 | 0 | 1,999.801 | +1,999.801 | 297.06 / 297.14 | 4.875 / 5.375 |

Peak resident memory was measured using GNU `time -v`; MiB is 2^20 bytes. The
450,000,000-byte V5 allocation guard is not a measured RSS peak or a verified
server limit. No general memory reduction or speedup is claimed for the added
neighborhood. Percentage improvement is undefined for the zero-profit baseline;
use the absolute gain on that case.

Both V5 and the hybrid also passed **42 standalone exact-oracle cases each**:
22 semantic boundaries and 20 fresh random cases. Five hybrid component checks
covered idle parcel bundles of 10/100/250, a busy-fleet 291→2,000 replacement,
and preservation of a 4,606-profit route against a 1,000-profit alternative.

The publication source adds explanatory comments after the benchmark. Removing
exactly those inserted comments reproduces the frozen measured source; executable
source text was not changed. The annotated publication source was compiled again
with the same flags. The existing HSOL ignored-`fgets`-return warning is nonfatal.

## 5. Why the constructed gain is certified

The fixture has 100 weight-one parcels, one taxi of capacity 100, revenue 1,000
per parcel, pickup latest 0.5, delivery latest 2, and zero service durations.
Cross-cluster travel time is 1 and cost 98,000; within-cluster time/cost is 0.001.
The origin is in the pickup cluster. Positive symmetric metric matrices are used.

Any route serving q parcels needs 2q arcs and at least one expensive crossing.
After crossing, it cannot return for another pickup before its deadline. Thus

```text
profit(q) <= 1,000*q - 98,000 - (2*q - 1)/1,000,   1 <= q <= 100.
```

This bound increases with q. The all-pickups-then-all-deliveries witness attains
it at q=100: **1,999.801**. The empty route yields zero. The fixture, certificate,
and feasible optimal witness are included beside this report. Its large capacity
is intentional; this is a mechanism counterexample, not a recovered server case
or evidence that ordinary capacity-30 inputs commonly have the same gap.

Earlier direct Linux V5/P2 comparisons also found 2,291/2,291 on the capacity-30
two-taxi decoy and 2,290/291 on the heterogeneous-capacity input. These support
using V5 as the foundation rather than replacing it with the older P2 solver.

## 6. Reproduction, scope, and next session

Compile the branch and the pinned V5 parent separately:

```bash
git show ee67c9f:submission/submission.cpp > /tmp/v5.cpp
g++ -std=c++17 -O2 -DNDEBUG -pedantic-errors /tmp/v5.cpp -o /tmp/v5
g++ -std=c++17 -O2 -DNDEBUG -pedantic-errors submission/submission.cpp -o /tmp/v5-p2
/usr/bin/time -v /tmp/v5 1 < docs/research/v5-parcel-bundles/consolidation-m100.txt > /tmp/v5.sol
/usr/bin/time -v /tmp/v5-p2 1 < docs/research/v5-parcel-bundles/consolidation-m100.txt > /tmp/v5-p2.sol
```

Check solutions with the project's independent `check-solution` command. The
published JSON preserves source hashes and exact new measurements; the local
workspace additionally retains all raw logs, binaries and historical experiments.
The older `submission.readable.cpp` remains the historical V5 artifact, and the
old modular packager should not be used to regenerate this hand-integrated file.

No further experiments were started after this declared batch. Next session,
prioritize new parcel-heavy ordinary inputs and additional seeds, then smaller
capacity consolidation examples. Revisit capacity-aware fallback only as a
separate hypothesis with a fresh holdout. Update the English technical PDF only
after selecting the final algorithm. Current data do not establish a general
contest score improvement, a 60% win rate, or equivalence to the unknown server.
