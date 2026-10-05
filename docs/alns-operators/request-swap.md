# Two-taxi request swap

Request swap is enabled by default after CleanCut and before request replacement.
It exchanges two complete served requests on different taxis. Every request
keeps both endpoints on the same taxi; the served-request set is unchanged.

Each incoming pickup occupies the outgoing pickup's position. An incoming
parcel's delivery occupies the outgoing delivery's position. An incoming
passenger's delivery goes immediately after its pickup, before the unchanged
nodes that previously lay between the outgoing endpoints. All other nodes
retain their order. For example:

```text
Before:
Taxi A: parcel pickup -> X -> Y -> parcel delivery
Taxi B: passenger pickup -> passenger delivery -> Z

After:
Taxi A: passenger pickup -> passenger delivery -> X -> Y
Taxi B: parcel pickup -> parcel delivery -> Z
```

This deterministic placement supports passenger/passenger, parcel/parcel, and
mixed swaps, including a parcel whose endpoints were separated. It does not
search alternative insertion positions. Pickup-before-delivery and passenger
adjacency are preserved structurally. Both completed routes must satisfy their
time windows and taxi-specific parcel capacities. Intermediate removals need
not be feasible, which matters for directed, nonmetric travel times.

Revenue cancels because the same requests remain served. The gain is the old
combined route cost minus the new combined route cost. Either individual route
can become more expensive as long as the combined gain is strictly positive.
The operator applies the globally best improving pair and repeats until none
improves, breaking exact gain ties by ascending request IDs `(a,b)`, `a < b`.
There is no accepted-swap count cap.

Candidate evaluation combines a fixed number of cached route segments and new
endpoints, avoiding route copies and insertion-position enumeration. The first
scan examines cross-taxi request pairs, O(served requests squared). SMD stores
the exact best request pair for each taxi pair and selects via a priority queue.
After a swap, only taxi-pair descriptors touching a changed taxi are rebuilt.
Unchanged pairs survive. Whole-route versions validate timing/load dependencies
and detect other local moves or restored ALNS states. Descriptor generations
invalidate old queue entries, and periodic compaction bounds queue storage.
Descriptor storage is O(taxis squared).

`SearchState::swap_requests` independently reconstructs and checks both final
routes before committing either. Rejected swaps preserve routes, assignments,
benefit, and actual route versions. Successful two-route changes clear both old
assignment sets before assigning either new route, using the same atomic commit
helper as CleanCut. `SwapRequest(false)` retains a full pair-scan reference for
differential validation. `--disable-ls` disables this move with the other local
moves; no new command-line option is needed. The existing two-cycle local-search
limit remains in effect.
