#include "init.hpp"

#include <vector>
#include <iostream>
#include <functional>

#include <numa.h>

std::vector<std::function<void()>> on_inits;

void init() {

    numa_set_strict(1);

    std::cerr << __FILE__ << ":" << __LINE__ << " calling " << on_inits.size() << " benchmark registerers...\n";
    for (const auto &fn : on_inits) {
        fn();
    }
    std::cerr << __FILE__ << ":" << __LINE__ << " done\n";
}

void on_init(std::function<void()> fn, const char *name) {
    on_inits.push_back(fn);
}
