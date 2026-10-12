# Synthetic performance results — 2026-10-12

The solver exceeds all nine original synthetic BKS targets in **27/27 runs across seeds 1–3 within 90 seconds**. The slowest process takes **87.18 seconds**, and the largest peak solver RSS is **636.33 MiB**. Both independent checkers accept every fleet. The same executable also passes nine 180-second quality runs and three further 180-second budget-allocation trials.

These comparisons use the frozen pre-update target ledger in `results/diagnostics/synthetic-speed-20261012/targets.csv`. The live synthetic BKS ledger has subsequently been raised. The 90-second acceptance is against the frozen targets, rather than the stronger newly updated records. Probation is excluded from this cohort.

## What changed

- Independent solver processes supply validation concurrency; CPLEX uses one thread per solve.
- Made sampled-fleet insertion choices lazy: refresh a request only when examined, retaining exact choices for unchanged routes. Independent endpoint enumeration checks cache correctness across route changes and restored states.
- Made sampling refinement lighter and more frequent: removal, insertion, clean-cut suffix exchange, and replacement; three cycles every two proposals. Full relocation and swap remain available in the ALNS fallback, final polishing, and `--full-fleet-refinement` ablations.
- Added buffered numeric input parsing. A matched largest-input read-and-check fell from **2.83s to 0.58s**; both matrices and the vertex mapping matched the original parser exactly on two large instances, covering **68,757,264 matrix entries** in total.
- Reserve a small fixed interval for final fleet selection, then polish the selected fleet while time remains. Generation continues to alternate sampled fleets, master feedback, and price-guided routes.
- Stream the independent checker into compact numeric arrays. It reproduced the original full route profiles while peaking at **276 MiB** on the largest instance.

## 90-second acceptance

The build is GNU C++ 11.4.0, Debug with effective O3, strict floating-point semantics, link-time optimization, and the optional CPLEX route-pool backend. The binary SHA-256 is `e9849f85aa5d23d9b1797a79e8701e97e7e95dd6f06f78c9d754c5f85bcedaa8`. Input hashes, source archives, commands, timing, solutions, and both checker outputs are preserved in `results/diagnostics/synthetic-speed-20261012/light3-cohort-90-seed{1,2,3}/`.

| Instance | Frozen BKS | Seed 1 | Seed 2 | Seed 3 | Maximum process seconds |
| --- | ---: | ---: | ---: | ---: | ---: |
| r500_p250_m250_s1 | 27,627 | 27,734 | 27,734 | 27,734 | 2.28 |
| r500_p250_m250_s2 | 28,036 | 28,153 | 28,153 | 28,153 | 1.83 |
| r500_p250_m250_s3 | 28,936 | 29,029 | 29,029 | 29,029 | 11.98 |
| r1000_p500_m500_s1 | 61,531 | 62,457 | 62,344 | 62,345 | 85.27 |
| r1000_p500_m500_s2 | 61,780 | 62,464 | 62,640 | 62,486 | 85.24 |
| r1000_p500_m500_s3 | 63,978 | 64,228 | 64,347 | 64,336 | 85.29 |
| r2000_p1000_m1000_s1 | 132,395 | 132,744 | 132,761 | 132,874 | 86.99 |
| r2000_p1000_m1000_s2 | 133,063 | 134,468 | 134,360 | 134,584 | 86.96 |
| r2000_p1000_m1000_s3 | 131,815 | 132,815 | 133,481 | 133,066 | 87.18 |

Whole-process time includes parsing, bank export, solution output, and cleanup. Validation happens afterward. Each search reads its instance and configuration; known-solution witnesses are used only by the separate checking process. All accepted runs use zero outer ALNS iterations, while generation retains lightweight neighborhood search.

## Quality within 180 seconds

The full seed-1 cohort uses `--generation-seconds 160`; all nine cases exceed the frozen targets, with a longest process time of **178.45s**. The three largest cases also use a matched `--generation-seconds 170` trial. Giving generation the extra ten seconds leaves a shorter ranked master, avoids the expensive dense final solve, and lowers the observed largest-instance memory peaks.

| Largest instance | 160s generation objective | 170s generation objective | Gain over matched 90s run | Process seconds at 170s generation | Peak RSS MiB |
| --- | ---: | ---: | ---: | ---: | ---: |
| r2000_p1000_m1000_s1 | 134,442 | 135,071 | +2,327 | 175.32 | 758.13 |
| r2000_p1000_m1000_s2 | 135,931 | 136,196 | +1,728 | 176.61 | 765.91 |
| r2000_p1000_m1000_s3 | 134,210 | 135,294 | +2,479 | 176.04 | 774.18 |

The 160-second generation cohort reached peak RSS **1,260.73 MiB**; the three 170-second trials stayed below **775 MiB**. More time does not guarantee a better stochastic outcome: r1000 s1 fell from 62,457 at 90 seconds to 62,438 in its longer run. Best-known export always retains the strongest checked fleet.

## Updated synthetic BKS

All nine live BKS solutions were rechecked with the current C++ checker and the compact independent Python simulator before replacement. Input hashes are unchanged, and `is_optimal=0` is retained. The ledger is [data/synthetic/best-known/best-known.csv](data/synthetic/best-known/best-known.csv); producing settings and fresh verification hashes are in [provenance.json](data/synthetic/best-known/provenance.json).

| Instance | Original target | Updated BKS |
| --- | ---: | ---: |
| r500_p250_m250_s1 | 27,627 | 27,734 |
| r500_p250_m250_s2 | 28,036 | 28,153 |
| r500_p250_m250_s3 | 28,936 | 29,029 |
| r1000_p500_m500_s1 | 61,531 | 62,503 |
| r1000_p500_m500_s2 | 61,780 | 62,783 |
| r1000_p500_m500_s3 | 63,978 | 64,514 |
| r2000_p1000_m1000_s1 | 132,395 | 135,071 |
| r2000_p1000_m1000_s2 | 133,063 | 136,196 |
| r2000_p1000_m1000_s3 | 131,815 | 135,567 |

The ledger retains the strongest independently checked fleet for each instance, including four records from prior runs. It is a record of feasible solutions, not a claim that the current solver reproduces every updated objective within 90 seconds. Frozen targets and witnesses preserve the original comparison after this ledger update.

## Resources and correctness

Validation permits up to six concurrent independent solves. The runner uses at most **two** shared solver slots across all invocations, on physical cores 2 and 4. Benchmark orchestration uses core 6 at lower priority. Each solver has a **1.5 GiB address-space cap**, a CPU-time cap, and an external timeout. Launches require 6 GiB of available host memory beyond the new job allowance. Checker subprocesses share the slot and run under memory caps; exported BKS Python checks use a 512 MiB cap.

The 13 regression targets pass. AddressSanitizer and UndefinedBehaviorSanitizer pass the core parser, insertion-cache, and route-generation/master fixtures. The acceptance audit verifies current compiled source, executable hashes, all instance and solution hashes, both checks, CPU affinity, and measured limits across **39** successful search runs.

Generation and route retention are bounded heuristics. Ranked-master completion certifies its subset; full-bank completion certifies the bank at that call. Final polishing can add routes afterward. Global optimality over every feasible route remains unproved. CPLEX is required for the measured large-instance configuration; the portable native backend remains available. Results concern these nine instances and the recorded hardware, settings, and seeds.

## Reproduction

```bash
cmake -S . -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug \
  -DSOICT_CPLEX_ROUTE_POOL=ON -DSOICT_AGGRESSIVE_OPTIMIZATION=ON \
  -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON \
  -DCPLEX_ROOT=/opt/ibm/ILOG/CPLEX_Studio2211
cmake --build cmake-build-debug --target SoICT2026 -j 2

./cmake-build-debug/SoICT2026 \
  --instance data/synthetic/r2000_p1000_m1000_s1.txt \
  --seed 1 --time-limit 90 --temp 200

./cmake-build-debug/SoICT2026 \
  --instance data/synthetic/r2000_p1000_m1000_s1.txt \
  --seed 1 --time-limit 180 --temp 200 --generation-seconds 170
```

Use the installed CPLEX path. The 170-second generation setting is checked here on the three largest instances; the full nine-instance longer cohort uses 160 seconds. The diagnostic runner keeps targets frozen and resource limits shared across invocations.

## Retired artifacts

Retired executable copies and route banks were removed; historical traces were losslessly compressed. Summaries, source snapshots, provenance, and checked solutions remain. The cleanup manifests are `results/cleanup-20261012.json` and `results/trace-compaction-20261012.json`. The generation and master design is described in [SYNTHETIC_SPEED.md](SYNTHETIC_SPEED.md).
