# SOICT 2026 input and output

The active supplied dataset is documented in [the probation README](../data/probation/README.md). Synthetic data and its archived results have been removed.

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

