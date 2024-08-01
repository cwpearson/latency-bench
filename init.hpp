#pragma once

#include <functional>

void init();

void on_init(std::function<void()>, const char *name);

struct OnInitFn {
    OnInitFn(std::function<void()> fn, const char *name) {
        on_init(fn, name);
    }
};
#define LB_CONCAT(a, b) LB_CONCAT2(a, b)
#define LB_CONCAT2(a, b) a##b
#define ON_INIT_FN_NAME(x) LB_CONCAT(_on_init_, __LINE__)
#define LB_ON_INIT(x, name)                                              \
  static OnInitFn ON_INIT_FN_NAME(x) =                            \
      OnInitFn(x, name);
