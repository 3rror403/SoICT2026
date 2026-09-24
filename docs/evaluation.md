# Heuristic evaluation and best-known solutions

The evaluator uses the synthetic instance manifest and selects cases with `N+M <= 100` (currently 20 instances). It runs each case with solver seeds `1` through `10`. The hard wall-time allowance is **60 seconds per run**. Four runs execute concurrently by default; use `--jobs` to change that number. A full evaluation therefore has 200 trials. CPLEX MIP is separate from this heuristic evaluator.

## Solver interface

The designated heuristic must be an executable accepting:

```text
SOLVER --instance INSTANCE --seed SEED --time-limit SECONDS --out SOLUTION
```

It should finish before the internal deadline (59 seconds by default), write a complete solution file, and exit with code zero. The evaluator launches each run in a separate process and kills it at 60 seconds if it has not returned. A killed run is recorded as `timeout`; any partial file is discarded. A completed run is accepted only after `SoICT2026 check-solution` verifies all routes and recomputes the objective. The evaluator accepts a `.py` solver by launching it with the current Python interpreter; a compiled solver can be passed directly. No heuristic solver is registered yet.

The solution format is:

```text
SOICT2026_SOLUTION 1
K
length_0 node_0 node_1 ...
...
length_(K-1) node_0 node_1 ...
```

Each node ID follows the ordering in [synthetic-data.md](synthetic-data.md). Empty taxi routes have length zero. The file stores the route only; arrival times, loads, and objective are recomputed by the checker.

Once a heuristic executable is available, run:

```sh
python3 src/eval/evaluate.py --solver-name my_solver --solver-exe ./cmake-build-debug/my_solver
```

Optional `--manifest`, `--checker`, `--best-known`, and `--results` paths support other datasets or local trials. `--time-limit` may lower the allowance for a trial but cannot exceed 60 seconds. The evaluator does not run the MIP.

## Results

Each invocation creates the next directory `results/<solver>/v0001/`, `v0002/`, and so on. Existing versions are never overwritten. `metadata.json` records the start/end time, solver path, Git commit and dirty flag, job count, and time limit. `runs.csv` has one row per instance and solver seed, including status, verified objective when feasible, elapsed wall time, and log path. It is updated every ten completions so partial runs remain visible. `summary.csv` has one row per instance: best objective, mean objective over feasible runs, mean wall time over all ten attempts, and feasible/timeout counts. The instance's generation seed remains distinct from the solver seed.

The best-known directory is `data/synthetic/best-known/`. Each entry in `best-known.csv` identifies an instance by filename and records its verified objective, route file, source solver/version/seed, and `is_optimal`. A heuristic run can replace an unproved record only with a strictly better feasible route. It cannot set `is_optimal=1`; that flag requires an exact proof. The five 10-request cases were recorded separately from proven CPLEX MIP solves with:

```sh
python3 src/eval/record_mip_optima.py
```

This command checks the CPLEX status and validates each saved route before marking it optimal. It does not create a heuristic performance version.
