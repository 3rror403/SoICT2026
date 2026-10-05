# ProfitInsertion: repair-specific incremental cache proposal

This records the original source-grounded proposal. It has since been implemented
and tested; see [the speed campaign](alns-speed-campaign.md) for retained changes
and measured evidence.

## Why repair remains expensive

The latest r240 seed-1 profile records 29,847 ProfitInsertion calls taking
48.455427 seconds (24.97% of the solve), or 1.623461 ms per repair. AddRequest is
called 98,268 times and averages 0.557655 ms per call. The repair is called fewer
times but does more work per invocation.

The default `all_unserved_repair=true` puts every currently unserved request into
the shuffled repair pool, including requests that were not just destroyed.
ProfitInsertion searches the remaining pool for the globally highest positive
insertion gain, inserts one request, then repeats the whole pool search.

For U initially unserved requests and m accepted insertions, it performs
approximately `(m + 1)U - m(m + 1)/2` best-insertion queries. Each query still
visits active-taxi cache entries and empty-taxi choices. This is a repeated
global greedy construction, not a one-pass repair.

The earlier exact r240 seed-1 diagnostic recorded 117,952,615 ProfitInsertion
best-cache queries and 1,585,088,282 active-route entries inspected: about 3,952
queries and 53,107 route-entry checks per repair. It recorded 156,566,533 route-
version rescans plus 103,582,852 lower-threshold rescans; threshold changes caused
39.82% of route rescans. These are counts from the earlier diagnostic, not new
counters added in this inspection.

The current cache's threshold-dependent results are valid: when an earlier
route's best gain falls, later unchanged routes may need a lower-threshold scan
to recover previously pruned candidates. Removing those checks alone would be
incorrect. The recent immutable request table removes descriptor construction,
but leaves this traversal and threshold behavior intact.

## Proposed representation and update

Give ProfitInsertion its own persistent dense request-by-taxi table, allocated
once per solve and reused. Each cell records the route version and that request's
best positive insertion on that taxi, with an explicit absence flag. Store each
request's winning taxi/positions/gain in a separate fixed-size array.

At entry to each repair, synchronize every unserved pool request's cells against
the candidate's route versions. This synchronization is necessary after destroy,
state restoration, and mutations by other operators. A request can also become
unserved after having skipped earlier cache updates while it was served.

During one repair:

1. Select the highest positive gain from stored request winners.
2. Commit the same request/taxi/position descriptor through `SearchState::insert`.
3. Mark the inserted request served, then refresh only the changed taxi's cell
   for **every** remaining pool request. In directed/nonmetric data, any of those
   requests may become newly attractive on the changed route.
4. Update request winners: compare the new cell with an unaffected winner, or
   recompute the maximum of that request's row if its winning taxi changed.
5. Repeat until no positive insertion exists.

The other taxi routes have unchanged costs and feasibility, so their exact cell
values stay valid. An empty taxi's cell describes its precomputed two-node route;
inserting into that taxi changes the same column from empty to active. Taxis that
become empty during destroy must be synchronized at the next repair entry.

Use exact positive route choices with a fixed positivity floor rather than a
floor derived from another taxi's current winner. That removes cross-taxi
threshold invalidation. Within each route scan, a best-only bound may use the
best gain found so far, with the existing numerical tolerance. Negative and zero
choices are not needed by ProfitInsertion's strict `gain > 1e-9` selection.

A linear pass over stored request winners is sufficient initially. A heap is
not required: every remaining request still needs its changed-route cell checked,
and heap maintenance could add work. Global ties must follow the original pool's
first occurrence; taxi and position ties must preserve the existing scanner's
order. Duplicate pool entries must not change that first-occurrence priority.

Keep this cache owned by ProfitInsertion so AddRequest's state transitions do
not acquire additional requirements. Retain the current no-index fallback until
equivalence for that path is explicitly checked. Bind empty-route cache data to
the current instance/index; version zero alone does not distinguish instances.

## Tradeoff and validation

Exact per-taxi positive choices can require more work on the first scan than a
scan pruned against another taxi's already known best gain. The net saving from
fewer route traversals and threshold rescans must be measured, not assumed. Do
not replace the operator with one-pass greedy insertion or reduce its repair
pool in this test, since those change its decisions.

Before integration, compare each selected descriptor and final repaired state
against full-solution enumeration on directed/nonmetric fixtures. Cover ties,
duplicate/reordered pools, taxi activation/emptying, overload, windows, threshold
drops, and reused caches after restoration and different instances. Then compare
r240 seed 1 with the current immutable-table baseline under matched flags and
settings, full 60,000 iterations, complete trajectory/solution comparison, and
the independent full checker. Report immediate repair savings separately from
whole-solve runtime.

The original investigation changed no solver code. The subsequent authorized
implementation and experiments are recorded in the speed campaign.
