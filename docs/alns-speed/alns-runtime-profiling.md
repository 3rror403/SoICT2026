# Runtime profiling preference

For SoICT2026 runtime profiling and speed diagnostics, use one instance and one
seed by default: r240, seed 1 when applicable. Do not automatically expand to
multiple seeds. The user explicitly requested this on 2026-10-05.

Record the source snapshot, compiler flags and workload. Check the final
solution independently, and compare search behavior for changes intended to
preserve it. This seed-count preference concerns speed diagnostics; it does not
establish search-quality robustness across seeds.
