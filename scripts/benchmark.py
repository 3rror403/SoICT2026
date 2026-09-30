#!/usr/bin/env python3
"""
Phase 0a — Benchmark script nhiều seed.

Usage:
    python scripts/benchmark.py [OPTIONS]

Options:
    --exe PATH          Path to SoICT2026 binary
                        (default: cmake-build-release/SoICT2026)
    --instances GLOB    Glob pattern for instance files
                        (default: data/synthetic/r10_*.txt,r20_*.txt,r50_*.txt,r100_*.txt)
    --seeds N           Number of seeds to run (default: 10)
    --time-limit SEC    Time limit per run in seconds (default: 60)
    --iterations N      If set, use deterministic --iterations N mode instead
    --bks FILE          Path to best-known CSV (default: data/synthetic/best-known/best-known.csv)
    --out CSV           Output CSV path (default: benchmark_results.csv)
    --jobs N            Parallel jobs (default: number of physical cores)
    --tag NAME          Variant label for output (default: baseline)

Examples:
    # Quick screen: r10 + r20, 5 seeds, 20s
    python scripts/benchmark.py --instances "data/synthetic/r10_*.txt" --seeds 5 --time-limit 20

    # Deterministic regression: confirm patch does not change objective
    python scripts/benchmark.py --iterations 50000 --seeds 1 --tag baseline
    python scripts/benchmark.py --iterations 50000 --seeds 1 --tag patched --exe cmake-build-release/SoICT2026_patched

    # Full benchmark: 10 seeds, 60s
    python scripts/benchmark.py --tag v4_baseline
"""

import argparse
import csv
import glob
import math
import os
import re
import subprocess
import sys
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path

SCRIPT_DIR = Path(__file__).parent
ROOT_DIR = SCRIPT_DIR.parent


def parse_args():
    p = argparse.ArgumentParser(description="SoICT2026 ALNS benchmark runner")
    p.add_argument("--exe", default=str(ROOT_DIR / "cmake-build-release" / "SoICT2026"))
    p.add_argument("--instances", default="data/synthetic/r10_*.txt,data/synthetic/r20_*.txt,"
                                          "data/synthetic/r50_*.txt,data/synthetic/r100_*.txt")
    p.add_argument("--seeds", type=int, default=10)
    p.add_argument("--time-limit", type=float, default=60.0)
    p.add_argument("--iterations", type=int, default=None,
                   help="Deterministic mode: run exactly N iterations")
    p.add_argument("--bks", default=str(ROOT_DIR / "data" / "synthetic" / "best-known" / "best-known.csv"))
    p.add_argument("--out", default=str(ROOT_DIR / "benchmark_results.csv"))
    p.add_argument("--jobs", type=int, default=None)
    p.add_argument("--tag", default="baseline")
    return p.parse_args()


def load_bks(path):
    """Returns dict: instance_name -> best_known_benefit (float)."""
    bks = {}
    try:
        with open(path) as f:
            reader = csv.DictReader(f)
            for row in reader:
                # CSV columns: instance, benefit  (or instance, best_benefit)
                key = row.get("instance", "").strip()
                val = row.get("benefit") or row.get("best_benefit") or row.get("best_known")
                if key and val:
                    bks[key] = float(val)
    except FileNotFoundError:
        print(f"[warn] BKS file not found: {path}", file=sys.stderr)
    return bks


def collect_instances(patterns):
    files = []
    for pattern in patterns.split(","):
        pattern = pattern.strip()
        if not os.path.isabs(pattern):
            pattern = str(ROOT_DIR / pattern)
        files.extend(sorted(glob.glob(pattern)))
    return list(dict.fromkeys(files))  # deduplicate, preserve order


def run_one(exe, instance_path, seed, time_limit, max_iterations, sol_dir):
    """Run one ALNS instance and return parsed result dict."""
    instance_name = Path(instance_path).stem
    sol_path = Path(sol_dir) / f"{instance_name}_s{seed}.sol"
    cmd = [exe, "solve-alns",
           "--instance", instance_path,
           "--seed", str(seed),
           "--out", str(sol_path)]
    if max_iterations is not None:
        cmd += ["--iterations", str(max_iterations)]
    else:
        cmd += ["--time-limit", str(time_limit)]

    t0 = time.perf_counter()
    try:
        proc = subprocess.run(cmd, capture_output=True, text=True,
                              timeout=time_limit + 30)
        wall = time.perf_counter() - t0
        stdout = proc.stdout.strip()
        # Parse: benefit=X iterations=Y seconds=Z stop=W
        m = re.search(r"benefit=([0-9.e+\-]+)", stdout)
        benefit = float(m.group(1)) if m else float("nan")
        m2 = re.search(r"iterations=(\d+)", stdout)
        iterations = int(m2.group(1)) if m2 else -1
        return {
            "instance": instance_name,
            "seed": seed,
            "benefit": benefit,
            "iterations": iterations,
            "wall_sec": wall,
            "returncode": proc.returncode,
        }
    except subprocess.TimeoutExpired:
        return {"instance": Path(instance_path).stem, "seed": seed,
                "benefit": float("nan"), "iterations": -1,
                "wall_sec": time_limit + 30, "returncode": -9}
    except Exception as e:
        return {"instance": Path(instance_path).stem, "seed": seed,
                "benefit": float("nan"), "iterations": -1,
                "wall_sec": 0, "returncode": -1, "error": str(e)}


def summarize(rows, bks):
    """Compute per-instance mean ± std benefit and gap vs BKS."""
    from collections import defaultdict
    by_inst = defaultdict(list)
    for r in rows:
        if not math.isnan(r["benefit"]):
            by_inst[r["instance"]].append(r["benefit"])

    summary = []
    for inst, benefits in sorted(by_inst.items()):
        n = len(benefits)
        mean = sum(benefits) / n
        std = math.sqrt(sum((b - mean) ** 2 for b in benefits) / max(n - 1, 1))
        stderr = std / math.sqrt(n)
        best_bks = bks.get(inst, float("nan"))
        gap = (best_bks - mean) / best_bks if not math.isnan(best_bks) and best_bks > 0 else float("nan")
        summary.append({
            "instance": inst,
            "n": n,
            "mean_benefit": mean,
            "std": std,
            "stderr": stderr,
            "gap_vs_bks": gap,
            "bks": best_bks,
            "min": min(benefits),
            "max": max(benefits),
        })
    return summary


def main():
    args = parse_args()
    instances = collect_instances(args.instances)
    if not instances:
        print(f"[error] No instances matched: {args.instances}", file=sys.stderr)
        sys.exit(1)
    bks = load_bks(args.bks)

    sol_dir = ROOT_DIR / "benchmark_sols"
    sol_dir.mkdir(exist_ok=True)

    seeds = list(range(1, args.seeds + 1))
    tasks = [(inst, s) for inst in instances for s in seeds]
    n_jobs = args.jobs or max(1, os.cpu_count() or 1)

    print(f"[benchmark] tag={args.tag} instances={len(instances)} seeds={args.seeds} "
          f"jobs={n_jobs} mode={'iter=' + str(args.iterations) if args.iterations else 'time=' + str(args.time_limit) + 's'}")

    raw_rows = []
    t_start = time.perf_counter()

    with ThreadPoolExecutor(max_workers=n_jobs) as pool:
        futures = {
            pool.submit(run_one, args.exe, inst, seed, args.time_limit,
                        args.iterations, sol_dir): (inst, seed)
            for inst, seed in tasks
        }
        done = 0
        for fut in as_completed(futures):
            r = fut.result()
            r["tag"] = args.tag
            raw_rows.append(r)
            done += 1
            inst_name = Path(futures[fut][0]).stem
            status = "OK" if r["returncode"] == 0 else f"ERR({r['returncode']})"
            print(f"  [{done:3d}/{len(tasks)}] {inst_name} seed={r['seed']:2d} "
                  f"benefit={r['benefit']:.4f} {status}")

    wall_total = time.perf_counter() - t_start
    print(f"\n[benchmark] Completed in {wall_total:.1f}s")

    # Write raw results
    raw_path = Path(args.out).with_suffix("") / "raw.csv" if Path(args.out).suffix == "" else Path(args.out)
    raw_path.parent.mkdir(exist_ok=True)
    with open(raw_path, "w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=["tag", "instance", "seed", "benefit",
                                               "iterations", "wall_sec", "returncode"])
        writer.writeheader()
        for r in raw_rows:
            writer.writerow({k: r.get(k, "") for k in writer.fieldnames})

    # Write summary
    summary = summarize(raw_rows, bks)
    summary_path = str(raw_path).replace(".csv", "_summary.csv")
    with open(summary_path, "w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=["instance", "n", "mean_benefit", "std",
                                               "stderr", "gap_vs_bks", "bks", "min", "max"])
        writer.writeheader()
        writer.writerows(summary)

    # Print summary table
    print(f"\n{'Instance':<28} {'N':>3} {'Mean':>12} {'Std':>8} {'SE':>8} {'Gap%':>7} {'BKS':>12}")
    print("-" * 82)
    for row in summary:
        gap_str = f"{row['gap_vs_bks']*100:+.3f}%" if not math.isnan(row["gap_vs_bks"]) else "  N/A "
        bks_str = f"{row['bks']:.4f}" if not math.isnan(row["bks"]) else "    N/A"
        print(f"{row['instance']:<28} {row['n']:>3} {row['mean_benefit']:>12.4f} "
              f"{row['std']:>8.4f} {row['stderr']:>8.4f} {gap_str:>7} {bks_str:>12}")

    print(f"\nRaw:     {raw_path}")
    print(f"Summary: {summary_path}")


if __name__ == "__main__":
    main()
