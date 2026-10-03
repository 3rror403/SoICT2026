# Request replacement

Request replacement is enabled by default as the last local-search move, after
clean-cut suffix exchange. It exchanges one served request for one unserved
request on the outgoing request's taxi. Passenger and parcel requests can
replace either type of request.

For each outgoing request, the operator constructs a temporary route without
its two endpoints. For each unserved request, it searches every allowed pickup
and delivery insertion position in that route. A passenger is inserted as an
adjacent pickup/drop-off pair; a parcel can have separated endpoints. The
completed route must satisfy timing, precedence, passenger adjacency, and
parcel capacity. Removal alone need not improve benefit or be feasible on
nonmetric travel times.

The benefit gain is incoming revenue minus outgoing revenue plus old route
cost minus new route cost. The best strictly improving replacement across all
served/unserved pairs is applied, and the search repeats until none improves.
Only the completed replacement is committed to `SearchState`; rejected
candidates preserve routes, assignments, benefit, and actual route versions.

Candidate insertion scans reuse the existing directed-cost bounds and segment
feasibility checks. Static move descriptors (SMD) summarize each taxi/unserved
request pair: the best outgoing request and insertion positions, its gain,
and the source route version. A priority queue selects the global best, breaking
equal gains by outgoing request, incoming request, pickup position, then delivery
position, matching the original exhaustive traversal.

After replacing outgoing `a` with incoming `b` on taxi `t`, the operator refreshes
all descriptors for `t`, discards candidates importing the now-served `b`, and
generates descriptors importing the newly-unserved `a` on unchanged taxis.
Other descriptors survive. The entire changed route is invalidated because
timing and load changes can affect positions beyond the changed arcs.

Every descriptor stores its exact best strictly improving move, or no move if
none improves. Insertion scans use the best gain already found for that
taxi/incoming request as their pruning threshold. They do not use the global
winning gain to skip lesser descriptors: those lesser moves may become the
winner after a subsequent replacement invalidates the current winner.

For each outgoing request, the reduced route and removal gain are cached by
source route version. New incoming requests on unchanged taxis reuse these
reduced routes, avoiding repeated route construction. The initial descriptor
build evaluates the full neighborhood; subsequent searches update only the
affected descriptors and select from the queue.

Route versions and the served/unserved set are synchronized on each search,
including after ALNS restores a previously rejected candidate or another local
move changes several routes. Descriptor generations invalidate obsolete queue
entries; periodic compaction bounds queue storage. Descriptor storage is
O(taxis * requests), plus the per-outgoing reduced-route caches. The original
O(requests squared) pair-cache scan is retained via
`ReplaceRequest(false)` for differential validation; the solver uses SMD by
default without a new command-line flag. The reference pair matrix is not
allocated by the default SMD path.

Local search still runs at most two full cycles. It skips the second cycle if
the first finds no improvement across all moves. `--disable-ls` disables
replacement along with the other local-search moves. `--check` retains full
solution evaluation independently of cached move gains.
