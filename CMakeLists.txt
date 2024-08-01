cmake_minimum_required(VERSION 3.5)
project(LatencyBench)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

include(FetchContent)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG        v1.8.5
)
FetchContent_MakeAvailable(benchmark)

add_executable(main main.cpp
  core_cas.cpp
  core_readwrite.cpp
  mem_latency.cpp
)
set_target_properties(main PROPERTIES CXX_STANDARD 11)
set_target_properties(main PROPERTIES CXX_STANDARD_REQUIRED TRUE)
target_link_libraries(main benchmark::benchmark)