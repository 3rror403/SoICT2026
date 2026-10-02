# Supplied probation distribution and v3 calibration

Measurements compare the five supplied cases against the archived v2 generator, before the v3 replacement. Quantiles describe observations; the original generation algorithm is unknown.

| Requests | Taxis | Requests/taxi | Median travel time | Passenger window median | Parcel pickup window median | Parcel delivery window median |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 4 | 2 | 2.0 | 17.0 | 22.5 | 35.0 | 42.5 |
| 10 | 3 | 3.33 | 46.0 | 9.0 | 20.0 | 43.0 |
| 30 | 5 | 6.0 | 52.0 | 11.0 | 21.0 | 40.0 |
| 100 | 10 | 10.0 | 50.0 | 13.0 | 17.0 | 39.0 |
| 240 | 20 | 12.0 | 50.0 | 14.0 | 19.0 | 40.0 |

## Measured conclusions

- All five time and cost matrices are symmetric, nonnegative, integer-valued, and have zero diagonals.
- Both matrices satisfy triangle inequality exactly in the 10-, 30-, 100-, and 240-request cases. The four-request example has four time and two cost triangle violations, each of magnitude one.
- Archived v2 synthetic 10- and 100-request mixed cases have about 7% of distinct ordered triples violating the travel-time triangle inequality; median time is 17 versus 46–52 in the four larger supplied cases.
- The larger supplied cases have passenger earliest times in 0–100, parcel earliest times in 0–90, capacities in 20–30, weights in 5–15, and services in 2–5.
- All 190 parcels in the four larger supplied cases satisfy Ed = Ep + service + direct_time + delta, with integer delta in 0–10.
- All 380 requests in those four cases are individually feasible on at least one initially empty taxi. This is not a claim that all requests can be served together.
- Larger-case median revenue/direct-cost ratios lie near 2, while archived v2 100-request synthetic medians are 3.41 for passengers and 3.52 for parcels.
- A two-dimensional Euclidean embedding fits the larger travel-time matrices with relative RMSE 0.56%–1.08%. This supports a geometric generation hypothesis; it does not identify the exact coordinate generator or rounding/closure rule.

## Implications for generation

Use the four larger supplied cases for the main calibration; retain the four-request example as a format/feasibility fixture. Reflect measured fleet counts rather than K=R/2; no single fleet scaling law is established by five cases. Generate a geometric base near a 100-by-100 scale and preserve metric structure for the observed-data profile. Keep asymmetric/nonmetric variants as robustness coverage because the problem permits them. Resample pickup-time, window-width, capacity, weight, and service distributions from larger cases; set parcel delivery earliest relative to direct travel and pickup service. Calibrate revenue against direct travel cost rather than copying fixed 80–120 values. Audit marginal quantiles, correlations, matrix symmetry/metric properties, and individual request feasibility before replacing data.

The implemented approximation is documented in the [synthetic README](../data/synthetic/README.md); calibration source hashes and revenue coefficients are in [calibration.json](../data/probation/calibration.json).
