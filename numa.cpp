#include "numa.hpp"

#include <iostream>
#include <vector>

#include <numa.h>
#include <unistd.h>

namespace numa {
    std::vector<int> all_cpus() {
        std::vector<int> cpus;
        for (int i = 0; i < numa_num_possible_cpus(); ++i) {
            if (numa_bitmask_isbitset(numa_all_cpus_ptr, i)) {
		    cpus.push_back(i);
            }
        }
        return cpus;
    }


std::vector<int> all_nodes() {
        std::vector<int> nodes;
        for (int i = 0; i < numa_num_possible_nodes(); ++i) {
            if (numa_bitmask_isbitset(numa_all_nodes_ptr, i)) {
		    nodes.push_back(i);
            }
        }
        return nodes;
}

void bind_thread_cpu(int cpu) {
    bitmask *cpu_mask = numa_allocate_cpumask();
    numa_bitmask_setbit(cpu_mask, cpu);
    numa_sched_setaffinity(gettid(), cpu_mask);
    numa_free_cpumask(cpu_mask); cpu_mask = nullptr;
}

void bind_thread_all_cpus() {
  numa_sched_setaffinity(gettid(), numa_all_cpus_ptr);
}

void bind_thread_node(int node) {
    numa_run_on_node(node);
}

void bind_thread_all_nodes() {
  numa_run_on_node(-1);
}

}
