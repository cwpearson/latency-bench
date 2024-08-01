# latency-bench


## Venado

```bash
module load gcc-native
cmake -S . -B build -DCMAKE_C_COMPILER=gcc-12 -DCMAKE_CXX_COMPILER=g++-12
make -C build
```