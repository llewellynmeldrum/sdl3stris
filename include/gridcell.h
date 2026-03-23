#pragma once
#include "types.h"
#include "vec2.h"

typedef struct GridCell {
    bool        occupied;
    ColorScheme colorscheme;
} GridCell;

static inline int get_grid_idx(size_t x, size_t y) {
    return y * DEF_ROWS + x;
}
static inline int get_grid_idxv(vec2 v) {
    return v.y * DEF_ROWS + v.x;
}
