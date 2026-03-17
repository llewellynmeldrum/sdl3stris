#pragma once
#include <stdint.h>

#include "sugar.h"

#define DEF_WIDTH 640
#define DEF_HEIGHT 800
#define DEF_COLS 20
#define BLOCK_SZ (DEF_WIDTH / DEF_COLS)
#define DEF_ROWS (DEF_HEIGHT / BLOCK_SZ)

#pragma clang diagnostic ignored "-Wmissing-braces"
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
