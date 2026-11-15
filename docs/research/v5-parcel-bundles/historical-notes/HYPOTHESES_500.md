# Independent hypothesis experiments

All candidates start from the same frozen E1 source, `11bb7a49`. Experiments use
N<=250 passengers, M<=250 parcels, C++17 and the same compiler/flags. No candidate
combines the other hypotheses. All three independently passed all 16 regression
groups with 100 small random routing cases and the existing exact component
comparisons. Their sources, executables, probes and test records are frozen under
`.build/hypotheses-500/H1`, `H2`, `H3`.

## Definitions

- **H1:** record actual bank evictions. If initial enumeration completes after
  evictions and K<=48, first select for up to 0.2 seconds, estimate request prices,
  then run priced enumeration for up to one second before normal final selection.
  The one-second allowance bounds enumeration, not the subsequent master solve
  or the price-estimation routine.
- **H2:** in the K=49–64 ranked master, replace the full-bank pricing LP with
  existing cheap request prices. Retain the 32/64/128 subset progression and the
  incumbent columns. Avoid computing those prices twice in dispatch.
- **H3:** restrict the additional E1 dispatch warm start to K<=64. The existing
  group-selector greedy starts remain available above that threshold.

## Actual full-driver comparisons

Each pair uses solver seed 1, unchanged production budgets and a 320-second
external watchdog. All outputs were independently feasible.

| Hypothesis | Input | E1 profit | Candidate profit | Difference |
| --- | --- | ---: | ---: | ---: |
| H1 | 250+250, K48, generator seed 348 | 33,782 | 33,974 | +192 (+0.568%) |
| H2 | Supplied 200+200, K50 | 32,012 | 32,012 | 0 |
| H3 | 250+250, K65, generator seed 365 | 39,786 | 39,809 | +23 (+0.058%) |
| H1 fixed holdout | 250+250, K40, generator seed 611 | 28,819 | 28,346 | -473 (-1.641%) |

H1's original pair took 41.20 versus 42.38 seconds; its holdout pair took 41.07
versus 42.31 seconds. H2 took 119.32 versus 119.25 seconds. H3 took 297.15 versus
297.23 seconds. Tiny single-pair timing differences do not establish speedups.

The holdout was fixed before viewing its scores. H1 was not adjusted afterwards.
Across its two non-adversarial inputs, H1 has one win and one loss; their raw profit
changes sum to -281. This is insufficient to accept H1 as a general scoring gain.

## Certified counterexamples and phase screens

The actual standalone programs were also tested on the earlier certified corpus
members. H1 returned 2,019/2,019 on the 13-request example and
16,004.933/16,004.933 on the strictly positive metric 41-request example. E1, H2 and
H3 retained the earlier 1,020 and 1,019.978 results. All outputs were independently
checked. H1's 41-request run took 41.35 seconds, against about 0.63 seconds for E1:
repair can substantially increase subsequent selection work.

An initial H1 attempt was interrupted by an overly short 30-second diagnostic
watchdog. It is recorded as an aborted attempt, not scored as an invalid or zero
solution. The harness was corrected to the production-compatible 320-second
watchdog before rerunning. The solver budget itself was unchanged.

Twenty additional 10-second search-only screens used the same five in-range
inputs as the preceding research. H2 improved its active K50 and K49 cells from
29,951 to 31,965 and from 31,560 to 33,324. Its full-driver result above shows why
these early gains must not be called final scoring gains. H3's active K65 and K97
screens changed 39,899 to 39,755 and 51,024 to 51,145: mixed evidence.

The screen harness invokes components directly and does not execute H1's new
main-driver repair block. Its H1 rows are timing controls, not tests of that
repair. Likewise, cells outside a variant's changed dispatch branch are controls.
Do not count their fluctuations as algorithm wins or losses.

## Decision

No hypothesis is promoted or combined in this round. **Active
`submission/submission.cpp` remains E1**, byte-identical to the starting source.
H1 validates the retention mechanism but fails its holdout; H2 improves early
selection with no measured full-driver gain on its tested input; H3 has only a
tiny one-seed full-driver increase and mixed screening evidence.

Before revising H1, trace both original-bank selection and repaired-bank
selection, including root-LP completion and incumbent quality. More columns can
consume the master budget; this is a hypothesis for the holdout loss, not an
established cause. Test any revision on a new holdout because this one has now
been inspected. H2 and H3 need more full-driver inputs/seeds before acceptance.

Raw diagnostic/certificate results are `hypotheses-500-results.json`; full-run
metadata, outputs hashes, variant hashes and regression records are in
`hypotheses-500-full-results.json`. Outputs and logs remain under `.build/`.
Reproduction scripts: `experiment_hypotheses.py`, `evaluate_hypotheses.py`,
`full_hypotheses.py`, `holdout_h1.py`. The build script restores the active E1 source
and binaries after building the isolated candidates. No push was performed.
