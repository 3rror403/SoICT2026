# ALNS destroy and repair study: r240

The 2026-10-04 study improved the best known benefit on
`r240_p120_m120_s5.txt` from **13,043 to 13,279**, a **1.8094%** increase.
Two complete route enumerators and an independently checked CPLEX fleet master
proved that **13,279 is optimal for these input bytes and the current problem
semantics**. A 2% improvement would require an integer benefit of at least
13,304, which exceeds this bound.

The input SHA-256 is
`93baf239e2d2ccb44cfb2b2a14bb15c42804acfc6f8f186cc9f05896e62b86e4`.
The instance, time/cost matrices, revenues and constraints were preserved.
The original best-known CSV and route are archived under
`results/r240-enhancement-20261004/starting-best-known/`.

## Retained operators

These destroy and repair extensions are enabled by default. Use
`--legacy-operators` to restore the original three destroy arms, removed-only
candidate pool and greedy repair. Individual ablations are
`--removed-only-repair`, `--disable-profit-repair`, `--disable-route-removal`,
`--disable-opportunity-removal` and `--disable-route-pool-repair`.

Repair considers **all currently unserved requests**, shuffled once per call,
instead of only requests returned by destruction. The adaptive repair arms are
the existing insertion in supplied order and **ProfitInsertion**. The latter
repeatedly inserts the request with the largest positive, feasible marginal
benefit over all taxis and legal positions. A route-version cache avoids
rescanning unchanged routes; it does not approximate the best insertion.

**RouteRemoval** visits taxis in shuffled order. It removes complete routes
atomically when they fit the requested destruction budget. Otherwise it tries
up to four shuffled subsets fitting the remaining budget. The final residual
route must be feasible. On directed/nonmetric data an intermediate shortcut
need not be feasible, so deleting requests sequentially would miss some valid
joint removals. Failed removals do not consume the budget or appear in the
returned request list.

**OpportunityRemoval** chooses an unserved request, weighted by its best
positive solo benefit where available. It removes served requests near that
request in pickup space, delivery space and pickup time, with randomized rank
selection. Each removal receives the same feasibility checks as the existing
destroy operators. Random, worst and related removal remain adaptive arms.

**RoutePoolRepair** collects positive-benefit routes from every feasible ALNS
proposal, including proposals rejected by simulated annealing. Routes retain
their original taxi identity. For an identical served set on one taxi, only
the highest-benefit sequence is needed. Each taxi keeps at most 200 served-set
columns; the final incumbent routes are restored even if the cap removed them.

The solver reserves at most one second of its existing allowance for the final
fleet repair. It chooses at most one route per taxi and at most one occurrence
of each request. Pair and triple fleet repairs provide a feasible incumbent,
then bounded branch-and-bound searches the complete recorded pool. With
nonnegative request prices `lambda[r]`, a valid upper bound is

```text
sum_r lambda[r]
  + sum_t max(0, max_route_on_t(profit(route) - sum_r_in_route lambda[r])).
```

Projected subgradients tighten these prices; every iterate supplies a valid
bound regardless of convergence. The complete selected fleet receives full
`Solution::evaluate()` validation before application. A deadline preserves the
best feasible fleet found so far. This repair uses portable C++ and has no MIP
solver dependency. Logging adds `route-pool.csv` with official vertex IDs.

Local search retains the existing two-cycle sequence: remove, add, passenger
relocation, parcel relocation, clean-cut suffix exchange, fixed-position
whole-request swap, and replacement. A reoptimized-position swap and an
admission/relocation prototype provided no supporting quality evidence and
were retired.

## Evaluation scope

The baseline and each quality trial used exactly this instance, solver seeds
1, 2 and 3, a requested allowance of 300 seconds, default temperature 200 and
cooling, and three concurrent single-threaded processes. Builds used GNU
11.4.0 with Release flags `-O3 -DNDEBUG -std=gnu++20 -O2`. CLI bookkeeping
reserves one second, leaving a solver budget of approximately 299 seconds.
The unchanged baseline mean benefit was 12,851.3; the supplied best-known
record, 13,043, is a different reference and is not the baseline mean.

| Configuration | Seeds 1 / 2 / 3 | Mean benefit | Worst | Mean iterations | Mean solver seconds |
| --- | ---: | ---: | ---: | ---: | ---: |
| baseline | 12929 / 13009 / 12616 | 12851.3 | 12616 | 59730 | 299.003 |
| greedy-all | 13159 / 12949 / 13133 | 13080.3 | 12949 | 38375 | 299.003 |
| profit-all | 13140 / 13043 / 13121 | 13101.3 | 13043 | 36146 | 299.004 |
| route-with-best-repair | 13143 / 13159 / 13121 | 13141.0 | 13121 | 39417 | 299.005 |
| opportunity-with-best | 13140 / 13144 / 13190 | 13158.0 | 13140 | 38753 | 299.005 |
| route-pool-group-repair | 13212 / 13235 / 13247 | 13231.3 | 13212 | 44617 | 299.000 |
| all-candidate-route-pool | 13265 / 13265 / 13279 | 13269.7 | 13265 | 44690 | 299.002 |
| priced-route-pool | 13279 / 13279 / 13279 | 13279.0 | 13279 | 45224 | 298.249 |
| final-default (no flags) | 13279 / 13279 / 13279 | 13279.0 | 13279 | 45033 | 298.249 |

The final no-flags defaults also produced the same checker-valid optimal fleet
on all three seeds. Each final pool search completed within its reserved
second. The normal project executable was rebuilt; all 11 project tests
(including CPLEX) passed, and the route-pool tests also passed AddressSanitizer
and UndefinedBehaviorSanitizer. Cache and operator reference tests use
directed/nonmetric matrices, tight windows, mixed request types and rejected
state restoration. Independent exhaustive fleet tests cover five taxis and
a coordinated four-taxi exchange that pairs and triples cannot improve.

The request-price variant gains 350, 270 and 663 benefit units over its
matched baseline seeds: mean gain **427.7**, or **3.3278% of the baseline
mean**. A descriptive paired bootstrap over these three differences gives
a 95% interval of **[287.3, 595.2]** benefit units (all 27 resamples, percentile
interpolation). This very small single-instance sample is exploratory and
was also used to select the operators; it is not held-out evidence.

Frozen binaries, source archives, compiler/flags, input and output hashes,
commands, traces, checks and seed rows are stored in
`results/r240-enhancement-20261004/<trial>/`. `comparison.csv` contains the
complete trial table. All reported completed quality outputs passed
`check-solution`. The selected final outputs also passed an independently
written Python route simulator.

Three seeds on one instance support the requested local judgement. They do
not establish quality or speed improvements across the full probation set.
Changes to iteration throughput under these wall-time budgets are not
standalone speed claims.

## Retired experiments

Activating positional regret on the removed-request pool gave mean benefit
12,904.0. Expanding only the greedy pool gave 13,080.3. Profit ordering over the
full pool gave 13,101.3; adding route removal gave 13,141.0 and opportunity
removal gave 13,158.0.

Distinct-taxi regret, positional regret with caching, noisy profit ordering,
and reoptimized-position swaps all had lower means than the best configuration
available at their respective trials. They were not retained as active arms.
The scoped positional-regret cache reproduced the uncached repair trajectory
exactly over all 51,126 recorded iteration states across the three seeds;
its throughput improvement did not establish a quality advantage.

Extending the preferred conventional configuration to a 1,200-second allowance
returned the same three objectives after its ordinary temperature stop at
60,000 iterations. Independent greedy route generation and bounded beam route
generation also failed to improve the 13,279 record, so they were kept as
diagnostics rather than added to the active solver. Archived negative evidence
is preserved. The earlier r95 shared-gap pair/delay trial remains documented
in [operator-trials.md](../evaluation/operator-trials.md).

## Exact certificate

An uncapped C++ forward enumerator traversed all feasible route prefixes for
each taxi. A separately written Python enumerator used the official input
directly, with no `RouteCache`, insertion delta or pruning code from ALNS.
It visited **427,881 prefixes including 20 empty roots** and reproduced the
same **21,461 positive-benefit served-set columns**, including each column's
best benefit. Its maximum observed route length was 11 service vertices;
this was an outcome, not an imposed limit.

Every passenger is appended as an adjacent pickup/drop-off pair. Parcels can
remain onboard across other requests, and only onboard parcels can be
delivered. The enumerators enforce the actual arrival deadlines, waiting,
service and taxi-specific capacity, with no return leg. The independent
enumeration has no time, beam, request-count or route-length cutoff.

Nonpositive complete routes can be omitted because an unused taxi is allowed
and omitting a route cannot create a request conflict. A lower-benefit route
with the same taxi and served set is dominated: feasibility is already checked
inside each complete route and all inter-route constraints depend only on
taxi identity and served requests.

The complete master has one binary variable per retained route, constraints
selecting at most one route per taxi and serving each request at most once,
and the sum of route benefits as its objective. Thus it represents every
potentially improving fleet solution. CPLEX 22.1.1, using one thread and zero
relative MIP gap, returned integer optimum and upper bound **13,279**. HiGHS
independently returned the same complete-master optimum. The extracted fleet
passed both the C++ checker and independent Python simulation.

The model, columns, enumeration counts, checked official solution, CPLEX log
and XML solution, hashes and certificate are archived in
`results/r240-enhancement-20261004/independent-exact-audit/`.
The earlier independent certificate is preserved alongside it.

A direct feasibility audit added the exact requirement `total benefit >= 13304`
to this complete master. CPLEX proved it **integer infeasible**, with zero
solutions. The threshold model, unchanged parent-master hash and CPLEX log
are preserved in `results/r240-enhancement-20261004/target-feasibility-audit/`.

To recheck the complete master with the installed CPLEX:

```sh
/opt/ibm/ILOG/CPLEX_Studio2211/cplex/bin/x86-64_linux/cplex -c "read results/r240-enhancement-20261004/independent-exact-audit/complete-master.lp" "set threads 1" "set mip tolerances mipgap 0" "set mip tolerances absmipgap 0" "optimize" "quit"
```

Private reproducibility helpers remain ignored under `tests/`. The audit
also stores copies of its enumeration and verification code, a C++ generator
binary, and frozen source. `tests/certify_r240_route_master.py --output NEW_DIR
--cpp-bank CSV` independently rebuilds the complete route bank and master;
`tests/verify_r240_certificate.py --directory DIR --binary SOLVER` checks its
CPLEX result and writes the official-format fleet and certificate.
The proof applies to this file and these semantics; it does not extend the
ordinary `solve-mip` command's ten-request limit.


## Explicit upper bound verified with integer arithmetic

A further audit constructs an upper bound without relying on a MIP solver's
reported gap. Give every request a nonnegative weight, and give each of four
three-resource conflict groups a nonnegative weight. A resource is one taxi
identity or one request. A route triggers a group when it uses at least two
of its three resources. Two selected routes cannot both trigger the same
group: their resource sets would intersect, duplicating a taxi or request.
Thus every request weight and every group weight can be charged at most once
in a feasible fleet.

For each taxi, enumerate its feasible routes and compute its maximum residual
profit after subtracting the request and group weights of each route, allowing
zero for an unused taxi. This maximum is its taxi weight. Every route's profit
is therefore bounded by its taxi weight plus its request and group weights.
Summing over any feasible fleet gives a universal upper bound: the sum of all
taxi weights, all request weights, and all group weights.

The stored integer certificate gives the following totals:

| Weight class | Total |
| --- | ---: |
| Taxi maximum residual profits | 10,563 |
| Request weights | 2,547 |
| Four conflict-group weights | 169 |
| Upper bound for every feasible fleet | **13,279** |

The nonzero conflict groups are:

| Three resources | Weight |
| --- | ---: |
| Parcels 55, 65 and 90 | 103 |
| Passenger 61, parcel 53 and parcel 120 | 43 |
| Taxi 6, passenger 61 and parcel 53 | 21 |
| Passenger 7, parcel 39 and parcel 89 | 2 |

For example, the chosen taxi-7 route serves passenger 89 and parcels 65, 76
and 90. Its request weights are 0, 38, 26 and 222; it triggers the first group.
Its benefit 848 is exactly covered by `459 + 0 + 38 + 26 + 222 + 103 = 848`.
The checker verifies this coverage for every one of the 21,461 retained routes,
and verifies that each taxi weight equals its actual maximum residual profit.
Lower-benefit routes with the same taxi and served set inherit the same bound;
nonpositive routes are covered by nonnegative weights.

Weights were discovered by linear programming and valid three-resource
conflict inequalities. The initial fractional bound was 13,311.6667; eight
added inequalities tightened it to 13,279. Only four carry positive weight
in the final certificate. Verification uses exact integer arithmetic and no
optimization solver:

```sh
python3 tests/verify_r240_algebraic_bound.py
```

The full weights, route-bank hash and discovery output are preserved under
`results/r240-enhancement-20261004/algebraic-upper-bound/`.
