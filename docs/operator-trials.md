# Operator trials on the probation set

The repaired shared-gap pair repair and delay-removal mechanisms from
`origin/nguyen_test_02@c16b2b6` were compared individually and together against
the current solver with clean-cut local search enabled.

The comparison used `r95_p47_m48_s7.txt`, seeds 1–10, initial temperature 200,
the default cooling schedule, local search every iteration, and the same
Release executable. Each variant ran ten single-threaded processes concurrently;
the four batches ran sequentially. Every run completed 60,000 iterations before
the 60-second hard timeout, and all forty final solutions were independently
feasible.

| Variant | Mean benefit | Best benefit | Worst benefit | Mean time (s) | Iterations/s |
| --- | ---: | ---: | ---: | ---: | ---: |
| Clean-cut baseline | 5596.2 | 5682 | 5550 | 37.61 | 1595.49 |
| Add pair repair | 5577.6 | 5626 | 5537 | 43.76 | 1371.04 |
| Add delay removal | 5561.7 | 5648 | 5496 | 37.82 | 1586.46 |
| Add both | 5575.2 | 5642 | 5504 | 40.92 | 1466.39 |

Relative to the clean-cut baseline, pair repair changed mean benefit by
-0.33% and mean runtime by +16.37%; delay removal changed them by -0.62% and
+0.57%; the combination changed them by -0.38% and +8.80%. Matched wins/ties/losses
were 4/1/5, 4/0/6, and 4/1/5, respectively.

The tested additions were retired from the active implementation and CLI.
All three variants had lower mean, best, and worst benefit. The paired bootstrap
95% intervals include zero, so the result does not establish that these
mechanisms are universally harmful. It provides no supporting evidence for
retaining these versions on the selected instance.

## Tested adaptations

Pair repair used the previously unused eight-neighbor partner bank, considered
all taxis, preserved precedence and passenger adjacency, recomputed the gain
when applying a block, and limited each repair call to two accepted pair
insertions. Delay removal checked shortcut feasibility, calculated the reduction
in actual finishing time including waiting, and reconsidered every remaining
request on the changed taxi after each removal.

Pair insertion matched full solution evaluation across 12,000 candidates.
Delay scores matched independent timing calculations across 200 candidates.
A dedicated regression confirmed that two individually unprofitable requests
can form a profitable pair. Existing ALNS, clean-cut, and probation I/O tests
passed, as did a short r95 run with per-iteration full checking and both additions.

The [archived report](../results/pair-delay-r95-t200-20261003/report.md) contains
all seed pairs, confidence intervals, operator counters, exact commands, source
and binary hashes, the repaired source and tests, and isolated best routes.
The supplied reference files were preserved. The clean-cut option remains
available in the active solver.

## Lesson

Adding feasible moves or stronger repair does not guarantee better final solutions. On r95, pair repair and delay removal lowered mean benefit, individually and together, despite passing correctness checks. Retain an operator only when matched-seed results justify its quality and runtime cost.
