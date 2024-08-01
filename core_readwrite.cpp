#include <atomic>

#include <benchmark/benchmark.h>

#define LINE_SIZE 128

struct CachePadded{
    std::atomic<bool> b;
    char pad[LINE_SIZE -sizeof(std::atomic<bool>)];
};

// shared between threads
CachePadded v1;
CachePadded v2;

static void BM_CoreReadWrite(benchmark::State& state) {

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

//   std::cerr << state.thread_index() << " " << &v1.b << " " << &v2.b << "\n";

  for (auto _ : state) {

    if (state.thread_index() == 0) {
        // std::cerr << "0: wait for v1 to be " << b << "\n";
        while (b != v1.b.load(std::memory_order_acquire)) {}
        // std::cerr << "0: " << !b << " -> v2\n";
        v2.b.store(!b, std::memory_order_release);
        b = !b;
    } else {
        // std::cerr << "1: wait for v2 to be " << b << "\n";
        while (b != v2.b.load(std::memory_order_acquire)) {}
        // std::cerr << "1: " << b << " -> v1\n";
        v1.b.store(b, std::memory_order_release);
        b = !b;
    }
  }

  state.counters["latency"] = benchmark::Counter(state.iterations(), benchmark::Counter::kIsRate | benchmark::Counter::kInvert);
}

BENCHMARK(BM_CoreReadWrite)->Threads(2)->UseRealTime();