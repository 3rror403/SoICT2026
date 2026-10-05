# GCC optimization pragmas for the judge package

The packaging default is now:

```cpp
#pragma GCC optimize("O3","unroll-loops")
```

The pragma is inserted at the beginning of the single-file package after
compaction. The packager reserves its characters before macro compression;
the readable source and metadata record the same choice. Modular solver sources
and CMake optimization settings are unchanged.

One sequential run per variant used `r240_p120_m120_s5.txt`, seed 1,
temperature 200, default cooling, and a 300-second budget. All three compiled
with GCC 11.4.0 using `-std=c++17 -O2 -DNDEBUG -pedantic-errors -Werror=pragmas`
and ran on CPU 2. The compacted solver bodies were identical except for the
pragma; all builds finished before timing began.

| Package | Solver seconds | Runtime reduction vs O2 | Benefit |
| --- | ---: | ---: | ---: |
| O2, no pragma | 156.249 | — | 13279 |
| O3 | 149.654 | 4.22% | 13279 |
| O3 and unrolling | 147.421 | 5.65% | 13279 |

All runs completed 60,000 iterations at the temperature floor. Their final
solution bytes were identical, matched the prior retained modular solver result,
and passed the independent full route checker. All sample checks returned 217.
The retained upload file contains 49,943 of the allowed 50,000 characters.

This is one run per variant on a shared machine; runtime variance and multi-seed
quality were not measured. The extra improvement from unrolling over O3 alone
was only 1.49%, and the experiment does not establish its repeatability.
The judge package omits intermediate trajectory logging, so validation covers
completed iterations and final solutions. These timings apply to the C++17
single-file judge package, rather than the modular diagnostic executable used
in previous reports. The experiment verifies local GCC compatibility; the live
contest compiler, CPU and acceptance were not tested.

Use `--optimization O3` to select O3 alone or `--optimization O2` to emit no
pragma. The default regeneration command is:

```bash
python3 scripts/pack_submission.py --time-limit 300 --seed 1 --readable-out submission/submission.readable.cpp
```

The complete experiment report (`results/diagnostics/pragma-20261005/report.md`)
contains raw timings, source and binary hashes, compiler commands, validation
logs, generated variants, and reproduction scripts.

The [AVX2 and selective-unrolling follow-up](pragma-followup.md) tested further
pragmas against a fresh baseline. None improved that comparison; the existing
global pragma remains the default.
