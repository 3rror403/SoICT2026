# Single-file contest submission

Generate the current default route-generation solver as one standalone C++17 file:

```sh
python3 scripts/pack_submission.py --time-limit 300 --seed 1
```

Upload `submission/submission.cpp`. It contains the current core, route generation, native route-pool selection, and local-search polishing, recursively inlines project headers, and needs only the C++ standard library. The unused ALNS fallback and optional CPLEX backend are excluded. The generator follows the `soict_core` and `soict_alns` source lists in `CMakeLists.txt`, omitting unused fallback sources, and records source SHA-256 hashes and the identifier name map in `submission/submission.meta.json`. Regenerate after changing solver sources or headers.

With no arguments, the generated executable reads the official probation instance from standard input and writes the benefit and exactly one route line per taxi to standard output. It uses solver seed 1 and the current default settings. The five-minute wall budget starts before input parsing; remaining search time is reduced by parsing time and a one-second output reserve. The solver can finish earlier. All diagnostic output is discarded. The submission assumes valid official input and omits exception handlers, assertion guards, bounds-checked vector access, and standalone input/solution validation. The modular executable retains its validation and diagnostics.

Compile and check locally:

```sh
g++ -std=c++17 -O2 -DNDEBUG submission/submission.cpp -o /tmp/soict-submission
/tmp/soict-submission < data/probation/r4_p2_m2_s1.txt > /tmp/sample.sol
./cmake-build-debug/SoICT2026 check-solution data/probation/r4_p2_m2_s1.txt /tmp/sample.sol
```

The default package is limited to 50,000 characters for CMS. The packager removes comments and unnecessary whitespace, deduplicates standard includes, and shortens identifiers while preserving token boundaries and string literals. When needed, it compresses repeated token sequences into macros and verifies that expansion reproduces the original tokens. Remaining one-letter identifiers are assigned to frequent generated aliases. Route generation, route-pool selection, and final full-neighborhood polishing remain included. The feasibility tests used to select valid routes and moves remain intact. Objective recomputation preserves cost subtraction and revenue addition order for valid solutions. The package omits the development CLI, synthetic generator, solution-file reader, instance writer, and development logging. With the default path generation and candidate sampling enabled, both complete and incomplete route banks select the route-generation path; the omitted ALNS fallback is never reached. The packager checks those defaults before generating the submission. The modular project sources retain their original names and behavior. The packaged executable runs the judge entry point directly and accepts no CLI options.

For a readable version of the same judge package, use:

```sh
python3 scripts/pack_submission.py --readable-out submission/submission.readable.cpp
```

Submission pruning also removes ALNS-only insertion caches, unindexed insertion
fallbacks, reference-only replacement/swap scans, route-bank CSV export, debug
state checking, unused state swapping, and CPLEX-only pricing. The generated
solver always constructs and carries its empty-taxi index and uses the default
cached local-search implementations. Random, related, and worst removal remain
because sampled fleet generation actively reuses them for perturbation.
Unused route schedule arrays and diagnostic timing/counter bookkeeping are
also omitted from the generated package.

Upload only `submission/submission.cpp`. The readable copy and metadata are review artifacts and exceed the CMS limit. The packager refuses to write a compact artifact above the limit; `--max-characters` can specify another platform cap. Compile the newly generated artifact before uploading, since new standard-library members may need to be protected from identifier shortening.

Use `--out PATH` for another output location, `--time-limit SECONDS` to package another judge budget, and `--seed SEED` to set the no-argument solver seed. The defaults are `submission/submission.cpp`, 300 seconds, and seed 1.

The default is `--optimization O3-unroll`, retained after the matched
[r240 comparison](../alns-speed/pragma-optimization.md).
`--optimization O3` adds `#pragma GCC optimize("O3")` at the beginning of the
judge package. `--optimization O3-unroll` also requests `unroll-loops`.
`--optimization O2` emits no optimization pragma and uses the compiler command's
optimization settings. These options target GCC; the modular project's build
settings are unchanged. The packager reserves space for the pragma before macro
compression and counts it toward the 50,000-character limit. The readable copy
contains the same pragma, and metadata records the selected option.

The generated package adapts the modular code's C++20 dependencies for C++17: span parameters become const vector references, route removal copies its partial request subset into a vector, route-cache bit width uses a zero-safe GCC builtin helper, and relocation filtering uses the erase/remove idiom instead of `std::erase_if`. The modular source remains unchanged. The package also omits the unused filesystem header.

For GCC 9 compatibility, the submission parses floating-point tokens with
`std::strtod` using a null-terminated copy of each bounded token. Integer tokens
continue to use `std::from_chars`. Decimal and exponent notation are supported.
