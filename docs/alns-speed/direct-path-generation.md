# Direct route generation and fleet recombination

The default solver first generates feasible taxi routes independently. When every taxi's prefix search completes within its budget, it skips ALNS, keeps up to 200 routes per taxi, and runs the conflict-free fleet master. If generation is incomplete, it falls back to the 6,000-step ALNS. Banks above 2,000 columns use a native LP-based branch-and-bound with basis reuse; smaller banks retain the request-price search. See [the n500 follow-up](n500-route-master.md) for the current defaults and measurements.

Each prefix tracks departure time, accumulated travel cost and revenue, parcel load, picked requests, and open parcels. Passengers are appended as adjacent pickup/drop-off pairs. Open parcels can be delivered or carried across other requests. Only positive-profit complete routes enter the bank; for identical served sets on one taxi, the best sequence dominates.

Pickup candidates are sorted by their latest feasible departure from the current node. A conservative floating-point guard supports this index, and every actual arrival is checked. No triangle inequality or removal-only feasibility assumption is used. Prefix enumeration is capped at 1,000,000 per taxi and shares a generation deadline of at most two seconds across the taxis.

## Historical matched r240 results, 2026-10-11

These measurements used the preceding 50,000-prefix/48-route default. The n500 follow-up increases both limits and replaces the master for larger banks; the table below is not a measurement of that newer implementation.

| Seed | Previous solve | New solve | Solve speedup | Previous process | New process | Process speedup |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| 1 | 15.859 s | 0.0876 s | 181.1x | 15.908 s | 0.2316 s | 68.7x |
| 2 | 15.692 s | 0.0707 s | 222.0x | 15.743 s | 0.1201 s | 131.0x |
| 3 | 15.654 s | 0.0834 s | 187.7x | 15.704 s | 0.1306 s | 120.3x |

Ratios of mean runtimes: **195.3x** for solve time and **98.2x** for the whole executable. Process time includes instance reading and output/log writing. It excludes the external checker. Both variants use the same 300-second CLI cap and finish early.

Every matched run and the seven held-out seed runs returned the identical checked 13,279 fleet. On this input, completed generation is deterministic; seeds do not change its route bank. The extra seeds check seed independence, not distinct search trajectories.

The final runs generate all **427,881 feasible prefixes**, then select from **960 columns**. Mean generation and trimming time is **0.0469 s**; mean master time is **0.0318 s**. They execute zero ALNS iterations.

These numbers compare the previous 6,000-step default with the new default, using sequential CPU-2-pinned runs on an Intel i7-11850H. The fresh Release builds have identical solver compiler commands, effective -O2, GNU C++ 11.4.0, and CPLEX disabled.

## Coverage and correctness

With its bank cap raised for validation, the generator reproduces all 21,461 positive-profit served-set columns and profits from the independent complete r240 enumeration. The reference hash agrees with the integer upper-bound certificate. Its uncapped bank export is diagnostic evidence, not runtime input to the solver.

Independent endpoint-permutation tests check the generator against full evaluation on 40 directed/nonmetric fixtures, plus targeted shortcut, budget and nonfinite-arrival cases. Existing fleet enumeration tests cover 100 route-pool trials. All 12 regression tests and both focused ASan/UBSan tests pass.

Forcing a one-prefix budget makes generation incomplete and reproduces the baseline's 6,025 progress records and final fleet. Disabling route-pool repair respects the existing ALNS ablation.

The route bank is capped even when prefix enumeration completes. Therefore the completed master proves optimality only within that bank. The separately verified integer certificate establishes 13,279 as the global optimum for the unchanged r240 input. The n500 follow-up provides additional measurements; quality beyond those two instances remains unmeasured.

## Rejected reductions

Disabling all local search in the 6,000-step ALNS returned 13,273. Reducing the direct-generation bank to 32 routes per taxi returned 13,212 and took about two seconds. A smaller bank can remove complementary paths and make the remaining master harder. These variants remain archived rather than enabled.

## Commands

```bash
./cmake-build-debug/SoICT2026 solve-alns --instance data/probation/r240_p120_m120_s5.txt --seed 1 --time-limit 300 > /tmp/r240-paths.sol
```

Use `--disable-path-generation` for the previous 6,000-step solver. Add `--cooling 0.9998465061085267` to restore the original 60,000-step schedule. `--paths-only` forces route generation without ALNS fallback. `--path-prefix-limit`, `--path-pool-limit`, and `--route-pool-limit` control generation work, the completed-generation bank, and the fallback bank respectively.

Private evidence is preserved in `results/diagnostics/path-generation-20261011/`: frozen baseline and final sources/binaries, commands and hashes, per-seed logs and solutions, both checker outputs, uncapped bank export and its independent comparison, retired pilots, fallback trajectory validation, regression/sanitizer logs, and report scripts.
