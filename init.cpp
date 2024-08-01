#include "init.hpp"

#include <vector>
#include <iostream>
#include <functional>

#include <numa.h>

std::vector<std::function<void()>> on_inits;

void init() {

    numa_set_strict(1);

    for (const auto &fn : on_inits) {
        std::cerr << "calling init fn...\n";
        fn();
    }
}

void on_init(std::function<void()> fn, const char *name) {
    on_inits.push_back(fn);
}