# Additional frozen-code P1 validation

This follow-up was declared before running in `p1-validation-plan.json`.
The actual production executables were run serially, with solver budgets
unchanged, a 320-second watchdog, and independent output checking. P1 source
remained `2242e3bc1bcbcfef7600d3023f4a2b00e6e23c7b755d106aa317e8fea0eb96eb`;
the A1 reference remained `dcc6e52f`. No tuning was performed during the run.
Both N and M are separately limited to 250.

## Full-driver results

| Input | Solver seeds | A1 mean | P1 mean | Result |
| --- | --- | ---: | ---: | --- |
| 0 passengers, 250 parcels, K16, generator seed1201 | 1, 2 | 11,993 | 11,993 | Tie |
| 50 passengers, 250 parcels, K49, generator seed1202 | 1, 2 | 31,340 | 31,340 | Tie |
| 250 passengers, 50 parcels, K32, generator seed1203 | 1 | 13,645 | 13,645 | Tie |
| Constructed 50-parcel capacity30 variant | 1 | 141 | 1,141 | +1,000; certified optimum 1,141 |

All 12 completed runs were feasible, with no crashes or watchdog timeouts. All
saved outputs were independently revalidated during aggregation; source, binary,
input and output hashes matched. Seeds are averaged within each input, not
counted as independent inputs. The certified case is a new parameterization of
the known failure family, not a fourth ordinary input or hidden-server sample.

The K49 input ran rebuilding in the actual main driver. Each run took roughly
147–152 seconds. The K16 and K32 ordinary inputs completed rapidly (about 0.25–0.60
seconds); there is no reason to force them to spend the full available budget.
The certified A1/P1 pair took about 102.40/101.78 seconds. These measurements do
not establish a runtime improvement.

## Additional component checks

Fifteen checks passed through the actual P1 parcel-seed entry point:

- Consolidation bundles of 10, 20, 50, 100, 200 and 250 parcels, all reaching their
  constructive exact optimum.
- Six corresponding vertex-label permutations, retaining the same objective.
- Capacity-two rejection, impossible delivery-window rejection and a
  passenger-only no-op, each returning the independently feasible empty fleet.

These exercise the seed mechanism, including the 250-parcel boundary, without
claiming that a short component run is a full-driver performance result.

## Interpretation and integration

P1 is integrated into `submission/submission.cpp`: rebuilding first tries normal
positive insertions and the existing pair escape, then calls the parcel-batch
seed once in its first round. It considers only idle taxis and unserved parcels,
uses a shared 50-ms allowance clipped to the phase deadline, and admits only a
complete route passing the full evaluator with positive true profit. The route
is stored in the same bank and participates in later master selection. This is
part of the normal standalone executable, not an external solver or test-only
fallback.

It addresses the confirmed higher-order parcel bootstrap gap while retaining
the existing search. It does not yet perform parcel-bundle ejection from occupied
taxis, prove global optimality, or establish higher ordinary-case contest scores.
The three new ordinary inputs tied; the only new full-driver gain was on the
certified constructed family. Keep this distinction when deciding what to claim
for submit 3.

Raw records are `p1-validation-components.json`,
`p1-validation-ordinary-results.json`, and
`p1-validation-certified-results.json`. The latter two use the general historical
aggregation helper; its `request_limit: 1000` denotes that helper's archive guard,
not this experiment's limits. The plan and actual dimensions enforce N<=250 and
M<=250. Representative and constructed results were deliberately aggregated
separately. Inputs, solutions and stderr logs remain under `.build/p1-validation/`.
Replay with `python tests/submission/validate_submit3.py`; it verifies frozen
versions and rechecks completed runs on resume.
