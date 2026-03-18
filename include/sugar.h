#pragma once
#include "types.h"
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define vec2_unpack(p) p.x, p.y
#define vec2_sub(a, b)                                                                             \
    (vec2) {                                                                                       \
        a.x - b.x, a.y - b.y                                                                       \
    }
#define vec2_add(a, b)                                                                             \
    (vec2) {                                                                                       \
        a.x + b.x, a.y + b.y                                                                       \
    }
#define vec2_mul(v, s)                                                                             \
    (vec2) {                                                                                       \
        v.x *s, v.y *s                                                                             \
    }
#define vec2_div(v, s)                                                                             \
    (vec2) {                                                                                       \
        v.x / s, v.y / s                                                                           \
    }

// integer
#define vec2_idiv(v, s)                                                                            \
    (vec2) {                                                                                       \
        (i64) v.x / (i64)s, (i64)v.y / (i64)s                                                      \
    }
#define vec2_print(v) printf("%s:{%f,%f}\n", #v, v.x, v.y)

#define ARRLEN(arr) (sizeof(arr) / sizeof(arr[0]))

#define LEN(expr) sizeof(expr)  // been using too much python

#define bound(val, lo, hi) (fmin(hi, (fmax(lo, val))))
// TODO: change to type generic version

static inline vec2 screen_to_grid(vec2 s_pos) {
    return vec2_idiv(s_pos, BLOCK_SZ);
}
static inline u64 urand(u64 min, u64 max) {
    u64 range = max - min;
    assert(range >= 0 || "Bro");
    return (min + (rand() % range));
}
