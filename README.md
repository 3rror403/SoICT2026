# SoICT2026

## Optimization retrospective — 2026-10-12

The performance target is reached: **27/27 runs** across nine synthetic instances and solver seeds 1–3 exceeded the original best-known solution (BKS) targets within 90 seconds. The slowest process took **87.18s**. Longer runs improved the three largest objectives by **1,728–2,479 points** over matched 90-second runs, within a 180-second limit. All nine synthetic BKS records have been refreshed after independent checks.

The main architectural change was to **generate useful routes, combine them into a fleet, then use that fleet to guide further generation**. Faster branching helped, but the quality and compatibility of candidate routes became the main concern.

The initial goal was an 8× speed improvement while preserving objective **13,279** on `data/probation/r240_p120_m120_s5.txt`. The later acceptance cohort contains nine synthetic instances with 500, 1,000, and 2,000 requests; probation is excluded. The 90-second comparison uses frozen original BKS targets, which are lower than the subsequently updated records.

1. **Shortening ALNS met the initial speed goal.**
   Insertion and replacement consumed about 90% of runtime on r240. A shorter cooling schedule achieved **10.17× speedup**, retaining 13,279 across three seeds. Cutting further or removing local search lost quality. This gain primarily came from doing less search.

2. **Generating routes before ALNS changed the approach.**
   Forward enumeration generated feasible routes for each taxi; a fleet master selected compatible alternatives. On r240, this retained 13,279 and achieved **98.2× whole-process speedup against the shortened ALNS baseline**. Keeping 32 routes per taxi failed, while 48 worked: complementary alternatives mattered.

3. **Larger instances exposed generation and branching limits.**
   On n500, limited generation triggered fallback ALNS, and the final branch and bound consumed hundreds of seconds. Broader generation, tighter LP bounds, and LP basis reuse produced **28,997 in roughly six seconds**. The native master worked well on moderate banks but struggled as banks grew.

4. **Cheap candidate generation often lost quality.**
   Beam search and insertion-focused variants produced narrow banks. Some banks had an upper bound below BKS, leaving branching unable to recover the missing quality. Ranking routes only by individual profit also discarded useful combinations. Diversity and compatibility mattered more than raw route profit.

5. **Fleet feedback made generation effective.**
   Sampling feasible fleets, perturbing and repairing them, applying occasional local refinement, and generating routes guided by request prices supplied better alternatives. Selected fleets fed back into the next round. The accepted synthetic runs used **zero outer ALNS iterations**, while retaining lightweight neighborhood search inside generation.

6. **Selective masters worked better than dense solves.**
   Full-bank CPLEX solves often spent their allowance at the root; parameter tuning offered little improvement. Ranking by request prices and widening from 32 to 64 alternatives per taxi made useful fleet selection faster. Completing a restricted master proves only that subset's optimum.

7. **Removing repeated work achieved the 90-second target.**
   Shortening sampling alone hurt quality. Lazy insertion-cache refresh, buffered input, and lighter refinement—three cycles every two proposals—made useful generation affordable. Largest-input parsing and checking fell from **2.83s to 0.58s**. With O3/LTO and single-threaded CPLEX, all 27 acceptance runs exceeded their frozen targets.

8. **More generation improved the largest cases within three minutes.**
   Giving generation 170 seconds reached **135,071**, **136,196**, and **135,294**, with a longest process time of **176.61s**. Gains over matched 90-second runs were **2,327**, **1,728**, and **2,479**. More time was not uniformly better: one medium case lost 19 points. The BKS ledger retains the strongest checked fleet for each instance.

### Strengths and limits

Both independent checkers accepted all **39** final search runs. All 13 regression targets and focused AddressSanitizer/UndefinedBehaviorSanitizer checks passed. Whole-process timing includes parsing and output; searches read no BKS witnesses or precomputed route banks. Validation uses two shared solver slots with memory caps, each running a solver. Compact checker storage and artifact cleanup reduced resource pressure; `results/` fell from about **3.9 GiB to 771 MiB**.

Candidate generation and route retention remain bounded heuristics. A completed master certifies its generated bank or selected subset, not every feasible route of the original problem. Global optimality on the synthetic cohort remains unproved. The measured large-instance performance uses **CPLEX, O3, and LTO**; equivalent timing with the portable native backend or on unseen instances has not been established.

[Algorithm and design decisions](SYNTHETIC_SPEED.md), [measurements and reproduction](SYNTHETIC_RESULTS.md), and [updated synthetic BKS](data/synthetic/best-known/best-known.csv) provide the supporting details.
