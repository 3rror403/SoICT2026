# Heuristic evaluation and best-known solutions

The evaluator uses the synthetic instance manifest and selects cases with `N+M <= 100` (currently 20 instances). It runs each case with solver seeds `1` through `10`. Four runs execute concurrently by default; use `--jobs` to change that number. A full evaluation therefore has 200 trials. The default **60 seconds per run is a timeout, not a measured runtime**. CPLEX MIP is separate from this heuristic evaluator.

## Solver interface

The designated heuristic must be an executable accepting:

```text
SOLVER --instance INSTANCE --seed SEED --time-limit SECONDS --out SOLUTION
```

It should finish before the internal deadline (59 seconds by default), write a complete solution file, and exit with code zero. The evaluator launches each run in a separate process and kills it at 60 seconds if it has not returned. A killed run is recorded as `timeout`; any partial file is discarded. A completed run is accepted only after `SoICT2026 check-solution` verifies all routes and recomputes the objective. The evaluator accepts a `.py` solver by launching it with the current Python interpreter; a compiled solver can be passed directly. The built `SoICT2026` executable implements ALNS through this direct option interface; `solve-alns` is also available as a subcommand for manual runs. ALNS stops when its geometrically cooled temperature reaches `0.0001` times its initial temperature, or when the time budget expires, whichever comes first. The default cooling factor is `exp(log(0.0001)/60000)`, which puts the temperature floor at about 60,000 completed iterations. The default initial temperature `10` has floor `0.001`. Local search runs every iteration by default. Add-request scans all unserved requests, while each relocation move scans served requests until it applies one improving move or exhausts its requests; remove-request scans all served requests. `--ls-interval N` can set a longer interval. The solver prints `stop=temperature` or `stop=time_limit` with its measured runtime. With the evaluator's 60-second limit, its one-second margin and the solver's one-second margin leave about 58 seconds for search.

Solutions use the official probation output format:

```text
Benefit
m_1 vertex_1 vertex_2 ...
... (exactly K route lines)
```

All vertex IDs are one based and refer to the instance's original matrix labels. Origins are omitted. An unused taxi has a line containing `0`. `SoICT2026 check-solution INSTANCE SOLUTION` verifies complete service, passenger adjacency, parcel precedence, taxi capacity, time windows, and the claimed benefit. No internal format marker or taxi ID appears in the output.

For standard input/output, use:

```sh
./cmake-build-debug/SoICT2026 solve-stdio --seed 1 --time-limit 60 < instance.txt > solution.txt
```

Diagnostics go to standard error in this mode. The file interface also accepts `--instance -` and `--out -`.

To evaluate the ALNS executable, run:

```sh
python3 src/eval/evaluate.py --solver-name alns --solver-exe ./cmake-build-debug/SoICT2026
```

Optional `--manifest`, `--checker`, `--best-known`, and `--results` paths support other datasets or local trials. Repeat `--instance FILE` to select specific cases from the main manifest. `--time-limit` may lower the allowance for a trial but cannot exceed 60 seconds. The evaluator does not run the MIP.

## Results

Each invocation creates the next directory `results/<solver>/v0001/`, `v0002/`, and so on, preserving earlier run records. Derived CSV metrics may be backfilled from those runs' archived logs. `metadata.json` records the start/end time, solver path, Git commit and dirty flag, job count, and time limit. `runs.csv` has one row per instance and solver seed, including status, verified objective when feasible, elapsed wall time, completed iterations when reported by the solver, stop reason, and log path. It is updated every ten completions so partial runs remain visible. `summary.csv` has one row per instance: best objective, mean objective over feasible runs, mean wall time over all ten attempts, `speed (it/s)`, temperature and time-limit stop counts, and feasible/timeout counts. `speed (it/s) = sum(completed iterations) / sum(wall_seconds)` across feasible runs; it is blank when the solver does not report iteration counts. The wall time is measured from solver process launch to exit, including startup and solution writing but excluding the separate checker. The instance's generation seed remains distinct from the solver seed.

## Dataset identity and best-known records

The active dataset is `probation-calibrated-v3`, described in [synthetic-data.md](synthetic-data.md). Instance names are unchanged from the earlier dataset, but their contents and taxi counts have changed. Previous generations and their best-known records are archived under `results/dataset-archive/`; the immediately preceding directed profile is `20261002-directed-probation-v2`. Their numerical comparisons remain valid only within that archived dataset.

The evaluator records manifest and instance SHA-256 hashes in each new run's metadata. Each best-known row includes `instance_sha256`; a mismatched or missing hash causes rejection before running experiments. When provided, manifest hashes are also verified. Changing instance files requires regenerating the manifest and resetting or archiving the matching best-known records.

The active best-known directory is `data/synthetic/best-known/`. It starts with an empty index after replacement. Each future entry records the checked objective, route file, source solver/version/seed, instance hash, and `is_optimal`. A heuristic can improve an unproved record but cannot mark it optimal. Exact proofs can be recorded separately using:

```sh
python3 src/eval/record_mip_optima.py
```

No old optimum is transferred to the new dataset, and replacing the dataset does not itself run a full solver evaluation.
