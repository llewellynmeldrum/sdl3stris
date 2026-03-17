#pragma once
#include "ringbuffer.h"
#include "types.h"
#include <SDL3/SDL.h>

typedef struct {
    bool m1;
    bool m2;
    vec2 mpos;
} Input;

typedef struct {
    bool        show;
    RingBuffer* fps_rb;
    RingBuffer* ft_rb;
    double      ms_lastframe;
} Perf;

typedef struct {
    SDL_FColor clear_color;
} Draw;
typedef struct {
    SDL_Window*   window;
    SDL_Renderer* renderer;
    Input         input;
    Perf          perf;
    Draw          draw;
    u64           frame_count;
    u64           clock_freq;
} SDLContext;
extern SDLContext ctx;

typedef struct {
    SDL_Vertex* data;
    size_t      size;
} VertexArray;

// T piece
const SDL_FColor purple[4];
// S piece --__
const SDL_FColor green[4];
// Z piece __--
const SDL_FColor red[4];
// J piece _-_
const SDL_FColor blue[4];
// O piece square
const SDL_FColor yellow[4];
// L piece -_-
const SDL_FColor orange[4];
// I piece
const SDL_FColor cyan[4];

void       drawVertexArray(VertexArray arr);
SDLContext init_ctx();

#define rgb(r, g, b) (SDL_FColor){r / 255.0, g / 255.0, b / 255.0, 1.0}
#define rgba(r, g, b, a) (SDL_FColor){r / 255.0, g / 255.0, b / 255.0, a / 255.0}
#define setrgb(r, g, b) SDL_SetRenderDrawColor(ctx.renderer, r, g, b, 255)
#define setrgba(r, g, b, a) SDL_SetRenderDrawColor(ctx.renderer, r, g, b, a)
#define setcolor(c) SDL_SetRenderDrawColorFloat(ctx.renderer, c.r, c.g, c.b, c.a)
#define getcolor(c) SDL_GetRenderDrawColorFloat(ctx.renderer, &c.r, &c.g, &c.b, &c.a)
#define makergba(r, g, b, a)                                                                       \
    (SDL_FColor) {                                                                                 \
        r, g, b, a                                                                                 \
    }

#define VTX(c, x, y)                                                                               \
    (SDL_Vertex) {                                                                                 \
        .color = c, .position = {x, y}, .tex_coord = {0}                                           \
    }
