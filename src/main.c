#include "sugar.h"
#include <math.h>
#include <stdlib.h>
#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

#include "debugtext.h"
#include "ringbuffer.h"
#include "sdlwrappers.h"
#include "sugar.h"
#include "timing.h"
#include "types.h"
const i64 PLAYFIELD_GXOFFSET = 5;
const i64 PLAYFIELD_WIDTH = 10;
const i64 PLAYFIELD_HEIGHT = 20;
const i64 PLAYFIELD_GYOFFSET = DEF_ROWS - PLAYFIELD_HEIGHT;

void draw_triangle(SDL_Vertex* tri_verts, size_t vert_count);

#define DrawTriangle(vertices) draw_triangle(vertices, arrlen(vertices))

void draw_triangle(SDL_Vertex* vertices, size_t vert_count) {
    SDL_RenderGeometry(ctx.renderer, NULL, vertices, vert_count, NULL, 0);
}

#define DrawLine(p1, p2) SDL_RenderLine(ctx.renderer, p1.x, p1.y, p2.x, p2.y)

#define QUAD_VERT_COUNT 4
#define DrawQuad(vertices, color) make_quad(vertices, color)
void render_quad(SDL_Vertex* vertices) {
    const int quad_indices[] = {
            0, 1, 2,  // top left tri
            0, 2, 3   // bottom left tri
    };
    SDL_RenderGeometry(ctx.renderer, NULL, vertices, QUAD_VERT_COUNT, quad_indices,
                       arrlen(quad_indices));
}
void make_quad(vec2* points, SDL_FColor color) {
    SDL_Vertex vertices[QUAD_VERT_COUNT] = (SDL_Vertex[]){
            VTX(color, points[0].x, points[0].y),
            VTX(color, points[1].x, points[1].y),
            VTX(color, points[2].x, points[2].y),
            VTX(color, points[3].x, points[3].y),
    };
    render_quad(vertices);
}
void drawBlockV(vec2 tl, double len, const SDL_FColor piece_colors[4]) {
    double bord = len * 0.14;  // border width
    {
        // MAIN QUAD
        vec2 quad_verts[] = {
                tl,
                {tl.x + len, tl.y},
                {tl.x + len, tl.y + len},
                {tl.x, tl.y + len},
        };
        DrawQuad(quad_verts, piece_colors[2]);
    }
    {
        // LEFT EDGE
        vec2 o = {tl.x, tl.y};
        vec2 quad_verts[] = {
                o,
                {o.x + bord, o.y + bord},
                {o.x + bord, o.y + len - bord},
                {o.x, o.y + len},
        };
        DrawQuad(quad_verts, piece_colors[1]);
    }
    {
        // RIGHT EDGE
        vec2 o = {tl.x + len - bord, tl.y};
        vec2 quad_verts[] = {
                {o.x, o.y + bord},
                {o.x + bord, o.y},
                {o.x + bord, o.y + len},
                {o.x, o.y + len - bord},
        };
        DrawQuad(quad_verts, piece_colors[1]);
    }
    {
        // TOP EDGE
        vec2 o = {tl.x, tl.y};
        vec2 quad_verts[] = {
                {o.x, o.y},
                {o.x + len, o.y},
                {o.x + len - bord, o.y + bord},
                {o.x + bord, o.y + bord},
        };
        DrawQuad(quad_verts, piece_colors[3]);
    }
    {
        // BOT EDGE
        vec2 o = {tl.x, tl.y + len};
        vec2 quad_verts[] = {
                {o.x, o.y},
                {o.x + bord, o.y - bord},
                {o.x + len - bord, o.y - bord},
                {o.x + len, o.y},
        };
        DrawQuad(quad_verts, piece_colors[0]);
    }
}

void drawBlockOutlineV(vec2 tl, double len, const SDL_FColor piece_colors[4]) {
    double bord = 1;  // border width
    {
        // LEFT EDGE
        vec2 o = {tl.x, tl.y};
        vec2 quad_verts[] = {
                o,
                {o.x + bord, o.y + bord},
                {o.x + bord, o.y + len - bord},
                {o.x, o.y + len},
        };
        DrawQuad(quad_verts, piece_colors[4]);
    }
    {
        // RIGHT EDGE
        vec2 o = {tl.x + len - bord, tl.y};
        vec2 quad_verts[] = {
                {o.x, o.y + bord},
                {o.x + bord, o.y},
                {o.x + bord, o.y + len},
                {o.x, o.y + len - bord},
        };
        DrawQuad(quad_verts, piece_colors[4]);
    }
    {
        // TOP EDGE
        vec2 o = {tl.x, tl.y};
        vec2 quad_verts[] = {
                {o.x, o.y},
                {o.x + len, o.y},
                {o.x + len - bord, o.y + bord},
                {o.x + bord, o.y + bord},
        };
        DrawQuad(quad_verts, piece_colors[4]);
    }
    {
        // BOT EDGE
        vec2 o = {tl.x, tl.y + len};
        vec2 quad_verts[] = {
                {o.x, o.y},
                {o.x + bord, o.y - bord},
                {o.x + len - bord, o.y - bord},
                {o.x + len, o.y},
        };
        DrawQuad(quad_verts, piece_colors[4]);
    }
}
void draw_block(double tlx, double tly, double len, const SDL_FColor piece_colors[4]) {
    drawBlockV((vec2){tlx, tly}, len, piece_colors);
}

void drawGrid() {
    SDL_FColor previous_color;
    getcolor(previous_color);
    setcolor(gr(128));
    vec2 o = {PLAYFIELD_GXOFFSET, 0};
    for (int y = 1; y < PLAYFIELD_HEIGHT; y++) {
        vec2 start = {o.x, o.y + y};
        vec2 end = {o.x + PLAYFIELD_WIDTH, o.y + y};
        start = vec2_mul(start, BLOCK_SZ);
        end = vec2_mul(end, BLOCK_SZ);
        DrawLine(start, end);
    }
    for (int x = 1; x < PLAYFIELD_WIDTH; x++) {
        vec2 start = {o.x + x, o.y};
        vec2 end = {o.x + x, o.y + PLAYFIELD_HEIGHT};
        start = vec2_mul(start, BLOCK_SZ);
        end = vec2_mul(end, BLOCK_SZ);
        DrawLine(start, end);
    }
    setcolor(previous_color);
}

void drawBoundaries() {
    // draw floor
    {
        vec2 o = {0, ctx.rows - PLAYFIELD_GYOFFSET};
        for (int x = 0; x < DEF_COLS; x++) {
            for (int y = 0; y < PLAYFIELD_GYOFFSET; y++) {
                vec2 pos = {o.x + x, o.y + y};
                pos = vec2_mul(pos, BLOCK_SZ);
                drawBlockV(pos, BLOCK_SZ, grey);
            }
        }
    }
    {
        // draw left wall
        vec2 o = {0, 0};
        for (int x = 0; x < PLAYFIELD_GXOFFSET; x++) {
            for (int y = 0; y < DEF_ROWS; y++) {
                vec2 pos = {o.x + x, o.y + y};
                pos = vec2_mul(pos, BLOCK_SZ);
                drawBlockV(pos, BLOCK_SZ, grey);
            }
        }
    }
    {
        // draw right wall
        vec2 o = {ctx.cols - PLAYFIELD_GXOFFSET, 0};
        for (int x = 0; x < PLAYFIELD_GXOFFSET; x++) {
            for (int y = 0; y < DEF_ROWS; y++) {
                vec2 pos = {o.x + x, o.y + y};
                pos = vec2_mul(pos, BLOCK_SZ);
                drawBlockV(pos, BLOCK_SZ, grey);
            }
        }
    }
}
SDL_AppResult SDL_AppIterate(void* _) {
    ctx.frame_count++;
    double ms_thisframe = get_current_ms();
    double ft_ms = ms_thisframe - ctx.perf.ms_lastframe;
    double fps = 1000.0 / (ft_ms);
    rb_push(ctx.perf.ft_rb, &ft_ms);
    rb_push(ctx.perf.fps_rb, &fps);

    setcolor(ctx.draw.clear_color);
    SDL_RenderClear(ctx.renderer);

    drawBoundaries();
    drawGrid();

    /*
    vec2   o = {50, 50};
    double ext = 40;

    vec2 i_piece_offsets[] = {
            {0, 0},
            {0, 1},
            {0, 2},
            {0, 3},
    };
    for (int i = 0; i < arrlen(i_piece_offsets); i++) {
        vec2 offset = i_piece_offsets[i];
        drawBlockV((vec2){o.x + ext * offset.x, o.y + ext * offset.y}, ext, cyan);
    }

    o.x += ext * 3;
    vec2 j_piece_offsets[] = {
            {0, 0},
            {0, 1},
            {0, 2},
            {-1, 2},
    };
    for (int i = 0; i < arrlen(j_piece_offsets); i++) {
        vec2 offset = j_piece_offsets[i];
        drawBlockV((vec2){o.x + ext * offset.x, o.y + ext * offset.y}, ext, blue);
    }

    {
        o.x += ext * 3;
        vec2 l_piece_offsets[] = {
                {0, 0},
                {0, 1},
                {0, 2},
                {1, 2},
        };
        for (int i = 0; i < arrlen(l_piece_offsets); i++) {
            vec2 offset = l_piece_offsets[i];
            drawBlockV((vec2){o.x + ext * offset.x, o.y + ext * offset.y}, ext, orange);
        }
    }
    {
        o.x += ext * 3;
        vec2 o_piece_offsets[] = {
                {0, 0},
                {0, 1},
                {1, 0},
                {1, 1},
        };
        for (int i = 0; i < arrlen(o_piece_offsets); i++) {
            vec2 offset = o_piece_offsets[i];
            drawBlockV((vec2){o.x + ext * offset.x, o.y + ext * offset.y}, ext, yellow);
        }
    }
    {
        o.x = 50;
        o.y += ext * 5;
        vec2 s_piece_offsets[] = {
                {0, 1},
                {1, 1},
                {1, 0},
                {2, 0},
        };
        for (int i = 0; i < arrlen(s_piece_offsets); i++) {
            vec2 offset = s_piece_offsets[i];
            drawBlockV((vec2){o.x + ext * offset.x, o.y + ext * offset.y}, ext, green);
        }
    }
    {
        o.x += ext * 4;
        vec2 t_piece_offsets[] = {
                {0, 0},
                {1, 0},
                {2, 0},
                {1, 1},
        };
        for (int i = 0; i < arrlen(t_piece_offsets); i++) {
            vec2 offset = t_piece_offsets[i];
            drawBlockV((vec2){o.x + ext * offset.x, o.y + ext * offset.y}, ext, purple);
        }
    }
    {
        o.x += ext * 5;
        vec2 s_piece_offsets[] = {
                {0, 0},
                {1, 0},
                {1, 1},
                {2, 1},
        };
        for (int i = 0; i < arrlen(s_piece_offsets); i++) {
            vec2 offset = s_piece_offsets[i];
            drawBlockV((vec2){o.x + ext * offset.x, o.y + ext * offset.y}, ext, red);
        }
    }

    */
    drawDebugWindow();

    SDL_RenderPresent(ctx.renderer);
    ctx.perf.ms_lastframe = ms_thisframe;
    ctx.perf.show = false;
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppInit(void** _, int argc, char* argv[]) {
    ctx = init_ctx();
    ctx.perf.ms_lastframe = get_current_ms();
    SDL_SetAppMetadata("cstris", "1.0", "lmeldrum.cstris");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("cstris - window", ctx.w, ctx.h, 0, &ctx.window,
                                     &ctx.renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(ctx.renderer, ctx.w, ctx.h,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);

    ctx.clock_freq = SDL_GetPerformanceFrequency();
    return SDL_APP_CONTINUE; /* carry on with the program! */
}

void printmpos() {
    double x = ctx.input.mpos.x;
    double y = ctx.input.mpos.y;
    printf("CLICK @: %.2f, %.2f\n", x, y);
}
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    (void)appstate;
    switch (event->type) {
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        ctx.input.m1 = (event->button.button == 1) ? true : ctx.input.m1, printmpos();
        ctx.input.m2 = (event->button.button == 2) ? true : ctx.input.m2;
        break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
        ctx.input.m1 = (event->button.button == 1) ? false : ctx.input.m1;
        ctx.input.m2 = (event->button.button == 2) ? false : ctx.input.m2;
        break;

    case SDL_EVENT_MOUSE_MOTION:
        ctx.input.mpos = (vec2){event->motion.x, event->motion.y};
        break;

    case SDL_EVENT_QUIT:  //
        return SDL_APP_SUCCESS;
        break;

    default:  //
        return SDL_APP_CONTINUE;
        break;
    }
    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    (void)appstate;
    (void)result;
    /* SDL will clean up the window/renderer for us. */
}
