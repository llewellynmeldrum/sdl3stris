#pragma once
#include "ringbuffer.h"
#include "types.h"
#include <SDL3/SDL.h>

typedef struct {
    bool m1down;
    bool m2down;
    vec2 mpos;   // raw mouse position, updated on mouse move event
    vec2 mgpos;  // mouse position snapped to grid.
} Input;

typedef struct {
    bool        show_perf_in_debug;
    RingBuffer* fps_rb;
    RingBuffer* ft_rb;  //  frametime (ms).
    double      ms_lastframe;
    double      ms_thisframe;
} Profiler;

typedef struct {
    SDL_FColor clear_color;
} Draw;

typedef struct SDLContext {
    SDL_Window*   window;
    SDL_Renderer* renderer;
    Input         input;  // interface for input handler
    Profiler      perf;   // holds performance info
    Draw          draw;   // holds draw settings
    // Primitives: (these should probably go into one of the other structs)
    u64    frame_count;
    u64    clock_freq;
    double w;
    double h;
    i64    cols;
    i64    rows;
} SDLContext;
extern SDLContext ctx;  // global context, i dont need multiple.

typedef struct {
    SDL_Vertex* data;
    size_t      size;
} VertexArray;
// Background piece color
void       drawVertexArray(VertexArray arr);
SDLContext init_ctx();

// SDL_FColor from 0->255 grey value.
#define gr(gr) (SDL_FColor){ gr / 255.0, gr / 255.0, gr / 255.0, 1.0 }

// SDL_FColor from 3x 0->255 rgb values
#define rgb(r, g, b) (SDL_FColor){ r / 255.0, g / 255.0, b / 255.0, 1.0 }

// SDL_FColor from 4x 0->255 rgba values
#define rgba(r, g, b, a) (SDL_FColor){ r / 255.0, g / 255.0, b / 255.0, a / 255.0 }

// Sets the renderer draw color from 3x 0->1.0 rgb values
#define setrgb(r, g, b) SDL_SetRenderDrawColor(ctx.renderer, r, g, b, 255)

// Sets the renderer draw color from 4x 0->1.0 rgba values
#define setrgba(r, g, b, a) SDL_SetRenderDrawColor(ctx.renderer, r, g, b, a)

// Sets the renderer draw color from a generic rgba(1.0,1.0,1.0,1.0) color struct.
#define setcolor(c) SDL_SetRenderDrawColorFloat(ctx.renderer, c.r, c.g, c.b, c.a)

// Gets the renderer draw color for a generic rgba(1.0,1.0,1.0,1.0) color struct.
#define getcolor(c) SDL_GetRenderDrawColorFloat(ctx.renderer, &c.r, &c.g, &c.b, &c.a)

// SDL_Vertex from color, and scalar x,y position
#define VTX(c, x, y)                                                                               \
    (SDL_Vertex) {                                                                                 \
        .color = c, .position = { x, y }, .tex_coord = { 0 }                                       \
    }
