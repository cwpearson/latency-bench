#include <atomic>

#include <benchmark/benchmark.h>


std::atomic<bool> flag;

static void BM_CoreCas(benchmark::State& state) {

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

  state.counters["latency"] = benchmark::Counter(state.iterations(), benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
}


BENCHMARK(BM_CoreCas)->Threads(2)->UseRealTime();