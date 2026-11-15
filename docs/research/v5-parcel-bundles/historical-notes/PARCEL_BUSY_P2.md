# P2: profitable parcel-bundle replacement in a busy fleet

P2 starts from P1 (`2242e3bc`) and keeps its idle-taxi parcel seed. The added
branch tests occupied taxis only when all taxis are busy. All instances obey
N<=250 and M<=250. Search budgets, bank policy and feasibility tolerances are
unchanged. Source/binaries are frozen under `.build/parcel-busy/P2/`.

## Reproduced P1 limit and exact bounds

The new 100-parcel instance uses one taxi, capacity 30, parcel weight 10,
service duration 2, integer values and positive symmetric metric matrices.
P1 serves the late small parcels for 291 and cannot invoke its idle-taxi seed.
The best route instead serves three early core parcels for **2,000**.

For q=0..3 core and d=0..D late parcels, any route uses 2(q+d) service-arrival
arcs. It needs zero costly crossings if q=0, at least one if q>0,d=0, and at
least two if q>0,d>0: core deliveries must finish by 30, while late pickups start
at 100. Profit is bounded by qR+dL-xC-[2(q+d)-x]. Maximizing this finite bound
and including the empty route gives the certified optimum; a supplied witness
attains it. The verifier checks all request attributes, capacities and matrix
entries before accepting the bound and independently checks the witness.

This is a constructed continuation of the known consolidation mechanism,
not a recovered server input or a representative distribution sample.

## Candidate policy and safety checks

On the first rebuilding round, P2 shares a 50-ms deadline across:

1. The original idle-taxi seed.
2. If that fails and every taxi is occupied, at most two taxis ordered by their
   current route profit, weakest first.
3. For each trial, temporarily empty that taxi in a copied SearchState and seed
   only parcels that were unserved in the original state. Released old jobs do
   not distract the batch seed's bounded starts.
4. Commit only if the **entire trial fleet's true profit** beats the original.
   Save accepted fleet routes in the bank. Rejected trials do not enter the bank.

This prevents accepting a profitable bundle that is worth less than the route
it displaces. It permits legal omission of old optional requests only when the
full fleet improves; normal subsequent insertion may recover those requests.
The existing versioned cache remains valid because changed routes get new
versions. The new branch adds no random-engine draws.

## Results

| Actual full-driver test | P1 | P2 | Certified optimum |
| --- | ---: | ---: | ---: |
| 100 parcels / one taxi | 291 | 2,000 | 2,000 |
| Fresh 70-parcel holdout / one taxi | 201 | 1,000 | 1,000 |
| Ordinary 50 passengers + 250 parcels / K49 | 31,340 | 31,340 | Unknown |
| Passenger-heavy 250 + 50 / K32 | 13,645 | 13,645 | Unknown |

All outputs were independently checked. The holdout ran as a fresh serial pair
with the original native budget and 320-second watchdog. The other three reuse
completed matching P1 input/source/seed measurements; their saved outputs were
rechecked, and archived hashes verified. They must not be used for speedup claims
or treated as simultaneous paired timing measurements. The two constructed
cases take about 101 seconds per process because rebuilding retains its existing
100-second phase allowance. No source tuning followed the holdout result.

All **16 regression groups** passed with 100 random cases. Forty additional
four-parcel seeded-fleet tests preserved feasibility, never reduced the supplied
fleet profit, and stayed below the exact optimum. Twenty of those used a very
valuable old route; every such rejected trial left the bank empty. A separate
4,606-profit route also remained unchanged when the alternative bundle earned
only 1,000. Idle-taxi compatibility tests retained the exact seed results for
bundles of 10, 100 and 250 parcels.

## Explicit remaining boundaries

Two extra component fixtures checked heterogeneous taxi capacities:

- With both taxis busy, the weakest has capacity 20 and cannot carry the triple;
  the second has capacity 30. P2 successfully tries the second and improves the
  supplied fleet from 291 to 2,003. The optimum is 2,291; this helper alone does
  not reinsert the displaced late requests. This is not a native-driver gap.
- With an idle capacity-20 taxi and an occupied capacity-30 taxi, P2 still returns
  291 on the supplied state although a 2,291 witness exists. The current gate
  stops replacement whenever any taxi is idle, even if that taxi cannot carry
  the group. This is a proved component boundary, not yet a full-driver failing
  instance. It is deliberately documented rather than silently changing the
  tested candidate mid-experiment.

P2 also retains the bounded six-start greedy batch, considers at most two weak
routes, and is called only in the first round of each rebuilding phase. It does
not implement multi-taxi reassignment or a complete neighborhood search. Ordinary
controls tied; broad contest-score or runtime improvements remain unproven.

## Selected local version and artifacts

P2 is installed in `submission/submission.cpp`, byte-identical to the tested
candidate. P1 remains archived and the previous cold-start reproduce ZIP remains
valid for its original source versions. No push occurred.

`parcel-busy-results.json`, `parcel-busy-boundaries.json`, and
`parcel-busy-full-results.json` retain results, test records, references to reused
baselines and hashes. `parcel-busy-repro.zip` contains the new inputs, optimum
witnesses, certificates, independent checker, verifier, both sources and a replay
script. Extract it and run `python reproduce_busy.py` with Python 3.10+ and g++;
the default pair takes about 3.5 minutes on the measured workstation.

Experiment scripts are `experiment_busy_parcel.py`, `evaluate_busy_parcel.py` and
`check_busy_boundaries.py`. The build script requires frozen P1 as its starting
source and refuses to overwrite a different active candidate.
