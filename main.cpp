#include <iostream>

#include <benchmark/benchmark.h>

#include "init.hpp"

int main(int argc, char** argv) {
  benchmark::Initialize(&argc, argv);
  init();
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
}
