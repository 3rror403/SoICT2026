# Heuristic evaluation and best-known solutions

The evaluator uses the synthetic instance manifest and selects cases with `N+M <= 100` (currently 20 instances). It runs each case with solver seeds `1` through `10`. Four runs execute concurrently by default; use `--jobs` to change that number. A full evaluation therefore has 200 trials. The default **60 seconds per run is a timeout, not a measured runtime**. CPLEX MIP is separate from this heuristic evaluator.

## Solver interface

The designated heuristic must be an executable accepting:

```text
SOLVER --instance INSTANCE --seed SEED --time-limit SECONDS --out SOLUTION
```

It should finish before the internal deadline (59 seconds by default), write a complete solution file, and exit with code zero. The evaluator launches each run in a separate process and kills it at 60 seconds if it has not returned. A killed run is recorded as `timeout`; any partial file is discarded. A completed run is accepted only after `SoICT2026 check-solution` verifies all routes and recomputes the objective. The evaluator accepts a `.py` solver by launching it with the current Python interpreter; a compiled solver can be passed directly. The built `SoICT2026` executable implements ALNS through this direct option interface; `solve-alns` is also available as a subcommand for manual runs. ALNS stops when its geometrically cooled temperature reaches `0.0001` times its initial temperature, or when the time budget expires, whichever comes first. The default cooling factor is `exp(log(0.0001)/60000)`, which puts the temperature floor at about 60,000 completed iterations. The default initial temperature `10` has floor `0.001`. Local search runs every iteration by default. Add-request scans all unserved requests, while each relocation move scans served requests until it applies one improving move or exhausts its requests; remove-request scans all served requests. `--ls-interval N` can set a longer interval. The solver prints `stop=temperature` or `stop=time_limit` with its measured runtime. With the evaluator's 60-second limit, its one-second margin and the solver's one-second margin leave about 58 seconds for search.

New solutions use the compact version 3 format:

```text
SOICT2026_SOLUTION 3
K A
taxi_id node_0 node_1 ...
... (A active-route lines total)
```

`K` is the total taxi count from the instance; `A` is the number of nonempty routes. Taxi IDs are zero based, strictly increasing, and less than `K`. Each listed line has at least one service node; the newline ends that taxi's route. Unlisted taxis are unused, and `A=0` is valid. Taxi IDs determine the route's origin node and thus its initial travel time and cost, even when capacities match. Each node ID follows the ordering in [synthetic-data.md](synthetic-data.md). The file stores routes only; the checker recomputes arrival times, loads, and benefit. The checker also reads version 1 files, which list all `K` routes including empty ones, and version 2 files, which include route lengths.

To evaluate the ALNS executable, run:

```sh
python3 src/eval/evaluate.py --solver-name alns --solver-exe ./cmake-build-debug/SoICT2026
```

Optional `--manifest`, `--checker`, `--best-known`, and `--results` paths support other datasets or local trials. Repeat `--instance FILE` to select specific cases from the main manifest. `--time-limit` may lower the allowance for a trial but cannot exceed 60 seconds. The evaluator does not run the MIP.

## Results

Each invocation creates the next directory `results/<solver>/v0001/`, `v0002/`, and so on, preserving earlier run records. Derived CSV metrics may be backfilled from those runs' archived logs. `metadata.json` records the start/end time, solver path, Git commit and dirty flag, job count, and time limit. `runs.csv` has one row per instance and solver seed, including status, verified objective when feasible, elapsed wall time, completed iterations when reported by the solver, stop reason, and log path. It is updated every ten completions so partial runs remain visible. `summary.csv` has one row per instance: best objective, mean objective over feasible runs, mean wall time over all ten attempts, `speed (it/s)`, temperature and time-limit stop counts, and feasible/timeout counts. `speed (it/s) = sum(completed iterations) / sum(wall_seconds)` across feasible runs; it is blank when the solver does not report iteration counts. The wall time is measured from solver process launch to exit, including startup and solution writing but excluding the separate checker. The instance's generation seed remains distinct from the solver seed.

### ALNS v0003 annealing-stop runtime

This representative pilot selects one existing synthetic instance at each size from the [main manifest](../data/synthetic/manifest.csv), runs ten solver seeds per instance, and used the then-default cooling factor `0.99`. All 40 runs were feasible and reached `stop=temperature`; none stopped at the safety deadline. The table reports measured `wall_seconds` from [runs.csv](../results/alns/v0003/runs.csv), including process startup and solution writing but excluding the separate checker run.

To repeat this pilot without mixing its best-known records with the full manifest's records, run:

```sh
python3 src/eval/evaluate.py --solver-name alns --solver-exe ./cmake-build-debug/SoICT2026 \
  --instance r10_p0_m10_s1.txt --instance r20_p10_m10_s3.txt \
  --instance r50_p25_m25_s3.txt --instance r100_p50_m50_s3.txt \
  --best-known /tmp/soict-annealing-pilot-best-known
```

| Requests | Runs | Mean wall time (s) | Speed (it/s) | Stop by temperature |
| ---: | ---: | ---: | ---: | ---: |
| 10 | 10 | 0.164626 | 5570.19 | 10 |
| 20 | 10 | 0.414743 | 2211.01 | 10 |
| 50 | 10 | 3.014613 | 304.19 | 10 |
| 100 | 10 | 18.982813 | 48.31 | 10 |

The corresponding objectives and per-instance statistics are in [summary.csv](../results/alns/v0003/summary.csv). These timings describe this pilot and cooling setting, not the full 20-instance evaluation. The original [v0001](../results/alns/v0001/summary.csv) evaluation used the time budget as its stopping rule, so its roughly 58-second runs measure budget consumption. The `0.995` cooling pilot in `v0002` was interrupted after 170 recorded runs because some 100-request runs still hit the deadline; its [metadata](../results/alns/v0002/metadata.json) marks it incomplete.

Faster cooling changes search quality as well as runtime. On the pilot's 100-request instance, mean benefit was `4437.426164` in v0003 versus `4441.833417` in the longer v0001 runs using the same ten solver seeds. This comparison is limited to that instance and the two different stopping settings.

### Single-run `-O2` speed check

After v0003, insertion search was changed to reuse the pickup segment and fixed route prefix while checking parcel delivery positions. The following local runs used `r100_p50_m50_s3.txt`, solver seed `1`, cooling `0.99`, and a Release build compiled with `-O2 -DNDEBUG`. Wall time covers solver process launch through exit; the checker ran afterward. Both runs stopped by temperature after 917 iterations and produced the same checked benefit, `4446.173466`, and identical route files.

| `-O2` solver | Wall time (s) | Speed (it/s) |
| --- | ---: | ---: |
| Before insertion-search change | 16.155737 | 56.76 |
| After insertion-search change | 10.625390 | 86.30 |

The observed speed gain was 52.0% on this one case and seed. It is a local paired check, not a full evaluation; the v0003 summary above remains the earlier `-O3` run with the previous insertion-search code.

The best-known directory is `data/synthetic/best-known/`. Each entry in `best-known.csv` identifies an instance by filename and records its verified objective, route file, source solver/version/seed, and `is_optimal`. A heuristic run can replace an unproved record only with a strictly better feasible route. It cannot set `is_optimal=1`; that flag requires an exact proof. The five 10-request cases were recorded separately from proven CPLEX MIP solves with:

```sh
python3 src/eval/record_mip_optima.py
```

This command checks the CPLEX status and validates each saved route before marking it optimal. It does not create a heuristic performance version.
