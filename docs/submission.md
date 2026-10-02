# Single-file contest submission

Generate the current ALNS solver as one standalone C++17 file:

```sh
python3 scripts/pack_submission.py --time-limit 300 --seed 1
```

Upload `submission/submission.cpp`. It contains the current core and ALNS implementation, recursively inlines project headers, and needs only the C++ standard library. The optional CPLEX solver is excluded. The generator follows the `soict_core` and `soict_alns` source lists in `CMakeLists.txt` and records source SHA-256 hashes and the identifier name map in `submission/submission.meta.json`. Regenerate after changing solver sources or headers.

With no arguments, the generated executable reads the official probation instance from standard input and writes the benefit and exactly one route line per taxi to standard output. It uses solver seed 1 and the current ALNS settings. The five-minute wall budget starts before input parsing; remaining search time is reduced by parsing time and a one-second output reserve. Annealing can finish earlier. Diagnostics go to standard error.

Compile and check locally:

```sh
g++ -std=c++17 -O2 -DNDEBUG submission/submission.cpp -o /tmp/soict-submission
/tmp/soict-submission < data/probation/r4_p2_m2_s1.txt > /tmp/sample.sol
./cmake-build-debug/SoICT2026 check-solution data/probation/r4_p2_m2_s1.txt /tmp/sample.sol
```

The default package is limited to 50,000 characters for CMS. The packager removes comments and unnecessary whitespace, deduplicates standard includes, and shortens identifiers while preserving token boundaries and string literals. It omits the development CLI, synthetic generator, solution-file reader, instance writer, and development logging. The modular project sources retain their original names and behavior. The packaged executable runs the judge entry point directly and accepts no CLI options.

For a readable version of the same judge package, use:

```sh
python3 scripts/pack_submission.py --readable-out submission/submission.readable.cpp
```

Upload only `submission/submission.cpp`. The readable copy and metadata are review artifacts and exceed the CMS limit. The packager refuses to write a compact artifact above the limit; `--max-characters` can specify another platform cap. Compile the newly generated artifact before uploading, since new standard-library members may need to be protected from identifier shortening.

Use `--out PATH` for another output location, `--time-limit SECONDS` to package another judge budget, and `--seed SEED` to set the no-argument solver seed. The defaults are `submission/submission.cpp`, 300 seconds, and seed 1.

The generated package adapts the modular code's C++20 dependencies for C++17: repair operators receive a const vector reference instead of a span, route-cache bit width uses a zero-safe GCC builtin helper, and relocation filtering uses the erase/remove idiom instead of `std::erase_if`. The modular source remains unchanged. The package also omits the unused filesystem header.
