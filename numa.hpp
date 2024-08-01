#pragma once

#include <vector>

namespace numa {
    std::vector<int> all_cpus();
    void bind_thread_cpu(int cpu);
    void bind_thread_all_cpus();
}
