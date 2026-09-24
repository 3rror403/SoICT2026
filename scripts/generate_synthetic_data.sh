#!/usr/bin/env bash
set -euo pipefail

repo_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
generator="$repo_dir/cmake-build-debug/SoICT2026"
output_dir="$repo_dir/data/synthetic"

if [[ ! -x "$generator" ]]; then
    echo "Build the generator first: cmake --build cmake-build-debug" >&2
    exit 1
fi

mkdir -p "$output_dir"
manifest="$output_dir/manifest.csv"
printf 'file,requests,passengers,parcels,taxis,seed\n' > "$manifest"

for requests in 10 20 50 100 250 500; do
    quarter=$((requests / 4))
    half=$((requests / 2))
    passengers=(0 "$quarter" "$half" "$((requests - quarter))" "$requests")
    for index in "${!passengers[@]}"; do
        seed=$((index + 1))
        n=${passengers[$index]}
        m=$((requests - n))
        name="r${requests}_p${n}_m${m}_s${seed}.txt"
        "$generator" generate --passengers "$n" --parcels "$m" \
            --taxis 100 --seed "$seed" --out "$output_dir/$name"
        printf '%s,%s,%s,%s,100,%s\n' "$name" "$requests" "$n" "$m" "$seed" >> "$manifest"
    done
done
