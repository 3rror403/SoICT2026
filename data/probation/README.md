# Probation test cases

The eight supplied cases use `rR_pN_mM_sI.txt`: `R=N+M`, and `I` is the case index, not a generation seed. Input and solution contents are preserved when renamed.

| Original name | Normalized name | Taxis |
| --- | --- | ---: |
| `sample.txt` | `r4_p2_m2_s1.txt` | 2 |
| `sample2.txt` | `r10_p5_m5_s2.txt` | 3 |
| `sample3.txt` | `r30_p15_m15_s3.txt` | 5 |
| `sample4.txt` | `r100_p50_m50_s4.txt` | 10 |
| `sample5.txt` | `r240_p120_m120_s5.txt` | 20 |
| `data1.txt` | `r6_p3_m3_s6.txt` | 2 |
| `data2.txt` | `r95_p47_m48_s7.txt` | 10 |
| `data3.txt` | `r50_p25_m25_s8.txt` | 6 |

[manifest.csv](manifest.csv) records all eight cases and their SHA-256 hashes. The evaluator uses this set by default, including the 240-request case: eight cases times ten solver seeds gives 80 trials.

`r4_p2_m2_s1.sol` contains the first sample's stated feasible routes and benefit 217.

## Best-known solutions

[best-known/best-known.csv](best-known/best-known.csv) indexes the supplied routes. Its first column, `r`, is the total request count; rows are sorted by this value. Matching solution files use the normalized instance stems. Additional routes and updated user routes have unknown solver seeds and no optimality claim. Their objectives are taken from the supplied files; the evaluator independently checks every indexed route before running.

[best-known/first-submission.json](best-known/first-submission.json) preserves the historical first CMS submission provenance for the original five cases. It is a historical snapshot, so later updated routes may differ.

`calibration.json` preserves the earlier distribution calibration of the four larger original cases. Synthetic datasets and their result archives have been removed; generator code remains available for development.
