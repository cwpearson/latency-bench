#include <algorithm>
#include <random>
#include <atomic>
#include <iostream>
#include <thread>

#include <benchmark/benchmark.h>

#include <numa.h>

#include "init.hpp"
#include "numa.hpp"

constexpr int SEED = 17;

auto BM_MemLatency = [](benchmark::State& state, int cpu, int node) {

  numa::bind_thread_cpu(cpu);

  using data_type = int;

  const size_t sizeKiB = state.range(0);
  const size_t n = sizeKiB * 1024 / sizeof(data_type);
  
  data_type * A = numa::alloc_onnode<data_type>(n, node);
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

  state.counters["cpu"] = cpu;
  state.counters["numa_node"] = node;
  state.counters["latency"] = benchmark::Counter(n * state.iterations(), benchmark::Counter::kIsRate | benchmark::Counter::kInvert);

  numa::free<data_type>(A, n);

  numa::bind_thread_all_cpus();
};

static void registerer() {


  for (int cpu : numa::all_cpus()) {
    for (int node : numa::all_nodes()) {

      std::string name = std::string("BM_MemLatency")
      + "/" + std::to_string(cpu)
      + "/" + std::to_string(node);

      benchmark::RegisterBenchmark(
        name.c_str(),
        BM_MemLatency,
        cpu, node
      )->UseRealTime()->RangeMultiplier(4)->Range(1, 1<<20);
    }
  }

}

LB_ON_INIT(registerer, "BM_MemLatency");

