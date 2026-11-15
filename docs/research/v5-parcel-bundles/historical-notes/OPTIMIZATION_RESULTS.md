# Local optimization results

All production changes are in `submission/submission.cpp`. No commit or push was performed. The archived baseline is GitHub master commit `e6645d6`.

This document records the first storage/pair-seeding change at source hash `5a3c...`. Later [adversarial evaluation](hard/README.md) found severe full-program quality gaps; matching the probation cohort does not establish general quality. A subsequent input-reader optimization has a separate source hash and separate parser/large-screen evidence.

## Changes

1. **Indexed capped-route eviction.** A lazily built ordered index replaces two full-bank minimum scans on eviction. Keys reference stable map entries, avoiding duplicate served-set vectors. Equal profits still evict the lexicographically first served set. Existing scores are removed from the index before mutation; protected insertions invalidate the index, so uncapped rebuilding does not maintain it unnecessarily. Building the index costs O(B log B); later accepted indexed updates cost O(log B), apart from route/key construction and comparisons, rather than scanning B routes.
2. **Bounded pair seeding.** When the first rebuilding proposal remains empty after profitable single insertion, try complete two-request routes. All six pickup-before-delivery interleavings are considered; the full evaluator checks passenger adjacency, windows and capacity. The search inspects at most 24 ranked requests and eight rotating taxis, with a 20 ms allowance clipped to the phase deadline. It runs once per rebuilding phase, preserves profitable routes in the bank and seeds the best feasible pair. It does not change ordinary nonempty rebuilding or claim to enumerate every profitable bundle.

## Correctness checks

The final C++17 release build passed all **14 test groups**, including:

- 22 hand-calculated boundary cases and 100 random exact comparisons, seeds 42000–42099.
- Two supplied exact cases (objectives 217 and 283).
- 122 exhaustive route-bank comparisons, 363 exhaustive insertion comparisons and 201 exact set-packing comparisons.
- 300 cache-mutation comparisons and seven fleet-size dispatch boundaries.
- 4,650 bank operations checked against an independent Python eviction-policy reference at five caps, including ties, existing-key upgrades and protected insertions.
- 32 pair-seed comparisons against exact two-request routes.

The eviction-policy and pair-seed tests also passed with a separate `_GLIBCXX_DEBUG` build. This checks standard-container iterator use; it is not an AddressSanitizer run.

## Fixed-work measurements

Compiler: MinGW g++ 13.2.0, C++17, `-O2`, with the source's existing O3/unroll pragma. Both versions used the same flags. Five repetitions alternated baseline/candidate order, with one benchmark process at a time. The figures below are medians measured inside the diagnostic probe; process startup and input parsing are excluded.

| Workload | Before | After | Ratio before/after |
| --- | ---: | ---: | ---: |
| 50,000 bank updates, cap 400 | 0.374787 s | 0.055458 s | 6.76x |
| 50,000 bank updates, cap 1,000 | 0.736336 s | 0.057053 s | 12.91x |
| Initial DFS, supplied r240 | 0.066142 s | 0.032142 s | 2.06x |
| Fixed-prefix DFS, 12 dense passenger requests | 0.376611 s | 0.385395 s | 0.98x |

Each fixed-work pair retained identical bank checksums, route counts and work counts. Smaller supplied DFS cases differed by roughly 1–3%, with mixed signs. The synthetic bank workload deliberately stresses eviction; its speedup is **not** a whole-solver speedup.

## Actual full-program comparison

The archived and optimized standalone executables were run on five supplied instances, with solver seeds 1, 2 and 3: **15 before/after pairs, 30 process runs**. Their production search budgets were not shortened. Every output was independently checked. All 15 pairs produced byte-identical solutions and matched the recorded best-known objective.

| Instance requests | Objective, both versions | Before median wall time | After median wall time |
| --- | ---: | ---: | ---: |
| 30 | 1,530 | 0.049 s | 0.045 s |
| 50 | 2,168 | 0.078 s | 0.073 s |
| 95 | 5,702 | 0.241 s | 0.237 s |
| 100 | 4,285 | 0.262 s | 0.276 s |
| 240 | 13,279 | 1.429 s | 1.396 s |

Whole-process results include parsing, search, output and startup. These short runs show mixed runtime changes; a uniform end-to-end speedup is not established. In particular, r100 was about 14 ms slower in this cohort. No claim is made for 1,000–2,000-request instances, memory scalability or a general improvement in final objective.

## Targeted rebuilding quality

These comparisons run rebuilding from empty for the same short allowance; they isolate the generator rather than the full solver.

| Case | Baseline rebuilding | Optimized rebuilding | Exact objective |
| --- | ---: | ---: | ---: |
| Two individually loss-making passenger requests | 0 | 4 | 4 |
| Parcel pair requiring interleaving, capacity 2 | 0 | 4 | 4 |
| Same parcel pair, capacity 1 | 0 | 0 | 0 |
| Zero-revenue bridge on nonmetric travel | 0 | 20 | 20 |

The original full solver already finds the profitable pairs through DFS on these tiny instances. The improvement gives rebuilding an additional escape when it cannot start from a profitable singleton; it is not evidence of a full-solver objective gain on the probation cohort. Price/diversity retention changes, broader swap neighborhoods and adaptive phase budgets were not included in this change.

## Evidence and reproduction

- `baseline.json`: original small-instance baseline.
- `optimization-tests.json`: final test counts and source hash.
- `optimization-comparison.json`: per-repetition component timings, all 30 full-program runs, checksums and targeted generator results.
- `.build/before/`: local archived source, baseline executables and the pre-change probe snapshot. It is ignored by Git. That probe predates the new `seed-pairs` entry point; the comparison uses the common `rebuild` entry point for both versions.

From the repository root, with the archived baseline still present:

```powershell
python tests/submission/run_tests.py --random-cases 100
python tests/submission/benchmark.py --repetitions 5 --full --seeds 1 2 3
```

Baseline working-copy SHA-256: `b786b4c76bad097f20d784aa6f18e35c19c62e932b344ef47d7916500eab49cc`.

Optimized source SHA-256: `5a3c8105f305d4ca0ddc7eebbbbdfd44d00bce8d343171c87a84ff6cbd2ebeb0`.
