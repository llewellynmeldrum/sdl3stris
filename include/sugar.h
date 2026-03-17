#pragma once
#include <stdint.h>
#define vec2_unpack(p) p.x, p.y
#define vec2_mul(v, s)                                                                             \
    (vec2) {                                                                                       \
        v.x *= s, v.y *= s                                                                         \
    }
#define vec2_print(v) printf("%s:{%f,%f}\n", #v, v.x, v.y)

#define arrlen(arr) (sizeof(arr) / sizeof(arr[0]))

#define len(expr) sizeof(expr)  // been using too much python

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t  i8;
