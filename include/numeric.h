#pragma once
#include "types.h"
#include <assert.h>
#include <stdlib.h>

static inline u64 urand(u64 min, u64 max) {
    u64 range = max - min;
    assert(range >= 0 || "Bro");
    return (min + (rand() % range));
}
static inline bool min(i64 a, i64 b) {
    return a <= b ? a : b;
}
static inline bool max(i64 a, i64 b) {
    return a >= b ? a : b;
}
