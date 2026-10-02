# Probation test cases

The five supplied cases use the synthetic folder's `rR_pN_mM_sI.txt` filename layout. Here `R=N+M` and `I` is the original supplied case index, not a generation seed. The instance contents are unchanged.

| Original name | Normalized name | Taxis |
| --- | --- | ---: |
| `sample.txt` | `r4_p2_m2_s1.txt` | 2 |
| `sample2.txt` | `r10_p5_m5_s2.txt` | 3 |
| `sample3.txt` | `r30_p15_m15_s3.txt` | 5 |
| `sample4.txt` | `r100_p50_m50_s4.txt` | 10 |
| `sample5.txt` | `r240_p120_m120_s5.txt` | 20 |

`r4_p2_m2_s1.sol` contains the first sample's stated feasible routes and benefit 217.

The four larger cases calibrate the [synthetic generator](../synthetic/README.md).
`calibration.json` records their hashes and fitted revenue coefficients; rebuild
it and the embedded records with `python3 scripts/calibrate_synthetic.py`.

## Best-known solutions

[best-known/best-known.csv](best-known/best-known.csv) records independently
checked routes from the first CMS submission, with benefits 217, 208, 1144,
4070, and 11856 in the case order above. Route files are in the same directory.
Every row includes the input SHA-256 and `is_optimal=0`; the submission seed
was not supplied. [first-submission.json](best-known/first-submission.json)
records provenance, route hashes, and checker results. These records belong to
the supplied probation inputs and are separate from synthetic best-known records.
