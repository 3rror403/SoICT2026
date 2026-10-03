# Heuristic evaluation and best-known solutions

The evaluator uses `data/probation/manifest.csv` and all eight supplied instances, including the 240-request case. It runs each case with solver seeds `1` through `10`, for 80 trials. Four runs execute concurrently by default; use `--jobs` to change that number. The default 60 seconds per run is a hard timeout. CPLEX MIP is separate from this heuristic evaluator.

## Solver interface

The designated heuristic must be an executable accepting:

```text
SOLVER --instance INSTANCE --seed SEED --time-limit SECONDS --out SOLUTION
```

It should finish before the internal deadline (59 seconds by default), write a complete solution file, and exit with code zero. The evaluator launches each run in a separate process and kills it at the selected hard timeout (60 seconds by default, at most 300 seconds) if it has not returned. It gives the solver a one-second shorter time budget. A killed run is recorded as `timeout`; any partial file is discarded. A completed run is accepted only after `SoICT2026 check-solution` verifies all routes and recomputes the objective. The evaluator accepts a `.py` solver by launching it with the current Python interpreter; a compiled solver can be passed directly. The built `SoICT2026` executable implements ALNS through this direct option interface; `solve-alns` is also available as a subcommand for manual runs. ALNS stops when its geometrically cooled temperature reaches `0.0001` times its initial temperature, or when the time budget expires, whichever comes first. The default cooling factor is `exp(log(0.0001)/60000)`, which puts the temperature floor at about 60,000 completed iterations. The default initial temperature `200` has floor `0.02`. Local search runs every iteration by default, with at most two full cycles of remove-request, add-request, passenger relocation, parcel relocation, clean-cut suffix exchange, and [request replacement](request-replacement.md). Each move applies improvements until its neighborhood is exhausted. If the first cycle finds no improvement across all moves, local search stops after that cycle. `--ls-interval N` can set a longer interval. The solver prints `stop=temperature` or `stop=time_limit` with its measured runtime.

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

Diagnostics go to standard error in this mode. The file interface also accepts `--instance -` and `--out -`. Omitting `--out` prints the solution to standard output. ALNS reports the initial solution and every completed iteration on standard error, including elapsed seconds, temperature, current benefit, and best benefit; improvements also produce a new-best message.

To evaluate the ALNS executable, run:

```sh
python3 src/eval/evaluate.py --solver-name alns --solver-exe ./cmake-build-debug/SoICT2026
```

Optional `--manifest`, `--checker`, `--best-known`, and `--results` paths support other datasets or local trials. Repeat `--instance FILE` to select specific cases from the probation manifest. `--time-limit` sets the hard allowance for a trial and can be at most 300 seconds. The evaluator does not run the MIP.

`--solver-temp T` passes an initial temperature to ALNS and records it in run
metadata. ALNS enables [clean-cut suffix exchange](clean-cut-2opt-star.md) by
default. `--solver-disable-clean-cut-2opt-star` disables it, while
`--solver-clean-cut-2opt-star` explicitly enables it. When neither option is
supplied, the evaluator uses the solver's default and records the override as
`null` in metadata.

## Results

Each invocation creates the next directory `results/<solver>/v0001/`, `v0002/`, and so on, preserving earlier run records. Derived CSV metrics may be backfilled from those runs' archived logs. `metadata.json` records the start/end time, solver path, Git commit and dirty flag, job count, and time limit. `runs.csv` has one row per instance and solver seed, including status, verified objective when feasible, elapsed wall time, completed iterations when reported by the solver, stop reason, and log path. It is updated every ten completions so partial runs remain visible. `summary.csv` has one row per instance: best objective, mean objective over feasible runs, mean wall time over all ten attempts, `speed (it/s)`, temperature and time-limit stop counts, and feasible/timeout counts. `speed (it/s) = sum(completed iterations) / sum(wall_seconds)` across feasible runs; it is blank when the solver does not report iteration counts. The wall time is measured from solver process launch to exit, including startup and solution writing but excluding the separate checker. For probation instances, `instance_seed` records the case index; it is distinct from the solver seed.

The evaluator prints an **expected score**, equal to the sum of each selected
instance's mean objective over the ten seeds. It saves the score in
`aggregate.csv` and `metadata.json`. A full probation evaluation sums all eight
instance means; selecting fewer instances gives the score for that subset.
The score is reported only when every run is feasible. Otherwise it is blank
in the CSV and `null` in metadata, with `score_complete` set to false.

## Dataset identity and best-known records

The active dataset is the eight supplied probation cases, listed in [the dataset README](../data/probation/README.md). Synthetic inputs and their archived results have been removed.

The evaluator records manifest and instance SHA-256 hashes in each run's metadata. Each best-known row includes `instance_sha256`; a mismatched or missing hash causes rejection before experiments. Manifest hashes are also verified.

The active best-known directory is `data/probation/best-known/`. The CSV starts with `r`, the total request count, and rows are sorted by it. Records include the objective, route file, source solver/version/seed when known, instance hash, and `is_optimal`. The evaluator independently checks these routes before use. A heuristic can improve an unproved record but cannot mark it optimal. The original CMS submission provenance remains a historical snapshot.

For the 10-request case, exact proofs can be recorded separately using:

```sh
python3 src/eval/record_mip_optima.py
```
