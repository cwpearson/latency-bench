#include <algorithm>
#include <random>
#include <atomic>
#include <iostream>
#include <thread>

#include <benchmark/benchmark.h>

constexpr int SEED = 17;

static void BM_MemLatency(benchmark::State& state) {
  // Perform setup here

  using data_type = int;

  const size_t sizeKiB = state.range(0);
  const size_t n = sizeKiB * 1024 / sizeof(data_type);


  data_type * A = new data_type[n];
  for (size_t i = 0; i < n; ++i) {
    A[i] = i;
  }
  std::mt19937 g(SEED);
  std::shuffle(A, A+n, g);

  for (auto _ : state) {
    uint32_t sum = 0;
    uint32_t current = A[0];
    for (uint32_t i = 0 ; i < n; ++i) {
        current = A[current];
        benchmark::DoNotOptimize(sum += current);
    }
  }

  state.counters["latency"] = benchmark::Counter(n * state.iterations(), benchmark::Counter::kIsRate | benchmark::Counter::kInvert);

  delete[] A;
}

// BENCHMARK(BM_MemLatency)->UseRealTime()->RangeMultiplier(2)->Range(1, 1<<20);
