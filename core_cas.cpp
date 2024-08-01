#include <atomic>
#include <iostream>
#include <thread>

#include <benchmark/benchmark.h>

#include "init.hpp"
#include "numa.hpp"

std::atomic<bool> flag;

auto BM_CoreCas = [](benchmark::State& state, int cpu0, int cpu1) {

  if (state.thread_index() == 0) {
    numa::bind_thread_cpu(cpu0);
  } else {
    numa::bind_thread_cpu(cpu1);
  }


  // reset state
  if (state.thread_index() == 0) {
    flag.store(false, std::memory_order_release);
  }

  for (auto _ : state) {
    if (state.thread_index() == 0) {
        bool exp = false;
        while(!flag.compare_exchange_strong(exp, true, std::memory_order_relaxed,std::memory_order_relaxed)) {}
    } else {
        bool exp = true;
        while(!flag.compare_exchange_strong(exp, false, std::memory_order_relaxed,std::memory_order_relaxed)) {}
    }
  }


  if (state.thread_index() == 0) {
    state.counters["cpu0"] = cpu0;
    state.counters["cpu1"] = cpu1;
    state.counters["latency"] = benchmark::Counter(2*state.iterations(), benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
  }
  numa::bind_thread_all_cpus();
};


static void registerer() {

  std::vector<int> cpus = numa::all_cpus();

  for (size_t i = 0; i < cpus.size(); ++i) {
    for (size_t j = i; j < cpus.size(); ++j) {
      int cpu0 = cpus[i];
      int cpu1 = cpus[j];

      std::string name = std::string("BM_CoreCas")
      + "/" + std::to_string(cpu0)
      + "/" + std::to_string(cpu1);

      benchmark::RegisterBenchmark(
        name.c_str(),
        BM_CoreCas,
        cpu0, cpu1
      )->Threads(2)->UseRealTime();
    }
  }

}

LB_ON_INIT(registerer, "BM_CoreCas");

