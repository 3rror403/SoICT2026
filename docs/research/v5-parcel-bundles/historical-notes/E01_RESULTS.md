# E1: feasible incumbent before LP work

Status: correctness checks passed; all 14 full-driver runs completed with
independently revalidated solutions. Extended testing does not support accepting
E1 as a scoring improvement. The current benchmark limit is 1,000 total
requests (passengers plus parcels).

The baseline is the local fast-input version, SHA-256
`f9c94cd5d2c48e80f7931a82fae6f026705eb2041d4a8c46650c8ba3918b30c7`.
E1 is SHA-256
`11bb7a49e9e4a19598a2a16b00297b1643065f84544073672d79644e43931ac0`.
Both use MinGW g++ 13.2.0, C++17 and `-O2`; the source's existing optimization
pragma remains the same. Baseline and candidate artifacts are separately frozen
under `.build/baseline-current/` and `.build/experiments/E01/`.

Previously, the exact master returned the supplied incumbent when its root LP
timed out, even if that incumbent was empty and useful routes were already in the
bank. Request-price estimation computed a greedy lower bound but did not retain
that packing as the fleet. E1 sorts columns by true profit and tries both filling
idle taxis in the incumbent and building a fresh conflict-free packing. It keeps
the better feasible result before pricing or LP work, and supplies it to the
master. Existing incumbents remain protected. Already expired deadlines return
without starting LP construction.

This is a warm-start change. It does not repair the known bank-retention trap or
prove that downstream timed search always scores at least as well as baseline.

## Correctness

All 16 groups passed with 100 random routing instances. Checks include 201 exact
small master comparisons, 102 greedy-packing scenarios with incumbent preservation
and conflict checks, a 12,000-column master with a 20 ms allowance, and preservation
of the supplied incumbent with an already expired deadline. The remaining route,
cache, bank-policy, parser and fleet-dispatch regressions also passed.

A separate before/after diagnostic uses 12,000 synthetic master columns over
12 taxis and 120 shared requests. Every selected packing is independently checked
for taxi assignment, request conflicts and profit accounting. Both versions time
out in their root LP at each allowance below, but E1 retains its feasible packing:

| Master allowance | Baseline profit | E1 profit |
| --- | ---: | ---: |
| 5 ms | 0 | 19.016 |
| 20 ms | 0 | 19.016 |
| 50 ms | 0 | 19.016 |

These are component measurements, not full routing scores. Raw rows are retained
in `e01-master-deadline-results.json`. The diagnostic harness is
`master_deadline_probe.cpp`, copied beside the archived baseline source and built
with the same C++17 flags. The query is under
`.build/experiments/E01/master-timeout-query.txt`.

## Full-driver comparison

The first cohort comprises 600 requests / 48 taxis, 650 / 49 and 700 / 65,
solver seed 1. The actual production executable runs serially, alternates
baseline/candidate order, and keeps its internal 299-second budget. The independent
checker validates each output. The external watchdog is 320 seconds.

| Requests / taxis | Baseline profit | E1 profit | Change | Baseline wall (s) | E1 wall (s) |
| --- | ---: | ---: | ---: | ---: | ---: |
| 600 / 48 | 38,563 | 38,563 | 0 | 41.47 | 41.78 |
| 650 / 49 | 37,316 | 37,316 | 0 | 136.71 | 134.60 |
| 700 / 65 | 49,161 | 49,172 | +11 (+0.0224%) | 297.23 | 297.24 |

There is one win, two ties and no losses on this initial single-seed cohort.
The mean relative change is +0.00746%; the raw sum changes by +11. These very small
differences can be affected by timing, so the experiment does not establish a
repeatable scoring advantage. E1 remains a local, tested warm-start safeguard;
further seeds and holdout evaluation are required before treating it as a stronger
contest submission. The next planned scoring experiment is E2's master cost
reduction, within the user's 1,000-request limit.

Raw measurements, outputs and hashes are in
`.build/experiments/E01/full-large/`; a durable copy of the measurements is
`e01-full-large-results.json`. This preliminary cohort is not a hidden-test win
rate; the official scoring formula and competitor results remain unknown.

## Extended test result and decision

Eight additional runs completed using the same frozen baseline and E1 binaries.
The new cohort was declared before running: seeds 2 and 3 on the 700-request
input, plus two 1,000-request inputs with different passenger/parcel mixtures.
One interrupted, unfinished run was restarted; already completed valid runs were
hash checked and reused. All 14 completed outputs were independently checked
again after merging. Inputs, solutions, source and binary hashes also matched.

| Requests / taxis / mixture | Seeds | Baseline mean | E1 mean | Delta |
| --- | --- | ---: | ---: | ---: |
| 600 / 48 / 300 passengers + 300 parcels | 1 | 38,563 | 38,563 | 0 |
| 650 / 49 / 325 + 325 | 1 | 37,316 | 37,316 | 0 |
| 700 / 65 / 350 + 350 | 1, 2, 3 | 49,161 | 49,145 | -16 |
| 1,000 / 97 / 700 + 300 | 1 | 66,603 | 66,358 | -245 |
| 1,000 / 48 / 250 + 750 | 1 | 45,291 | 45,309 | +18 |

The 700-request deltas by seed are +11, -59 and 0. The initial +11 does not
establish a repeatable gain. Counting inputs after averaging their seeds gives
**one win, two ties and two losses**. The mean relative change across inputs is
**-0.07213%**, and the sum of input mean profits changes by **-243**. These are
proxy objective measures, not the unknown official competition scoring formula.
Only the 700-request input has multiple solver seeds, so this is still a small
cohort rather than a hidden-test performance estimate.

Decision: E1's component-level timeout safeguard is demonstrated, but this test
does not justify promoting E1 as a stronger scoring submission. Keep the frozen
baseline as the scoring reference for E2. Production source was not changed
during this extension and still contains experimental E1; no automatic rollback
or new algorithm tuning was performed. The measured failure is final objective
quality, with no invalid outputs, crashes or watchdog timeouts.

The merged data, per-seed results and experiment metadata are retained in
`e01-extended-results.json`. Reproduction and the merge command are documented in
`README.md`.
