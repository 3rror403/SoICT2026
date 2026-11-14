# Second submission

Upload **`submission.cpp`**. It is the readable, standalone C++17 implementation of v010 plus the memory corrections and the validated late parcel-pair supplement (v002). `submission.readable.cpp` is an identical review copy; `submission.meta.json` records their hashes and provenance.

The source includes the embedded HSOL implementation and its MIT licence. It requires no CPLEX, external solver, input-specific route bank or answer file. Research counters, environment-controlled logging and bank exports have been removed. Default seed is1; an optional numeric CLI argument supports local seed checks. Standard output contains only the objective and one route per taxi. The299-second internal budget starts before input parsing.

Compile the exact upload:

```sh
g++ -std=c++17 -O2 -DNDEBUG -pedantic-errors submission/submission.cpp -o /tmp/soict-submission
/tmp/soict-submission < data/probation/r100_p50_m50_s4.txt > /tmp/r100.sol
./cmake-build-debug/SoICT2026 check-solution data/probation/r100_p50_m50_s4.txt /tmp/r100.sol
```

The original names and structured source are retained; there is no identifier shortening, macro compression or minification. The current local uncompressed packaging convention is500,000 characters. Historical50,000-character compressed-packager instructions describe an older artifact.

The authoritative source for this version is `submission.cpp`. `scripts/pack_submission.py` and `scripts/pack_uncompressed_submission.py` assemble an older modular solver and do not reproduce this promoted standalone algorithm. The pre-existing `submission.uncompressed*` files are preserved development snapshots, not the second-submission upload.

Promotion and validation records, including backups of every previous submission file, are under `results/submission/second_trial_20261114/`. The measured working gates are <300 seconds wall/CPU time and <=600,000,000 bytes peak RSS per solver process. Local checks do not imply that an upload has occurred.

## Validated second-submission snapshot

Source SHA256: `a8456d88199e26d66efe84a45fe2b7e37a9973fccd733bc68ecfb44925d72011` (362,522 characters). Strict C++17 builds under GCC9.5 and GCC11.4 pass. All18 requested probation/synthetic runs, plus one full parcel-feature regression, pass both checkers and the resource gates. Sixteen smoke checks and eight AddressSanitizer/UndefinedBehaviorSanitizer cases pass. See [the exact-upload validation report](../results/submission/second_trial_20261114/report.md). This file has not been uploaded automatically.
