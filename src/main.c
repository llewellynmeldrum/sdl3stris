#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "game/draw.h"
#include "game/piecequeue.h"

#include "debugtext.h"
#include "piecedata.h"
#include "ringbuffer.h"
#include "sdlwrappers.h"
#include "sugar.h"
#include "timing.h"
#include "types.h"

typedef struct Cell {
    // idk
} Cell;
typedef struct GridState {
    Cell cells[DEF_CELLCOUNT];
} GridState;

typedef enum GameState {
    GameStatePaused,  // game is paused
    GameStateActive,  // pieces are falling
} GameState;
typedef struct GameContext {
    GameState  state;
    PieceQueue piecequeue;
    size_t     tick;  // gametick = 20 per second, gametick = (ms_since_start / 1000)*20
    GridState  gridstate;
    double     droptimer;
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
    };
    srand(time(NULL));
    for (int i = 0; i < PQ_CAPACITY; i++) {
        pq_push(&gtx.piecequeue, random_piece());
        printf("%d", gtx.piecequeue.head->type);
    }
    return gtx;
}
GameContext gtx;

void profileStats() {
    ctx.frame_count++;  // i like the first frame to be 1
    ctx.perf.ms_thisframe = get_current_ms();
    double ft_ms = ctx.perf.ms_thisframe - ctx.perf.ms_lastframe;
    double fps = 1000.0 / (ft_ms);
    rb_push(ctx.perf.ft_rb, &ft_ms);
    rb_push(ctx.perf.fps_rb, &fps);
}

void drawPieceQueue(PieceQueue pq) {
    PieceQueueNode* cur = pq.head;
    vec2            g_queue_pos = { ctx.cols - PLAYFIELD_GXOFFSET + 1, 2 };
    for (int i = 0; i < pq.size; i++) {
        PieceDef* pd = get_piece_def(cur->type);
        drawPiece(g_queue_pos, cur->type);
        g_queue_pos.y += 4;
        cur = cur->prev;  // moving down the queue towards the back
    }
}
SDL_AppResult SDL_AppIterate(void* _) {
    profileStats();

    setcolor(ctx.draw.clear_color);
    SDL_RenderClear(ctx.renderer);

    drawWalls();
    drawPieceQueue(gtx.piecequeue);
    // drawGrid((vec2){ PLAYFIELD_GXOFFSET, 0 },
    //          (vec2){ ctx.cols - PLAYFIELD_GXOFFSET, PLAYFIELD_HEIGHT });
    drawDebugWindow();

    SDL_RenderPresent(ctx.renderer);
    ctx.perf.ms_lastframe = ctx.perf.ms_thisframe;
    ctx.perf.show_perf_in_debug = false;
    return SDL_APP_CONTINUE;
}

void print_string(void* input) {
    const char* s = input;
    printf("'%s'", s);
}
SDL_AppResult SDL_AppInit(void** _, int argc, char* argv[]) {
    ctx = init_ctx();
    gtx = init_GameContext();
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
        ctx.input.m1down = (event->button.button == 1) ? true : ctx.input.m1down, printmpos();
        ctx.input.m2down = (event->button.button == 2) ? true : ctx.input.m2down;
        break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
        ctx.input.m1down = (event->button.button == 1) ? false : ctx.input.m1down;
        ctx.input.m2down = (event->button.button == 2) ? false : ctx.input.m2down;
        break;

    case SDL_EVENT_MOUSE_MOTION:
        ctx.input.mpos = (vec2){ event->motion.x, event->motion.y };
        ctx.input.mgpos = snapToGrid((vec2){ round(event->motion.x), round(event->motion.y) });
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
