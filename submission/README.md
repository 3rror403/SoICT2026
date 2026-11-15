# Current submission

The selected artifact is `submission.cpp`: V5 with the conservative P2 parcel-bundle neighborhood. It remains a standalone C++17 solver with embedded HSOL, route-bank selection, priced route generation, and parcel local search. `submission.readable.cpp` is the historical V5 artifact and is no longer byte-identical.

The annotated selected source was compiled with GCC 11.4 on WSL Ubuntu 22.04:

```bash
g++ -std=c++17 -O2 -DNDEBUG -pedantic-errors submission/submission.cpp -o submission_solver
```

The source is 391,494 bytes. It preserves V5's 298-second internal search deadline and 450,000,000-byte allocation guard. The guard is not measured peak RSS or a verified server limit. The new three-input comparison measured approximately 5.38–136.76 MiB peak RSS for the hybrid on WSL; native Windows compilation is unverified.

The parcel local search performs complete-route relocation and swap validation on directed/nonmetric instances. Request-price estimation is deadline-aware and retains only fully completed price vectors when its caller deadline expires.

The bundle move shares a 50-ms deadline, validates complete routes, and charges displaced-route profit before replacing an occupied taxi. Rejected trials do not enter the route bank.

See [the version history and experimental report](../docs/research/v5-parcel-bundles/README.md) for V1–V5 methods, local alternatives, rejected directions, exact-oracle checks, and full-driver results. The measured result is one certified improvement (0 to 1,999.801) and two ordinary-input ties; it does not establish a general hidden-test advantage. Source hashes and validation provenance are in `submission.meta.json` and the report's `results.json`.
