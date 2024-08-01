#pragma once

#include <vector>

#include <numa.h>

namespace numa {
    std::vector<int> all_cpus();
    void bind_thread_cpu(int cpu);
    void bind_thread_all_cpus();
    std::vector<int> all_nodes();
    void bind_thread_node(int node);
    void bind_thread_all_nodes();

    template <typename T>
    T *alloc_onnode(std::size_t n, int node) {
        return reinterpret_cast<T*>(numa_alloc_onnode(sizeof(T) * n, node));
    }
    template <typename T>
    void free(T* start, std::size_t n) {
        numa_free(start, sizeof(T) * n);
    }
}
