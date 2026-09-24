#!/usr/bin/env python3
"""Record proven 10-request MIP solutions separately from heuristic evaluation."""

import argparse
from pathlib import Path
import re
import subprocess
import tempfile

from evaluate import BEST_FIELDS, ROOT, command_for, read_best_index, read_cases, write_csv


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--solver", type=Path, default=ROOT / "cmake-build-debug/SoICT2026")
    parser.add_argument("--manifest", type=Path, default=ROOT / "data/synthetic/manifest.csv")
    parser.add_argument("--best-known", type=Path, default=ROOT / "data/synthetic/best-known")
    args = parser.parse_args()
    solver = args.solver.resolve()
    cases, instances = read_cases(args.manifest.resolve())
    best_dir = args.best_known.resolve()
    best_dir.mkdir(parents=True, exist_ok=True)
    index = best_dir / "best-known.csv"
    best = read_best_index(index, solver, instances)
    for row, instance in cases:
        if int(row["requests"]) != 10:
            continue
        with tempfile.TemporaryDirectory() as temporary:
            output = Path(temporary) / "mip.sol"
            solved = subprocess.run(command_for(solver, ["solve-mip", str(instance),
                                                        "--time-limit", "300", "--out", str(output)]),
                                    capture_output=True, text=True, timeout=330)
            header = solved.stdout.splitlines()[0] if solved.stdout else ""
            match = re.search(r"^status=optimal\b.*\bbenefit=([-+0-9.eE]+)", header)
            if solved.returncode != 0 or not match or not output.is_file():
                raise RuntimeError(f"MIP did not prove {row['file']}: {solved.stdout} {solved.stderr}")
            objective = float(match.group(1))
            checked = subprocess.run(command_for(solver, ["check-solution", str(instance), str(output)]),
                                     capture_output=True, text=True, timeout=30)
            if checked.returncode != 0 or not checked.stdout.startswith("benefit="):
                raise RuntimeError(f"invalid MIP route for {row['file']}")
            actual = float(checked.stdout.split("=", 1)[1].strip())
            if abs(actual - objective) > 1e-6 * (1 + abs(objective)):
                raise RuntimeError(f"MIP objective differs from route for {row['file']}")
            previous = best.get(row["file"])
            if previous and float(previous["objective"]) > objective + 1e-6:
                raise RuntimeError(f"existing best-known exceeds MIP optimum: {row['file']}")
            destination = best_dir / (instance.stem + ".sol")
            temporary_path = destination.with_name(destination.name + ".tmp")
            temporary_path.write_bytes(output.read_bytes())
            temporary_path.replace(destination)
            best[row["file"]] = {"instance": row["file"], "objective": f"{objective:.17g}",
                                 "is_optimal": "1", "solution": destination.name,
                                 "solver": "mip", "version": "cplex-exact", "seed": ""}
            write_csv(index, BEST_FIELDS, [best[key] for key in sorted(best)])
            print(f"{row['file']}: optimal {objective:.10f}")


if __name__ == "__main__":
    main()
