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

numactl -C $(seq 0 4 287 | paste -sd ',' -) build/main --benchmark_filter='Cas'
build/main --benchmark_filter='Latency/0/0/'
```

## Roadmap

- [ ] symlink or copy numactl into build
