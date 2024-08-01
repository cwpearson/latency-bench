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
FetchContent_Declare(
  numactl
  GIT_REPOSITORY https://github.com/numactl/numactl.git
  GIT_TAG        v2.0.18
)
FetchContent_MakeAvailable(benchmark numactl)

FetchContent_GetProperties(numactl)

add_custom_command(
  OUTPUT numactl_autogen_success
  WORKING_DIRECTORY ${numactl_SOURCE_DIR}
  COMMAND ./autogen.sh
  COMMAND touch ${CMAKE_CURRENT_BINARY_DIR}/numactl_autogen_success
)
add_custom_target(numactl_autogen
  DEPENDS numactl_autogen_success
)

add_custom_command(
  OUTPUT numactl_configure_success
  WORKING_DIRECTORY ${numactl_BINARY_DIR}
  COMMAND ${numactl_SOURCE_DIR}/configure --prefix=${numactl_BINARY_DIR}
  COMMAND touch ${CMAKE_CURRENT_BINARY_DIR}/numactl_configure_success
)
add_custom_target(numactl_configure
  DEPENDS numactl_configure_success
)
add_dependencies(numactl_configure numactl_autogen)

add_custom_command(
  OUTPUT numactl_make_success
  WORKING_DIRECTORY ${numactl_BINARY_DIR}
  COMMAND make CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} install
  COMMAND touch ${CMAKE_CURRENT_BINARY_DIR}/numactl_make_success
)
add_custom_target(numactl_make
  DEPENDS numactl_make_success
)
add_dependencies(numactl_make numactl_configure)

add_library(numactl::numa SHARED IMPORTED)
set_target_properties(numactl::numa PROPERTIES
  IMPORTED_LOCATION ${numactl_BINARY_DIR}/lib/libnuma.a
  INCLUDE_DIRECTORIES ${numactl_BINARY_DIR}/include
)
add_dependencies(numactl::numa numactl_make)


if(NOT LB_CACHE_LINESIZE)
  execute_process(
    COMMAND getconf LEVEL1_DCACHE_LINESIZE
    RESULT_VARIABLE CODE
    OUTPUT_VARIABLE LB_CACHE_LINESIZE
  )
  if (CODE EQUAL 0)
    message(STATUS "Autodetected cache size as ${LB_CACHE_LINESIZE}")
  else()
    message(FATAL "Faield to autodetect cache line size. Use -DLB_CACHE_LINESIZE to set it")
  endif()
endif()




add_executable(main main.cpp
  init.cpp
  numa.cpp
  core_cas.cpp
  core_readwrite.cpp
  mem_latency.cpp
)
set_target_properties(main PROPERTIES CXX_STANDARD 11)
set_target_properties(main PROPERTIES CXX_STANDARD_REQUIRED TRUE)
target_link_libraries(main benchmark::benchmark)
target_link_libraries(main numactl::numa)
target_compile_definitions(main PRIVATE LB_CACHE_LINESIZE=${LB_CACHE_LINESIZE})
