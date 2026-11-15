# Alternative search directions and adaptive selection

The common baseline is frozen E1 (`11bb7a49`). All new experiments obey the
250-passenger / 250-parcel limits and retain the production budget. Each candidate
was separately built and passed all 16 regression groups with 100 random cases.
No G1/G2/G3 combination was tested or installed.

## Three alternative directions

| Variant | Mechanism | Component witness | Full-driver result |
| --- | --- | --- | --- |
| G1 | Add a greedy ordering by profit / sqrt(served request count), retaining the best actual-profit fleet | 30 to 60 | Not shortlisted after weak screening |
| G2 | Insert one bank route, eject conflicts, greedily refill idle taxis, accept only actual-profit gains; at most 25 ms | 20 to 32 | 500 requests/K48: 33,782 vs 33,782; fresh K40/seed701: 30,045 vs 30,045 |
| G3 | Regret-2 request insertion during rebuilding, using the existing versioned insertion cache | 40 extra rebuilding cases independently checked against feasibility and exact upper bounds | Supplied 400 requests/K50: 32,012 vs 32,012 |

G1 and G2 witnesses are synthetic master-column fixtures, not full routing-score
improvements. Twenty 10-second search-only screens on five in-range inputs showed
G2 wins/ties/losses of 3/1/1. G3's effects were smaller and mixed. Native K<=48
runs skip rebuilding when enumeration completes, whereas the diagnostic schedule
always invokes rebuilding: G3's screen effects there must not be interpreted as
native behavior. Full-driver tests above were selected to exercise the modified
paths. Each full pair has one seed; no statistically stable scoring gain is
claimed for G1, G2 or G3. The initial-packing improvements can disappear once the
master has enough time. G3's rebuilding checks exercised the modified path, rather
than relying solely on small full-program tests that may skip rebuilding.

## Adaptive A1: repair only a concentrated, poorly utilized bank

Following the user's request to apply useful methods adaptively, A1 gates the
earlier H1 repair using observable bank/fleet features. It requires:

1. Initial enumeration completed and an actual capped-bank eviction occurred.
2. Between 2 and 48 taxis.
3. A single request occurs in at least 80% of stored columns.
4. A diagnostic greedy packing uses at least one taxi but no more than
   max(1, floor(K/3)).

Only then does it run H1's 0.2-second initial selection, request-price estimation
and at most one second of priced enumeration before normal final selection. The
one-second quota bounds enumeration, not all downstream LP work. The thresholds
are heuristic, based on the earlier retention failure mechanism; no optimality
or classifier accuracy guarantee is implied. This is not an oracle that knows
which variant's final score will win. It deliberately forgoes H1 gains when the
observed concentration/utilization pattern is absent.

The separate bank diagnostic completed enumeration in all five checks below.
It measures the concentration/utilization part of the gate; native production
also checks actual eviction and the driver conditions.

| Case | Max request share | Greedy active taxis | Gate | E1 score | A1 score |
| --- | ---: | ---: | --- | ---: | ---: |
| Certified 13-request trap | 100% | 1/2 | On | 1,020 | 2,019 |
| Certified metric 41-request trap | 100% | 1/16 | On | 1,019.978 | 16,004.933 |
| Prior H1 win: 500/K48/seed348 | 10.7604% | 47/48 | Off | 33,782 | 33,782 |
| Prior H1 loss: 500/K40/seed611 | 9.365% | 40/40 | Off | 28,819 | 28,819 |
| Fresh 500/K32/seed811 | 11.7% | 32/32 | Off | 24,725 | 24,725 |

All scores in this table invoke the actual standalone programs, solver seed 1,
with independent output validation. The fresh seed811 input was fixed before
viewing its score; A1 was not tuned after evaluating it. The old win/loss cases
are development cases, not new holdouts. As expected, A1 avoids the observed H1
loss while giving up its +192 gain on the old win case. Adversarial-family gains
are kept separate from the three ordinary inputs rather than combined into an
overall contest win rate.

The metric trap takes about 41.75 seconds with A1, versus 0.80 seconds for E1.
The three ordinary pairs took approximately 41.40/41.30, 41.07/41.09, and
1.56/1.73 seconds (E1/A1). The rare-path repair can thus be expensive. These are
local single-pair timings, not speedup guarantees. More seeds and varied normal
inputs remain necessary for a competition-wide scoring claim.

## Selected local source and artifacts

**A1 alone is installed in `submission/submission.cpp`**, with matching standalone
and probe binaries and its passed regression record under `.build/`. The E1
reference and all four candidate source/binary snapshots remain separately
archived under `.build/experiments/E01/` and `.build/alternatives-500/`.

Raw records: `alternatives-500-results.json`, `adaptive-500-results.json`, and
`alternatives-500-full-results.json` (variant hashes, test records, full-run input
and output hashes and metadata). Source was changed only after testing finished;
the active source is byte-identical to the tested A1 snapshot. No push occurred.

Reproduction scripts: `experiment_alternatives.py`, `evaluate_alternatives.py`,
`full_alternatives.py`, `experiment_adaptive.py`, and `evaluate_adaptive.py`.
Build scripts require the frozen E1 source as their starting active source and
refuse to overwrite a different active candidate; run them only in a separate
experiment workspace or after explicitly preparing that baseline. They do not
silently overwrite existing experiment directories.
