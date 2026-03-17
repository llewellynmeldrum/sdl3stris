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

const extern SDL_FColor red;
const extern SDL_FColor green;
const extern SDL_FColor blue;
const extern SDL_FColor grey;
// T piece
const extern SDL_FColor purple_0;
const extern SDL_FColor purple_1;
const extern SDL_FColor purple_2;
const extern SDL_FColor purple_3;

// S piece --__
const extern SDL_FColor green_0;
const extern SDL_FColor green_1;
const extern SDL_FColor green_2;
const extern SDL_FColor green_3;

// Z piece __--
const extern SDL_FColor red_0;
const extern SDL_FColor red_1;
const extern SDL_FColor red_2;
const extern SDL_FColor red_3;

// J piece _-_
const extern SDL_FColor blue_0;
const extern SDL_FColor blue_1;
const extern SDL_FColor blue_2;
const extern SDL_FColor blue_3;

// O piece square
const extern SDL_FColor yellow_0;
const extern SDL_FColor yellow_1;
const extern SDL_FColor yellow_2;
const extern SDL_FColor yellow_3;

// L piece -_-
const extern SDL_FColor orange_0;
const extern SDL_FColor orange_1;
const extern SDL_FColor orange_2;
const extern SDL_FColor orange_3;

// I piece
const extern SDL_FColor cyan_0;
const extern SDL_FColor cyan_1;
const extern SDL_FColor cyan_2;
const extern SDL_FColor cyan_3;

void       drawVertexArray(VertexArray arr);
SDLContext init_ctx();

#define rgb(r, g, b) (SDL_FColor){r / 255.0, g / 255.0, b / 255.0, 1.0}
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
