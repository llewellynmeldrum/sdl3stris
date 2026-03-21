#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "game/draw.h"
#include "game/piecequeue.h"

#include "debug_overlay.h"
#include "logger.h"
#include "piecedata.h"
#include "ringbuffer.h"
#include "sdlwrappers.h"
#include "sugar.h"
#include "timing.h"
#include "types.h"
void set_debug_overlay(void);

typedef struct Cell {
    // idk
} Cell;
typedef struct GridState {
    Cell cells[DEF_CELLCOUNT];
} GridState;

typedef enum GameState {
    GameStateStartMenu,
    GameStatePaused,
    GameStateActive,
} GameState;

typedef enum Direction {
    Direction_UP,
    Direction_LEFT,
    Direction_DOWN,
    Direction_RIGHT,
} Direction;

typedef struct Piece {
    PieceData* pd;
    Direction  facing;
} Piece;

typedef struct GameContext {
    GameState  state;
    PieceQueue piecequeue;
    size_t     tick;  // gametick = 20 per second, gametick = (ms_since_start / 1000)*20
    GridState  gridstate;
    double     droptimer;
    Piece      fallingPiece;
} GameContext;

PieceType random_piece() {
    return (PieceType)urand(PieceType_START, PieceType_COUNT);
}
GameContext init_GameContext() {
    // 1. create piece_queue
    // 2. set gametick
    // 3. set gamestate
    GameContext gtx = (GameContext){
        .state = GameStateActive,
        .piecequeue = pq_create(),
        .tick = 0,
        .droptimer = 5000.0,  // ms until a new piece is dropped
                              // starts counting down when there is no piece in the air
        .fallingPiece = {},
    };
    srand(time(NULL));
    for (int i = 0; i < PQ_CAPACITY; i++) {
        pq_push(&gtx.piecequeue, random_piece());
        // LOGLN("%d", gtx.piecequeue.head->type);
    }
    return gtx;
}
GameContext gtx;

void profileStats(i64* res_param_dt_ms) {
    ctx.frame_count++;  // i like the first frame to be 1
    //
    ctx.perf.ms_thisframe = get_current_ms();
    double dt = ctx.perf.ms_thisframe - ctx.perf.ms_lastframe;
    if (!ctx.perf.ms_lastframe) {
        dt = 1;
    }
    double fps = 1000.0 / dt;
    rb_push(ctx.perf.ft_rb, &dt);
    rb_push(ctx.perf.fps_rb, &fps);
    set_debug_overlay();

    *res_param_dt_ms = (i64)dt;
}

void drawPieceQueue(PieceQueue pq) {
    PieceQueueNode* cur = pq.head;
    vec2            g_queue_pos = { ctx.cols - PLAYFIELD_GXOFFSET + 1, 2 };
    LOGERR("This function is commented out.");
    /*
    for (int i = 0; i < pq.size; i++) {
        PieceData* pd = get_piece_data(cur->type);
        g_drawPiece(g_queue_pos, cur->type);
        g_queue_pos.y += 4;
        cur = cur->prev;  // moving down the queue towards the back
    }
    */
}

GameContext updateGameContext(i64 dt_ms, GameContext gtx) {
    // go through state, all that other shit and build new frames gamecontext based on inputs
    // handle input

    ctx.input.key_repeat_delay_ms_remaining -= dt_ms;

    if (ctx.input.rotate_left_pressed) {
        gtx.fallingPiece.facing++;
        gtx.fallingPiece.facing %= 3;
        LOG("%d\n", gtx.fallingPiece.facing);
        ctx.input.rotate_left_pressed = false;
    }
    if (ctx.input.rotate_right_pressed) {
        gtx.fallingPiece.facing--;
        gtx.fallingPiece.facing %= 3;
        LOG("%d\n", gtx.fallingPiece.facing);
        ctx.input.rotate_right_pressed = false;
    }
    ctx.input.key_repeat_delay_ms_remaining = ctx.input.PRESS_DELAY_MS;
    return gtx;
}
SDL_AppResult SDL_AppIterate(void* _) {
    i64 dt_ms = {};
    profileStats(&dt_ms);
    gtx = updateGameContext(dt_ms, gtx);

    setcolor(ctx.draw.clear_color);
    SDL_RenderClear(ctx.renderer);

    drawWalls();
    {
        vec2 g_tlpos = { 2, 2 };
        for (PieceType T = 0; T < 4; T++) {
            g_drawPiece(g_tlpos, T, gtx.fallingPiece.facing);
            g_tlpos.x += 5;
        }
    }
    {
        // figure out transparency it would be nice for debugging maybe
        // anyway figure out the origins and get rotation down.
        //
        vec2 g_tlpos = { 2, 8 };
        for (PieceType T = 4; T < PieceType_COUNT; T++) {
            g_drawPiece(g_tlpos, T, gtx.fallingPiece.facing);
            g_tlpos.x += 5;
        }
    }
    // drawPieceQueue(gtx.piecequeue);
    // // playgrid:
    // drawGrid((vec2){ PLAYFIELD_GXOFFSET, 0 },
    //         (vec2){ ctx.cols - PLAYFIELD_GXOFFSET, PLAYFIELD_HEIGHT });
    //
    // TESTING ROTATION, ACTUAL ROTATION WILL APPLY TO OFFSETS
    //    vec2 rotated_offsets[OFFSET_LEN] = {};
    //    for (int i = 0; i < OFFSET_LEN; i++) {
    //        rotated_offsets[i] = piece->l_blockOffsets[i];
    //        double rotation = M_PI_2 * 1;  // ctx.rotation_idx;
    //        rotated_offsets[i] = rotate_vec2(rotated_offsets[i], rotation);
    //    }
    drawDebugOverlay(true);
    SDL_RenderPresent(ctx.renderer);
    ctx.perf.ms_lastframe = ctx.perf.ms_thisframe;
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppInit(void** _, int argc, char* argv[]) {
    ctx = init_ctx();
    gtx = init_GameContext();
    SDL_SetAppMetadata("cstris", "1.0", "lmeldrum.cstris");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("cstris - window", ctx.w, ctx.h, SDL_WINDOW_TRANSPARENT,
                                     &ctx.window, &ctx.renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(ctx.renderer, ctx.w, ctx.h,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);
    SDL_SetRenderDrawBlendMode(ctx.renderer, SDL_BLENDMODE_BLEND);

    ctx.clock_freq = SDL_GetPerformanceFrequency();
    return SDL_APP_CONTINUE; /* carry on with the program! */
}

void printmpos() {
    double x = ctx.input.s_mpos.x;
    double y = ctx.input.s_mpos.y;
    LOG("CLICK @: %.2f, %.2f\n", x, y);
}
// clang-format off
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    (void)appstate;
    ctx.input.rotate_left_pressed = false;
    ctx.input.rotate_right_pressed = false;
    switch (event->type) {
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        ctx.input.m1_pressed = (event->button.button == 1) ? true : ctx.input.m1_pressed,
//        printmpos();
        ctx.input.m2_pressed = (event->button.button == 2) ? true : ctx.input.m2_pressed;
        break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
        ctx.input.m1_pressed = (event->button.button == 1) ? false : ctx.input.m1_pressed;
        ctx.input.m2_pressed = (event->button.button == 2) ? false : ctx.input.m2_pressed;
        break;

    case SDL_EVENT_MOUSE_MOTION:
        ctx.input.s_mpos = (vec2){ event->motion.x, event->motion.y };
        ctx.input.g_mpos = snapToGrid((vec2){ round(event->motion.x), round(event->motion.y) });
        break;

    case SDL_EVENT_KEY_DOWN:
        switch(event->key.key){
        case SDLK_UP: ctx.input.slow_drop_pressed = true; break;
        case SDLK_DOWN: ctx.input.fast_drop_pressed = true; break;
        case SDLK_LEFT: ctx.input.rotate_left_pressed = true; break;
        case SDLK_RIGHT: ctx.input.rotate_right_pressed = true; break;
        }
        break;
    case SDL_EVENT_KEY_UP:
        switch(event->key.key){
//        case SDLK_UP:       ctx.input.slow_drop_pressed =    false; break;
//        case SDLK_DOWN:     ctx.input.fast_drop_pressed =  false; break;
//        case SDLK_LEFT:     ctx.input.rotate_left_pressed =  false; break;
//        case SDLK_RIGHT:    ctx.input.rotate_right_pressed = false; break;
        }
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

// clang-format on
//
/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    (void)appstate;
    (void)result;
    /* SDL will clean up the window/renderer for us. */
}

void set_debug_overlay(void) {
    overlay_reset();
    vec2        mpos = { ctx.input.s_mpos.x, ctx.input.s_mpos.y };
    vec2        s_mpos = screen_to_grid(mpos);
    const char* m1str = (ctx.input.m1_pressed ? "DOWN" : "UP");
    OVERLAY_PRINTLN("mpos: %.4f, %.4f", mpos.x, mpos.y);
    OVERLAY_PRINTLN("m1: %s", m1str);
    OVERLAY_PRINTLN("spos: %.2f %.2f", vec2_unpack(ctx.input.s_mpos));
    OVERLAY_PRINTLN("gpos: %.2f %.2f", vec2_unpack(s_mpos));
    OVERLAY_PRINTLN("m1:%d|m2:%d|UP:%d|DN:%d|LE:%d|RI:%d", ctx.input.m1_pressed,
                    ctx.input.m2_pressed, ctx.input.slow_drop_pressed, ctx.input.fast_drop_pressed,
                    ctx.input.rotate_left_pressed, ctx.input.rotate_right_pressed);

    if (ctx.perf.show_perf_in_debug) {
        OVERLAY_PRINTLN("frametime: %.4lf", dbl_rb_avg(ctx.perf.ft_rb));
        OVERLAY_PRINTLN("framecount: %ld", ctx.frame_count);
        OVERLAY_PRINTLN("fps: %.1lf", dbl_rb_avg(ctx.perf.fps_rb));
    }
}
