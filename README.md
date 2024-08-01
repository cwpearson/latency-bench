# latency-bench

* Memory latency microbenchmark
* Core-to-core microbenchmark using CAS on an atomic bool
* Core-to-core microbenchmark using read/write swaps

## Known Recipes

### Venado (August 2024)

```bash
module load gcc-native
cmake -S . -B build -DCMAKE_C_COMPILER=gcc-12 -DCMAKE_CXX_COMPILER=g++-12
make -C build
```

## Roadmap

