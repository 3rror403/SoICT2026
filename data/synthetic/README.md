# Synthetic dataset

30 development instances, calibrated from the four larger supplied cases in
[`data/probation`](../probation/README.md). This approximates their observed
structure; it does not establish the hidden contest distribution.

## Dataset layout

Names: `r{requests}_p{passengers}_m{parcels}_s{seed}.txt`.
Each size has five passenger shares: 0%, approximately 25%, 50%, approximately
75%, and 100%, assigned seeds 1–5 respectively. The 50/50 cases match the
supplied composition; the other mixes explore robustness.

| Requests | 10 | 20 | 50 | 100 | 250 | 500 |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| Taxis | 3 | 4 | 6 | 10 | 21 | 39 |

Taxi counts interpolate linearly through observed `(requests,taxis)` pairs
`(4,2), (10,3), (30,5), (100,10), (240,20)`, rounding to the nearest integer.
Above 240 requests, extend the final segment. Extrapolation is a modeling choice.

## Generation rules

- **Locations:** draw coordinates independently in a 100 × 100 square.
- **Matrices:** `time = max(1, ceil(distance))`,
  `cost = max(1, ceil(2.48 × distance))`; diagonals are zero. The main profile
  is symmetric and satisfies triangle inequality. Geometry and the cost scale
  are approximations fitted to the supplied cases.
- **Request fields:** choose one of the four calibration cases uniformly, then
  one complete record uniformly. This preserves relationships among its fields.
  Taxi capacities use the same sampling approach. Service and weight are copied.
- **Small noise:** add an integer in `[-2,2]` to pickup earliest time, clamping to
  `[0,100]` for passengers and `[0,90]` for parcels. Multiply each window width
  independently by `U(0.9,1.1)`, round, and clamp to at least one.
- **Parcel delivery:** `Ed = Ep + service + time(P,D) + delta`, where the sampled
  record provides `delta` in `[0,10]`. Set `Ld = Ed + perturbed delivery width`.
- **Revenue:** `max(1, round(a × cost(P,D) + b + residual))`. Fit `a,b` separately
  for passengers and parcels with equal total weight per calibration case;
  reuse the sampled record's residual. Coefficients are in `generation.json`.
- **Feasibility:** resample pickup locations until at least one taxi can serve
  that request alone, with up to five units of arrival margin. After 4,096 failed
  draws, place the pickup at an origin. Parcel delivery timing and capacity are
  included. This does not guarantee simultaneous service or positive profit.

All serialized values are integers. `std::mt19937_64` uses the filename seed;
reproduction requires the same compiler/standard-library implementation.
`manifest.csv` records counts and input hashes; `generation.json` records the
rules, calibration sources, and build hashes. `validation.json` records the
replacement dataset's distribution and individual feasibility observations.

## Regenerate

From the repository root:

```sh
python3 scripts/calibrate_synthetic.py  # only needed if supplied cases change
cmake --build cmake-build-debug --target SoICT2026
bash scripts/generate_synthetic_data.sh
```

For a fresh distribution report (requires NumPy), run
`python3 scripts/summarize_synthetic.py`.

This overwrites the active 30 files and manifest. Archive/reset best-known
solutions and results whenever inputs change.

For optional **directed, potentially nonmetric** robustness data, write to a
separate directory:

```sh
bash scripts/generate_synthetic_data.sh cmake-build-debug/SoICT2026 /tmp/synthetic-directed directed-noise
```

That profile independently multiplies each directed arc by `U(0.9,1.1)`, sharing
the multiplier between time and cost, then rounds. It is permitted by the
problem and extends the supplied cases' observed matrix structure.

See [input/output details](../../docs/synthetic-data.md).
