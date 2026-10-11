#!/usr/bin/env bash
set -euo pipefail

# Usage: generate_synthetic_data.sh [GENERATOR [OUTPUT_DIR [PROFILE [SUITE]]]]
# large: three balanced passenger/parcel instances per size, seeds 1..3.

repo_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
generator="${1:-$repo_dir/cmake-build-debug/SoICT2026}"
output_dir="${2:-$repo_dir/data/synthetic}"
profile="${3:-empirical}"
suite="${4:-standard}"
case "$suite" in
    standard) sizes=(10 20 50 100 250 500) ;;
    large) sizes=(500 1000 2000) ;;
    *) echo "Unknown suite: $suite (expected standard or large)" >&2; exit 1 ;;
esac
case "$profile" in
    empirical) generation=probation-calibrated-v3 ;;
    directed-noise) generation=probation-calibrated-v3-directed-noise ;;
    *) echo "Unknown profile: $profile" >&2; exit 1 ;;
esac
if [[ ! -x "$generator" ]]; then
    echo "Build the generator first: cmake --build cmake-build-debug --target SoICT2026" >&2
    exit 1
fi
mkdir -p "$output_dir"
manifest="$output_dir/manifest.csv"
printf 'file,requests,passengers,parcels,taxis,seed,generation,sha256\n' > "$manifest"

# Linear interpolation through observed (requests,taxis), extrapolating the last segment.
for requests in "${sizes[@]}"; do
    taxis=$(python3 - "$requests" <<'PY'
import math, sys
r = int(sys.argv[1])
points = [(4,2), (10,3), (30,5), (100,10), (240,20)]
a,b = points[-2:]
for left,right in zip(points,points[1:]):
    if r <= right[0]:
        a,b = left,right
        break
print(max(1, math.floor(a[1]+(r-a[0])*(b[1]-a[1])/(b[0]-a[0])+0.5)))
PY
)
    quarter=$((requests / 4))
    half=$((requests / 2))
    if [[ "$suite" == large ]]; then
        passengers=("$half" "$half" "$half")
    else
        passengers=(0 "$quarter" "$half" "$((requests - quarter))" "$requests")
    fi
    for index in "${!passengers[@]}"; do
        seed=$((index + 1))
        n=${passengers[$index]}
        m=$((requests - n))
        name="r${requests}_p${n}_m${m}_s${seed}.txt"
        "$generator" generate --passengers "$n" --parcels "$m" \
            --taxis "$taxis" --seed "$seed" --profile "$profile" --out "$output_dir/$name"
        digest=$(sha256sum "$output_dir/$name")
        digest=${digest%% *}
        printf '%s,%s,%s,%s,%s,%s,%s,%s\n' "$name" "$requests" "$n" "$m" "$taxis" "$seed" "$generation" "$digest" >> "$manifest"
    done
done

python3 - "$repo_dir" "$generator" "$output_dir" "$profile" "$generation" "$suite" <<'PYPROFILE'
import csv
import datetime as dt
import hashlib
import json
from pathlib import Path
import sys
repo, executable, output = map(Path, sys.argv[1:4])
profile_name, generation, suite = sys.argv[4:]
def digest(path):
    hasher = hashlib.sha256()
    with path.open('rb') as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b''):
            hasher.update(block)
    return hasher.hexdigest()
calibration = json.loads((repo/'data/probation/calibration.json').read_text())
for source in calibration['sources']:
    if digest(repo/'data/probation'/source['file']) != source['sha256']:
        raise ValueError(f"Calibration source hash mismatch: {source['file']}")
with (output/'manifest.csv').open() as stream:
    rows = list(csv.DictReader(stream))
sizes = sorted({int(row['requests']) for row in rows})
profile = {
    'generation': generation,
    'profile': profile_name,
    'suite': suite,
    'calibration': calibration,
    'taxi_rule': 'Nearest integer linear interpolation through (4,2),(10,3),(30,5),(100,10),(240,20); extrapolate final segment',
    'requests': sizes,
    'taxi_counts': [next(int(row['taxis']) for row in rows if int(row['requests']) == size) for size in sizes],
    'seeds': sorted({int(row['seed']) for row in rows}),
    'instances_per_size': {str(size): sum(int(row['requests']) == size for row in rows) for size in sizes},
    'passenger_mix': '50% passengers, 50% parcels' if suite == 'large' else '0%, 25%, 50%, 75%, 100% passengers',
    'coordinates': 'Independent U(0,100) in two dimensions; rejection-condition pickups on solo feasibility',
    'travel_time': 'max(1,ceil(Euclidean distance)) off diagonal',
    'travel_cost': 'max(1,ceil(2.48*Euclidean distance)) off diagonal',
    'request_sampling': 'Uniform case then uniform complete request record; capacity sampled the same way',
    'earliest_jitter': [-2,2],
    'width_noise': [0.9,1.1],
    'delivery_earliest': 'pickup_earliest + service + direct_time + sampled_delta',
    'revenue': 'max(1,round(case-balanced least-squares slope*direct_cost + intercept + sampled record residual))',
    'symmetric_matrices': profile_name == 'empirical',
    'triangle_inequality': profile_name == 'empirical',
    'direction_noise': None if profile_name == 'empirical' else 'Independent U(0.9,1.1) per direction, shared by time and cost, then round',
    'solo_feasibility': 'Pickup coordinates conditioned on reachability; parcel capacity and delivery window also respected',
    'integer_values': True,
    'generator_source_sha256': digest(repo/'src/io/SyntheticGenerator.cpp'),
    'calibration_header_sha256': digest(repo/'src/io/SyntheticCalibration.hpp'),
    'script_sha256': digest(repo/'scripts/generate_synthetic_data.sh'),
    'executable_sha256': digest(executable),
    'manifest_sha256': digest(output/'manifest.csv'),
    'created_utc': dt.datetime.now(dt.timezone.utc).isoformat(),
}
(output/'generation.json').write_text(json.dumps(profile,indent=2)+'\n')
PYPROFILE
