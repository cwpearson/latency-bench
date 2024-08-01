#include <atomic>
#include <iostream>

#include <benchmark/benchmark.h>

#include "init.hpp"
#include "numa.hpp"

#define LINE_SIZE 128

struct CachePadded{
    std::atomic<bool> b;
    char pad[LINE_SIZE -sizeof(std::atomic<bool>)];
};

// shared between threads
CachePadded v1;
CachePadded v2;

auto BM_CoreReadWrite = [](benchmark::State& state, int cpu0, int cpu1) {

  if (state.thread_index() == 0) {
    numa::bind_thread_cpu(cpu0);
  } else {
    numa::bind_thread_cpu(cpu1);
  }

  // t0 reads from v1 and writes to v2
  // t1 reads from v2 and writes from v1

  // reset state
  if (state.thread_index() == 0) {
    v1.b.store(false, std::memory_order_release);
  } else {
    v2.b.store(false, std::memory_order_release);
  }

  // t0 has false, t1 has true
  bool b = state.thread_index() == 1;

  for (auto _ : state) {

    if (state.thread_index() == 0) {
        while (b != v1.b.load(std::memory_order_acquire)) {}
        v2.b.store(!b, std::memory_order_release);
        b = !b;
    } else {
        while (b != v2.b.load(std::memory_order_acquire)) {}
        v1.b.store(b, std::memory_order_release);
        b = !b;
    }
  }

  state.counters["latency"] = benchmark::Counter(state.iterations(), benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
  
  numa::bind_thread_all_cpus();
};

static void registerer() {

  std::vector<int> cpus = numa::all_cpus();

  // if we pin both threads to the same core, it just appears to be very slow
  // because they can't execute at the same time
  for (size_t i = 0; i < cpus.size(); ++i) {
    for (size_t j = i+1; j < cpus.size(); ++j) {
      int cpu0 = cpus[i];
      int cpu1 = cpus[j];

      std::string name = std::string("BM_CoreReadWrite")
      + "/" + std::to_string(cpu0)
      + "/" + std::to_string(cpu1);

      benchmark::RegisterBenchmark(
        name.c_str(),
        BM_CoreReadWrite,
        cpu0, cpu1
      )->Threads(2)->UseRealTime();
    }
  }

}

LB_ON_INIT(registerer, "BM_CoreReadWrite");
