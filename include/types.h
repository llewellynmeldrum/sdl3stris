#pragma once
#include <stdint.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t  i8;

#define DEF_COLS 20
#define DEF_ROWS (DEF_HEIGHT / BLOCK_SZ)

#define DEF_WIDTH (double)640
#define DEF_HEIGHT (double)800
#define BLOCK_SZ (DEF_WIDTH / DEF_COLS)

#pragma clang diagnostic ignored "-Wgnu-folding-constant"
#define DEF_CELLCOUNT (int)(DEF_COLS * DEF_ROWS)
#pragma clang diagnostic ignored "-Wmissing-braces"
// >>> g (grid) prefix means its a grid position. (absolute)
// >>> l (local) prefix means its a grid position local to the piece's space
// >>> s (screen) prefix means its a screen position.
typedef struct {
    union {
        struct {
            float x;
            float y;
        };
        struct {
            float width;
            float height;
        };
    };
} vec2;
