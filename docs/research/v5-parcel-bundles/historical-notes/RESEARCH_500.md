# Research under N <= 250 passengers and M <= 250 parcels

The user's revised limits supersede the earlier scale assumption: at most 500
total requests, with each request type separately limited to 250. No new taxi
limit was supplied. Earlier 600–1,000-request experiments remain historical
evidence about mechanisms, not in-range contest evaluation. The generator and
full-driver runner now reject inputs exceeding either type limit. The screening
cohort has also been replaced with in-range cases.

Production code remains unchanged during this investigation. The archived
baseline is f9c94cd5; E1 is 11bb7a49. The fast parser, indexed bank eviction and
bounded pair seeding are already present in both versions.

## New measurements

Five inputs were tested with the same 10-second search-only diagnostic schedule
and independent output checker. These are not production-driver scores. Each
synthetic input uses 250 passengers and 250 parcels; different taxi cells have
different generator seeds, so their scores do not isolate taxi-count causality.

| Input | Baseline | E1 | Delta |
| --- | ---: | ---: | ---: |
| Supplied 200+200, K=50 | 29,072 | 30,073 | +1,001 |
| Generated 250+250, K=48 | 30,381 | 32,505 | +2,124 |
| Generated 250+250, K=49 | 29,283 | 31,531 | +2,248 |
| Generated 250+250, K=65 | 39,822 | 39,735 | -87 |
| Generated 250+250, K=97 | 51,463 | 51,167 | -296 |

On the supplied 400-request input, enumeration took 0.325 seconds and retained
49,546 columns. With a one-second initial selection budget, baseline returned
zero while E1 committed a feasible 29,453-profit fleet. The K=48 and K=49
500-request cells likewise retained 48,000 and 49,000 columns; their baseline
initial selections returned zero, while E1 returned 32,176 and 31,421.

Thus request count alone does not determine master size: even an in-range
instance can yield tens of thousands of columns. Some enumeration-complete flags
differ between timed runs; this is another reason to avoid interpreting these
single diagnostic pairs as a repeatable full-budget advantage.

Raw traces, outputs and input hashes are in `research-500-results.json` and
`.build/research-500/`. Reproduce with `python tests/submission/research_500.py`.

## Connection to earlier findings and next experiments

1. **E3, bank diversity and completion semantics:** the certified 13–41-request
   counterexamples are still within the new limits. Profit-only retention removes
   complementary routes, and DFS completion can cause small fleets to skip
   rebuilding. This is a proved mechanism, unlike an assumption that every full
   bank causes a score loss. Track actual discarded served sets, protect the
   incumbent, reserve a bounded diversity quota and test a short repair allowance
   when enumeration completed but retention discarded columns. Do not force
   every such input into 100 seconds of rebuilding. Keep the old bank-reference
   test as a policy-specific baseline and add explicit expectations if policy
   changes. The earlier cap-4,096 ablation already shows why increasing cap alone
   is not a general solution.
2. **E2, restricted master before expensive pricing:** K=49–64 currently solves a
   full-bank LP for prices before selecting 32/64/128 columns per taxi. Reuse cheap
   opportunity prices, include incumbent columns, solve a small subset first and
   widen only when time remains. Evaluate the entire pricing-plus-selection
   phase. The new 400-request trace confirms this bottleneck remains relevant.
3. **E1 by dispatch branch, as a hypothesis:** K>64 already has four priced greedy
   starts in its group selector. An additional warm start can change the search
   trajectory without adding a missing capability. Both the old full tests and
   new short screens warrant testing a selective E1 against the baseline. They
   do not yet prove a branch threshold should be changed.

Do not combine these experiments into one candidate. First establish full-driver
baselines on the supplied 200+200 input and generated 250+250 inputs, then test one
change with equal production budgets, multiple seeds and separate holdout inputs.
Include passenger-only and parcel-only cells. Reuse the exact small oracle,
independent checker and certified hard corpus; keep adversarial-family results
separate from representative input counts. Input parsing is no longer the first
research priority; retaining the existing fast reader is sufficient until new
measurements say otherwise.

## Full-driver and counterexample confirmation

The actual standalone programs were then run on the supplied 200+200 / 50-taxi
input with seed 1, unchanged internal budgets and independent checking. Both
returned **32,012**: baseline in 122.36 seconds, E1 in 118.61 seconds. This matches
the objective recorded in the repository's best-known file, not a proof of global
optimality. The diagnostic +1,001 advantage disappeared at full budget. A single
timing pair does not establish a speedup. Raw metadata and solutions are retained
under `.build/research-500/full-main/`, with a durable measurement copy in
`research-500-full-results.json`.

Two certified in-range counterexamples were also replayed through both actual
standalone programs, independently validating each output:

| Requests | Baseline | E1 | Certified optimum |
| ---: | ---: | ---: | ---: |
| 13 | 1,020 | 1,020 | 2,019 |
| 41, strictly positive metric distances | 1,019.978 | 1,019.978 | 16,004.933 |

These four additional runs confirm that the earlier retention failure survives
under the new limits and remains unchanged by E1. Their measurements are in
`research-500-hard-results.json`. Prioritize a bounded E3 retention/repair
experiment, then E2's restricted-master experiment; selectively applying E1 is
a separate hypothesis, not an accepted change. No production algorithm edits
were made in this research round.
