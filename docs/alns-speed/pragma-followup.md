# AVX2 and selective unrolling

Neither additional pragma improved the fresh r240, seed-1 comparison. The
production package retains `#pragma GCC optimize("O3","unroll-loops")`, with
no CPU target or explicit per-loop unroll factor.

| Package | Solver seconds | More runtime than baseline | Benefit |
| --- | ---: | ---: | ---: |
| Current baseline | 142.811 | — | 13279 |
| AVX2 target | 149.337 | 4.57% | 13279 |
| Explicit factor 4 | 152.108 | 6.51% | 13279 |
| Explicit factor 8 | 149.857 | 4.93% | 13279 |

All packages retained global O3 and automatic unrolling. Factors 4 and 8
overrode the compiler's unroll choice only for InsertionSearch's cost-array,
pickup-position, and delivery-position loops. The minimum/prefix reduction and
suffix recurrence received no explicit factor. AVX2 and local unrolling were
tested individually, not combined.

Each variant ran once on `r240_p120_m120_s5.txt`, seed 1, default temperature 200
and cooling, with a 300-second allowance. Builds used GCC 11.4.0 and identical
`-std=c++17 -O2 -DNDEBUG -pedantic-errors -Werror=pragmas` options. Compilation,
sample checks and preprocessing finished before the sequential full runs on
CPU 2. This machine supports AVX2; the live CMS CPU was not verified.

Every run completed 60,000 iterations and produced byte-identical final solutions,
all independently checked at benefit 13279. Samples matched at benefit 217.
All artifacts fit the 50,000-character cap. GCC preprocessing verified that each
local directive remained immediately before its intended loop after compaction.
Disassembly confirmed vector instructions in the AVX2 package.

The failed trial options were removed from the production packager. The archived
packager, compactor and wrapper preserve the experiment implementation and
reproduction commands in the full report (`results/diagnostics/pragma-followup-20261005/report.md`).
The current upload source remains byte-identical to the tested baseline, at
49,943 characters.

These are one-run observations on a shared machine, not statistical estimates of
runtime variance or multi-seed quality. A roughly 0.22-second disassembly audit
overlapped the baseline run; builds and preprocessing did not. Intermediate
search trajectories were not recorded by the judge package. These results do
not establish why the additions ran slower or that they are slower universally.
