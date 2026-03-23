#pragma once
#include "types.h"
#include <math.h>
#pragma clang diagnostic ignored "-Wgnu-folding-constant"
#pragma clang diagnostic ignored "-Wmissing-braces"

// # g (grid) prefix means its a *grid-space* **absolute** position
// # l (local) prefix means its a *local-space* grid position (local to the piece's space)
// # s (screen) prefix means its a screen position - (top left is 0.0,0.0)
typedef struct vec2 {
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

#define vec2_unpack(p) p.x, p.y

static inline vec2 rotate_vec2(vec2 a, double theta) {
    return (vec2){ .x = a.x * cos(theta) - a.y * sin(theta),
                   .y = a.x * sin(theta) + a.y * cos(theta) };
}

// ->return a *vec2* containing the *piecewise* min of *a* and *b*'s components
static inline vec2 vec2_min(vec2 a, vec2 b) {
    return (vec2){
        .x = fmin(a.x, b.x),
        .y = fmin(a.y, b.y),
    };
}

// -> return a *vec2* containing the *piecewise* max of *a* and *b*'s components
static inline vec2 vec2_max(vec2 a, vec2 b) {
    return (vec2){
        .x = fmax(a.x, b.x),
        .y = fmax(a.y, b.y),
    };
}

// -> return a *vec2* containing the *piecewise* subtraction of *a* from *b*
static inline vec2 vec2_sub(vec2 a, vec2 b) {
    return (vec2){ a.x - b.x, a.y - b.y };
}
// -> return a *vec2* containing the *piecewise* addtion of *a* to *b*
static inline vec2 vec2_add(const vec2 a, const vec2 b) {
    return (vec2){ a.x + b.x, a.y + b.y };
}
// -> return a *vec2* containing the *piecewise* multiplication of *v* by *scalar*
static inline vec2 vec2_mul(vec2 v, double scalar) {
    return (vec2){ v.x * scalar, v.y * scalar };
}

// -> return a *vec2* containing the *piecewise* **REAL** division of *v* by *scalar*
static inline vec2 vec2_div(vec2 v, double scalar) {
    return (vec2){ v.x / scalar, v.y / scalar };
}

// -> return a *vec2* containing the *piecewise* **INTEGER** division of *v* by *scalar*
static inline vec2 vec2_idiv(const vec2 v, const i64 scalar) {
    return (vec2){
        (float)(v.x / scalar),
        (float)(v.y / scalar),
    };
}

#define vec2_print(v) printf("%s:{%f,%f}\n", #v, v.x, v.y)

// -> translate *screen-space* `vec2` to *grid-space* `vec2`.
static inline vec2 screen_to_grid(const vec2 s_pos) {
    return vec2_idiv(s_pos, BLOCK_SZ);
}

// -> translate *grid-space* `vec2` to *screen-space* `vec2`.
static inline vec2 grid_to_screen(const vec2 g_pos) {
    return vec2_mul(g_pos, BLOCK_SZ);
}

// -> translate *local-space* `vec2` to *grid-space* `vec2`.
// # **l_pos** must be a `local` space coordinate.
// # **g_origin** must be the `grid` space coordinate describing the *origin* of some local space
static inline vec2 local_to_grid(const vec2 l_pos, const vec2 g_origin) {
    return vec2_add(l_pos, g_origin);
}
