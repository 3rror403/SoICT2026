#!/usr/bin/env python3
"""Run a heuristic over the synthetic instances with a hard process timeout."""

import argparse
import csv
import datetime as dt
import json
import math
import os
from pathlib import Path
import re
import shutil
import signal
import statistics
import subprocess
import sys
import time
from concurrent.futures import ThreadPoolExecutor, as_completed


ROOT = Path(__file__).resolve().parents[2]
RUN_FIELDS = ["instance", "instance_seed", "solver_seed", "status", "objective", "wall_seconds", "log"]
BEST_FIELDS = ["instance", "objective", "is_optimal", "solution", "solver", "version", "seed"]


def write_csv(path, fields, rows):
    temporary = path.with_name(path.name + ".tmp")
    with temporary.open("w", newline="", encoding="utf-8") as stream:
        writer = csv.DictWriter(stream, fieldnames=fields)
        writer.writeheader()
        writer.writerows(rows)
    temporary.replace(path)


def read_cases(manifest):
    with manifest.open(newline="", encoding="utf-8") as stream:
        rows = list(csv.DictReader(stream))
    cases = []
    all_instances = {}
    seen = set()
    for row in rows:
        name = row["file"]
        if Path(name).name != name or name in seen:
            raise ValueError(f"invalid or duplicate instance name: {name}")
        seen.add(name)
        requests = int(row["requests"])
        if int(row["passengers"]) + int(row["parcels"]) != requests:
            raise ValueError(f"inconsistent manifest row: {name}")
        path = manifest.parent / name
        if not path.is_file():
            raise FileNotFoundError(path)
        all_instances[name] = path.resolve()
        if requests <= 100:
            cases.append((row, path.resolve()))
    if not cases:
        raise ValueError("manifest has no instances with at most 100 requests")
    return cases, all_instances


def command_for(executable, arguments):
    if executable.suffix.lower() == ".py":
        return [sys.executable, str(executable), *arguments]
    return [str(executable), *arguments]


def kill_process(process):
    if os.name == "posix":
        try:
            os.killpg(process.pid, signal.SIGKILL)
        except ProcessLookupError:
            pass
    else:
        process.kill()
    process.wait()


def run_one(case, seed, solver, checker, run_dir, limit):
    row, instance = case
    name = row["file"]
    stem = f"{instance.stem}_seed{seed}"
    solution = run_dir / "work" / f"{stem}.sol"
    log = run_dir / "logs" / f"{stem}.log"
    internal_limit = min(59.0, max(0.01, limit - 1.0))
    args = ["--instance", str(instance), "--seed", str(seed),
            "--time-limit", str(internal_limit), "--out", str(solution)]
    started = time.monotonic()
    elapsed = None
    status = "process_error"
    objective = ""
    with log.open("wb") as stream:
        try:
            process = subprocess.Popen(command_for(solver, args), cwd=ROOT,
                                       stdout=stream, stderr=subprocess.STDOUT,
                                       start_new_session=(os.name == "posix"))
            try:
                process.wait(timeout=max(0.001, limit - (time.monotonic() - started)))
            except subprocess.TimeoutExpired:
                kill_process(process)
                elapsed = time.monotonic() - started
                status = "timeout"
            else:
                elapsed = time.monotonic() - started
                if elapsed >= limit:
                    status = "timeout"
                elif process.returncode == 0 and solution.is_file():
                    checked = subprocess.run(command_for(checker, ["check-solution", str(instance), str(solution)]),
                                             capture_output=True, text=True, timeout=30)
                    if checked.returncode == 0 and checked.stdout.startswith("benefit="):
                        objective = float(checked.stdout.split("=", 1)[1].strip())
                        if not math.isfinite(objective):
                            raise ValueError("nonfinite checked objective")
                        status = "feasible"
                    else:
                        stream.write((checked.stdout + checked.stderr).encode("utf-8", errors="replace"))
                        status = "invalid_solution"
        except Exception as error:
            stream.write(("\nEvaluator error: " + str(error) + "\n").encode("utf-8", errors="replace"))
            status = "process_error"
    if elapsed is None:
        elapsed = time.monotonic() - started
    return ({"instance": name, "instance_seed": row["seed"], "solver_seed": seed,
             "status": status, "objective": objective,
             "wall_seconds": f"{elapsed:.6f}",
             "log": str(log.relative_to(run_dir))}, solution if status == "feasible" else None)


def read_best_index(path, checker, instances):
    if not path.exists():
        return {}
    with path.open(newline="", encoding="utf-8") as stream:
        rows = list(csv.DictReader(stream))
    best = {}
    for row in rows:
        name = row["instance"]
        if name in best or name not in instances or Path(row["solution"]).name != row["solution"]:
            raise ValueError(f"invalid best-known entry: {name}")
        solution = path.parent / row["solution"]
        checked = subprocess.run(command_for(checker, ["check-solution", str(instances[name]), str(solution)]),
                                 capture_output=True, text=True, timeout=30)
        if checked.returncode != 0 or not checked.stdout.startswith("benefit="):
            raise ValueError(f"best-known route is invalid: {name}")
        actual = float(checked.stdout.split("=", 1)[1].strip())
        recorded = float(row["objective"])
        if not math.isfinite(recorded) or abs(actual - recorded) > 1e-6 * (1 + abs(recorded)):
            raise ValueError(f"best-known objective disagrees with its route: {name}")
        if row["is_optimal"] not in ("0", "1"):
            raise ValueError(f"invalid optimality flag: {name}")
        best[name] = row
    return best


def make_version(results_root, solver_name):
    parent = results_root / solver_name
    parent.mkdir(parents=True, exist_ok=True)
    for number in range(1, 10000):
        path = parent / f"v{number:04d}"
        try:
            path.mkdir()
            (path / "work").mkdir()
            (path / "logs").mkdir()
            return path
        except FileExistsError:
            continue
    raise RuntimeError("too many evaluation versions")


def git_state():
    commit = subprocess.run(["git", "rev-parse", "HEAD"], cwd=ROOT,
                            capture_output=True, text=True)
    dirty = subprocess.run(["git", "status", "--porcelain"], cwd=ROOT,
                           capture_output=True, text=True)
    return {"commit": commit.stdout.strip() if commit.returncode == 0 else "unknown",
            "dirty": bool(dirty.stdout.strip()) if dirty.returncode == 0 else None}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--solver-name", required=True)
    parser.add_argument("--solver-exe", type=Path, required=True)
    parser.add_argument("--checker", type=Path, default=ROOT / "cmake-build-debug" / ("SoICT2026.exe" if os.name == "nt" else "SoICT2026"))
    parser.add_argument("--manifest", type=Path, default=ROOT / "data/synthetic/manifest.csv")
    parser.add_argument("--best-known", type=Path, default=ROOT / "data/synthetic/best-known")
    parser.add_argument("--results", type=Path, default=ROOT / "results")
    parser.add_argument("--jobs", type=int, default=4)
    parser.add_argument("--time-limit", type=float, default=60.0)
    args = parser.parse_args()
    if not re.fullmatch(r"[A-Za-z0-9_-]+", args.solver_name):
        parser.error("solver name may contain only letters, digits, underscores, and hyphens")
    if args.jobs < 1 or args.jobs > 64:
        parser.error("jobs must be between 1 and 64")
    if not math.isfinite(args.time_limit) or not 0 < args.time_limit <= 60:
        parser.error("time limit must be positive and at most 60 seconds")
    solver = args.solver_exe.resolve()
    checker = args.checker.resolve()
    if not solver.is_file() or not checker.is_file():
        parser.error("solver executable and checker must exist")
    manifest = args.manifest.resolve()
    cases, instances = read_cases(manifest)
    best_dir = args.best_known.resolve()
    best_dir.mkdir(parents=True, exist_ok=True)
    index = best_dir / "best-known.csv"
    best = read_best_index(index, checker, instances)
    source_state = git_state()
    run_dir = make_version(args.results.resolve(), args.solver_name)
    metadata = {"solver": args.solver_name, "solver_exe": str(solver),
                "manifest": str(manifest), "jobs": args.jobs,
                "solver_seeds": list(range(1, 11)), "max_requests": 100,
                "hard_timeout_seconds": args.time_limit,
                "started_utc": dt.datetime.now(dt.timezone.utc).isoformat(), **source_state}
    (run_dir / "metadata.json").write_text(json.dumps(metadata, indent=2) + "\n", encoding="utf-8")
    print(f"Running {len(cases)} instances x 10 seeds with {args.jobs} jobs; results: {run_dir}", flush=True)
    completed = []
    with ThreadPoolExecutor(max_workers=args.jobs) as pool:
        futures = [pool.submit(run_one, case, seed, solver, checker, run_dir, args.time_limit)
                   for case in cases for seed in range(1, 11)]
        for count, future in enumerate(as_completed(futures), 1):
            completed.append(future.result())
            if count % 10 == 0:
                write_csv(run_dir / "runs.csv", RUN_FIELDS, [item[0] for item in completed])
                print(f"Completed {count}/{len(futures)} runs", flush=True)
    order = {row["file"]: i for i, (row, _) in enumerate(cases)}
    completed.sort(key=lambda item: (order[item[0]["instance"]], item[0]["solver_seed"]))
    run_rows = [item[0] for item in completed]
    write_csv(run_dir / "runs.csv", RUN_FIELDS, run_rows)

    summary = []
    for row, _ in cases:
        subset = [item for item in run_rows if item["instance"] == row["file"]]
        values = [float(item["objective"]) for item in subset if item["status"] == "feasible"]
        summary.append({"instance": row["file"], "best_obj": max(values) if values else "",
                        "mean_obj": statistics.mean(values) if values else "",
                        "mean_time_sec": statistics.mean(float(item["wall_seconds"]) for item in subset),
                        "feasible_runs": len(values),
                        "timeout_runs": sum(item["status"] == "timeout" for item in subset),
                        "total_runs": len(subset)})
    write_csv(run_dir / "summary.csv", list(summary[0]), summary)

    for run, solution in completed:
        if solution is None or float(run["objective"]) < 0:
            continue
        name = run["instance"]
        value = float(run["objective"])
        previous = best.get(name)
        if previous and value <= float(previous["objective"]) + 1e-7:
            continue
        if previous and previous["is_optimal"] == "1":
            raise RuntimeError(f"heuristic exceeded a proven optimum: {name}")
        destination = best_dir / (Path(name).stem + ".sol")
        temporary = destination.with_name(destination.name + ".tmp")
        temporary.write_bytes(solution.read_bytes())
        temporary.replace(destination)
        best[name] = {"instance": name, "objective": f"{value:.17g}", "is_optimal": "0",
                      "solution": destination.name, "solver": args.solver_name,
                      "version": run_dir.name, "seed": run["solver_seed"]}
        write_csv(index, BEST_FIELDS, [best[key] for key in sorted(best)])
    if not index.exists():
        write_csv(index, BEST_FIELDS, [])
    shutil.rmtree(run_dir / "work")
    metadata["finished_utc"] = dt.datetime.now(dt.timezone.utc).isoformat()
    (run_dir / "metadata.json").write_text(json.dumps(metadata, indent=2) + "\n", encoding="utf-8")
    print(f"Completed {len(run_rows)} runs; {sum(row['status'] == 'feasible' for row in run_rows)} feasible", flush=True)


if __name__ == "__main__":
    main()
