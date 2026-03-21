#pragma once
#include "types.h"
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define ARRLEN(arr) (sizeof(arr) / sizeof(arr[0]))

#define LEN(expr) sizeof(expr)  // been using too much python

#define bound(val, lo, hi) (fmin(hi, (fmax(lo, val))))

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
