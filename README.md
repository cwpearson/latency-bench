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

### Perlmutter (August 2024)

```bash
export ACCOUNT=<your account>
salloc -C cpu --ntasks=1  -q regular -A $ACCOUNT
cmake -S . -B build
make -C build

build/main --benchmark_filter='Latency/0/0/'


for numa in $(seq 0 7); do

cat > script.slurm << EOF
#!/bin/bash
#SBATCH --qos=regular
#SBATCH -A $ACCOUNT
#SBATCH -C cpu
#SBATCH --ntasks 1
#SBATCH -J "latency_$numa"
#SBATCH -o "latency_${numa}_%A.txt"
#SBATCH -t 20

build/main --benchmark_filter=\"Latency/0/$numa/\" --benchmark_format=csv
EOF

  sbatch script.slurm
  rm script.slurm
done

watch -n5 sqs

```

## Roadmap

- [x] symlink or copy numactl into build
