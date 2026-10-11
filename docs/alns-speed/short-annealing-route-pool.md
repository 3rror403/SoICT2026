# Short annealing followed by route-pool repair

This first optimization stage shortened the cooling schedule to about 6,000 iterations instead of 60,000. The current solver uses [direct route generation](direct-path-generation.md) first and retains this ALNS as its fallback. The results below describe the preceding stage.

The r240 profile puts most runtime in AddRequest, ReplaceRequest, GreedyInsertion and ProfitInsertion. Route-pool repair contributes the final improvement at a small fraction of that cost. Shortening route-bank generation therefore removes repeated insertion work while retaining fleet recombination.

| Seed | Former default seconds | New default seconds | Speedup | Benefit |
| --- | ---: | ---: | ---: | ---: |
| 1 | 144.05 | 13.88 | 10.38x | 13279 |
| 2 | 140.65 | 13.97 | 10.07x | 13279 |
| 3 | 140.71 | 14.00 | 10.05x | 13279 |

The ratio of mean runtimes is **10.17x**, and every matched seed exceeds 8x. Both schedules finish under the same 300-second CLI cap. This measures end-to-end time for different amounts of search work; individual iterations have not been made eight times faster.

All six fleets pass the C++ full evaluator and an independent Python route simulation. The 13,279 integer upper-bound certificate was reverified. An approximately 3,000-step pilot returned 13,273 on seed 3, so it was rejected.

The fresh 6,000-step profile spends 83.5% in the four insertion/replacement components and about 0.057 seconds (0.41%) in final route-pool repair. Its final repair adds 314 benefit. The restricted route-pool search is not a general proof of global optimality; the separate complete-route certificate establishes the optimum for this input.

Measurements use r240_p120_m120_s5.txt, seeds 1–3, one process at a time on CPU 2, an Intel i7-11850H, GNU C++ 11.4.0, and fresh Release builds with effective -O2 and CPLEX disabled. The available 12 regression tests pass.

Run the rebuilt solver with:

```bash
./cmake-build-debug/SoICT2026 solve-alns --instance data/probation/r240_p120_m120_s5.txt --disable-path-generation --seed 1 --time-limit 300 > /tmp/r240-speed.sol
```

To restore the former annealing schedule, add `--cooling 0.9998465061085267`.

The new default applies to other instances as well, but their quality and speed have not been measured in this campaign. An original run cut short by a smaller time limit has a different comparison denominator.

Private evidence is preserved under `results/diagnostics/speed8-20261011/`: original and final source snapshots, binary and input hashes, compiler commands, matched seed logs, route banks, checked official solutions, profile instrumentation and CSVs, retired pilots, and the reproducible report scripts.
