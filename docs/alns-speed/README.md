# ALNS speed

Runtime profiling, insertion optimizations, and speed experiments for ALNS.

- [n500 route master](n500-route-master.md): wider generation, native LP bounds, basis reuse, and checked n500/r240 results.
- [Direct route generation](direct-path-generation.md): generation-first design, ALNS fallback, and historical matched r240 evidence.
- [Short annealing and route-pool repair](short-annealing-route-pool.md): the preceding 6,000-step optimization and its matched r240 evidence.
- [Speed campaign](alns-speed-campaign.md): retained optimizations, granular neighborhood trials, runtime distribution, and validation.
- [GCC optimization pragmas](pragma-optimization.md): C++17 judge-package comparison of O2, O3, and loop unrolling.
- [AVX2 and selective unrolling](pragma-followup.md): follow-up trials of a CPU target and explicit insertion-loop factors.
- [Runtime profiling preference](alns-runtime-profiling.md): default instance and seed, comparison settings, and checks.
- [Insertion scratch reuse](alns-insertion-scratch.md): reusable buffers and insertion search profiling.
- [AddRequest and ProfitInsertion](alns-add-profit-speed.md): insertion data and empty-taxi reuse experiments.
- [Speed priorities 1–3](alns-speed-priorities-1-3.md): fixed-prefix, cache reuse, and cache commit work.
- [Incremental ProfitInsertion design](profit-insertion-incremental-design.md): design and implementation follow-up.
