# Current submission

Upload `submission.cpp`. It is a standalone readable C++17 solver with embedded HSOL, route-bank selection, priced route generation, and parcel local search inside fleet reconstruction. `submission.readable.cpp` is byte-identical.

Build check used GCC 9.5 with:

```bash
g++ -std=c++17 -O2 -DNDEBUG -pedantic-errors submission/submission.cpp -o submission_solver
```

The source is 385,044 bytes. The solver uses a 298-second internal search deadline under a 299-second wall budget. The embedded LP backend has an estimated 450 MB workspace guard; final smoke measurements were below the observed 500 MB memory-failure ceiling.

The parcel local search performs complete-route relocation and swap validation on directed/nonmetric instances. Request-price estimation is deadline-aware and retains only fully completed price vectors when its caller deadline expires.

Preparation evidence is recorded in `results/submission/third_preparation_20261115/final_manifest.json`. No upload is performed automatically.
