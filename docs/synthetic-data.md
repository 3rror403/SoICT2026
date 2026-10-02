# SOICT 2026 synthetic data

The active profile is `probation-calibrated-v3`, calibrated from the four larger
supplied probation cases. The concise [dataset README](../data/synthetic/README.md)
describes every generation rule, fleet scaling, filenames, seeds, and regeneration.
The [distribution analysis](probation-distribution.md) records the observations
behind these choices. These cases are synthetic development data.

The active 30 files retain six sizes and five mixes. Supplied cases remain
separate in `data/probation`; their filenames use case indices rather than seeds.
The four-request sample remains an I/O fixture and is excluded from request-field
calibration. The main profile follows the observed symmetric, metric matrices;
the optional `directed-noise` profile covers permitted asymmetric/nonmetric inputs.

## Input and output

```text
K N M
K lines: origin_vertex parcel_capacity
N lines: pickup_vertex dropoff_vertex earliest latest service revenue
M lines: pickup_vertex dropoff_vertex weight pickup_earliest pickup_latest delivery_earliest delivery_latest service revenue
V lines of V travel-time values
V lines of V travel-cost values
```

`V = K + 2N + 2M`; matrix rows and columns follow official vertices `1..V`. Generated labels put origins first, then each passenger's pickup/drop-off pair, then each parcel's pair. The reader accepts any permutation of those labels, translates both matrices into the solver's existing grouped node order, and retains the labels for output. The representation uses one distinct vertex per origin and endpoint, consistent with this vertex-count model; duplicate labels are rejected. If the contest later permits shared vertices, that extension needs explicit service semantics.

Output is the recomputed benefit followed by exactly `K` lines, each containing a route length and that many official service-vertex IDs. Unused taxis have a line containing `0`. Origins are omitted from route lines. The checker independently verifies the routes and requires the claimed benefit to match within `1e-6 * (1 + abs(recomputed_benefit))`.

Arriving before an earliest time causes waiting before service. Service occurs at both endpoints. Passenger drop-off has no window. Taxis start at time zero and finish at their final stop without a return leg.

## Replacement history

Previous datasets are preserved under `results/dataset-archive/`:

- `20261002-pre-probation`: original development instances and results.
- `20261002-symmetric-probation-v1`: first sample-derived symmetric profile.
- `20261002-directed-probation-v2`: directed sample-derived profile, metadata,
  best-known records, generator source, documentation, and executable.

Historical objectives apply only to their original input hashes. Best-known
records for the active generation start empty; the evaluator checks hashes to
prevent mixing generations.
