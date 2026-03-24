#include "vec2.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "game/draw.h"
#include "game/piecequeue.h"

#include "debug_overlay.h"
#include "gamecontext.h"
#include "gridcell.h"
#include "logger.h"
#include "piecedata.h"
#include "ringbuffer.h"
#include "sdlwrappers.h"
#include "sugar.h"
#include "timing.h"
#include "types.h"

static void overlay_setFields(void);
static void handleInputRequests(void);

static size_t tick_last_frame;

GameContext gtx;
SDLContext  ctx;

// 1. define this:
// #define DBG_REPORT_CALLS LOG_CALL
// 2. instantiate DBG_REPORT_CALLS, eg:
// ```c
// void foo(){
//      DBG_REPORT_CALLS;
//      // ...
// }
// ```
#define LOG_CALL                                                                                   \
    do {                                                                                           \
        LOGFN("Called.");                                                                          \
    } while (0)

#ifdef DBG_REPORT_CALLS
#define return(x)                                                                                  \
    do {                                                                                           \
        const char* str = TOSTR(x);                                                                \
        LOGFN("Returning: %s", str);                                                               \
        return x;                                                                                  \
    } while (0)
#else
#define return(x) return x
#endif

// #undef RETURN
// #define RETURN(x) return x
static bool isPieceInBounds(Piece p) {
    if (!gtx.isPieceActive)
        return (false);
    PieceData* pd = getPieceData(p.T);
    vec2       g_topLeftPos = p.g_pos;
    for (int blk_idx = 0; blk_idx < BLOCKS_PER_PIECE; blk_idx++) {
        size_t idx = getBlockOffset(gtx.activePiece.rotation, blk_idx);
        vec2   g_piecePos = local_to_grid(pd->l_blockOffsets[idx], g_topLeftPos);
        size_t gidx = get_grid_idxv(g_piecePos);
        if (g_piecePos.x >= PLAYFIELD_SPAWN_XMAX || g_piecePos.x < PLAYFIELD_SPAWN_XMIN ||
            g_piecePos.y >= PLAYFIELD_SPAWN_YMAX || g_piecePos.y < PLAYFIELD_SPAWN_YMIN) {
            LOGERR("%.f,%.f is out of bounds!", vec2_unpack(g_piecePos));
            return (false);
        }
        GridCell cell = gtx.grid[gidx];
        if (cell.occupied) {
            LOGERR("%.f,%.f is already occupied!", vec2_unpack(g_piecePos));
            return (false);
        }
    }
    return (true);
}

static void placeBlock(vec2 g_pos, ColorScheme colorscheme) {
    gtx.grid[get_grid_idxv(g_pos)] = (GridCell){ .colorscheme = colorscheme, .occupied = true };
}
static void placePiece(Piece activePiece) {
    if (!gtx.isPieceActive) {
        //        LOGERR("Cannot place piece, there is not one active. .droptimer:%lf",
        //        gtx.droptimer);
        return;
    }
    if (!isPieceInBounds(activePiece)) {
        //        LOGERR("Cannot place piece, it is out of bounds." /* It shouldve been moved back
        //        in when the
        //                " "illegal move attempt was made, check input handling.*/
        //               ".droptimer:%lf",
        //               gtx.droptimer);
        return;
    }

    PieceData* pd = getPieceData(activePiece.T);
    for (int i = 0; i < BLOCKS_PER_PIECE; i++) {
        int  idx = getBlockOffset(activePiece.rotation, i);
        vec2 g_pos = local_to_grid(pd->l_blockOffsets[idx], activePiece.g_pos);
        placeBlock(g_pos, pd->colorscheme);
    }
    gtx.isPieceActive = false;
    gtx.droptimer_current = gtx.droptimer;
}

static void game_start(SDL_Window* win, SDL_Renderer* rend, size_t w, size_t h) {
    tick_last_frame = 0;
    LOGLN("start_game(%p, %p, %zu, %zu)", win, rend, w, h);
    ctx = init_ctx(win, rend, w, h);
    gtx = init_GameContext();
}

static void game_restart(void) {
    game_start(ctx.window, ctx.renderer, ctx.w, ctx.h);
}

void profileFrameStats(i64* res_param_dt_ms) {
    ctx.frame_count++;  // i like the first frame to be 1
    //
    ctx.perf.ms_thisframe = get_current_ms(ctx.clock_freq);
    double dt = ctx.perf.ms_thisframe - ctx.perf.ms_lastframe;
    if (!ctx.perf.ms_lastframe) {
        dt = 1;
    }
    double fps = 1000.0 / dt;
    rb_push(ctx.perf.ft_rb, &dt);
    rb_push(ctx.perf.fps_rb, &fps);
    overlay_setFields();

    *res_param_dt_ms = (i64)dt;
}

void drawPieceQueue(PieceQueue pq) {
    PieceQueueNode* cur = pq.head;
    vec2            g_queue_pos = { ctx.cols - PLAYFIELD_XMIN + 1, 2 };
    for (int i = 0; i < pq.size; i++) {
        g_drawPiece(g_queue_pos, cur->type, 0);
        g_queue_pos.y += 4;
        cur = cur->prev;  // moving down the queue towards the back
    }
}

GridCell* getCell(GridCell* playfield, vec2 g_pos) {
    vec2   p_pos = { g_pos.x - PLAYFIELD_XMIN, g_pos.y - PLAYFIELD_YMIN };
    size_t idx = p_pos.y * DEF_COLS + p_pos.x;
    return &playfield[idx];
}

void setCell(GridCell* playfield, vec2 g_pos, GridCell val) {
    vec2   p_pos = { g_pos.x - PLAYFIELD_XMIN, g_pos.y - PLAYFIELD_YMIN };
    size_t idx = p_pos.y * DEF_COLS + p_pos.x;
    playfield[idx] = val;
}
// checks to see if any of the pieces block sit outside of the PLAYFIELD.

static inline i64 signed_mod(i64 x, i64 mod) {
    return llabs((x % mod));
}

void piece_rotateLeft(Piece* p) {
    p->rotation = signed_mod(p->rotation - 1, Direction_COUNT);
}
void piece_rotateRight(Piece* p) {
    p->rotation = signed_mod(p->rotation + 1, Direction_COUNT);
}
void attemptPieceRotation(Direction dir) {
    const Direction rotationBefore = gtx.activePiece.rotation;
    switch (dir) {
    case Direction_LEFT:
        piece_rotateLeft(&gtx.activePiece);
        if (!isPieceInBounds(gtx.activePiece)) {
            gtx.activePiece.rotation = rotationBefore;
        }
        break;

    case Direction_RIGHT:
        piece_rotateRight(&gtx.activePiece);
        if (!isPieceInBounds(gtx.activePiece)) {
            gtx.activePiece.rotation = rotationBefore;
        }
        break;
    default:
        LOGFATAL("Rotation of %s is unsupported.", Direction_tostr(dir));
        break;
    }
}

static bool attemptPieceMove(Direction dir) {
    bool moved = false;
    switch (dir) {
    case Direction_UP:
        LOGFATAL("Bro defies gravity");
        break;
    case Direction_DOWN:
        gtx.activePiece.g_pos.y++;
        if (!isPieceInBounds(gtx.activePiece)) {
            gtx.activePiece.g_pos.y--;
            placePiece(gtx.activePiece);
        } else {
            moved = true;
        }
        break;

    case Direction_LEFT:
        gtx.activePiece.g_pos.x--;
        if (!isPieceInBounds(gtx.activePiece)) {
            gtx.activePiece.g_pos.x++;
        } else {
            moved = true;
        }
        break;

    case Direction_RIGHT:
        gtx.activePiece.g_pos.x++;
        if (!isPieceInBounds(gtx.activePiece)) {
            gtx.activePiece.g_pos.x--;
        } else {
            moved = true;
        }

        break;
    default:
        LOGFATAL("Unknown direction (%d) supplied.", dir);
        break;
    }

    return moved;
}

static void app_clear(SDL_FColor color) {
    setcolor(color);
    SDL_RenderClear(ctx.renderer);
}

static void app_draw(void) {
    app_clear(ctx.draw.clear_color);
    if (gtx.isPieceActive) {
        g_drawPiece(gtx.activePiece.g_pos, gtx.activePiece.T, gtx.activePiece.rotation);
    }
    drawDebugOverlay(true);
    drawPlayfield();
    drawPieceQueue(gtx.piecequeue);
    if (gtx.state == GameStateGameOver) {
        // draw game over screen
    }

    SDL_RenderPresent(ctx.renderer);
}
static Piece piecequeue_getNewPiece(PieceQueue* piecequeue, const Piece* old) {
    pq_push(piecequeue, random_piece_type());
    Piece res = (Piece){
        .T = pq_pop(piecequeue),
        .g_pos = get_initial_position(old->T),
        .rotation = get_initial_rotation(old->T),
    };
    return res;
}

static bool grid_isRowFull(GameContext* gtx, size_t row) {
    for (int col = PLAYFIELD_XMIN; col < PLAYFIELD_XMAX; col++) {
        int idx = get_grid_idx(col, row);
        if (!gtx->grid[idx].occupied) {
            return false;
            break;
        }
    }
    return true;
}

static void grid_swapRows(GameContext* gtx, size_t rowA, size_t rowB) {
    for (int col = PLAYFIELD_XMIN; col < PLAYFIELD_XMAX; col++) {
        int idxA = get_grid_idx(col, rowA);
        int idxB = get_grid_idx(col, rowB);

        GridCell temp = gtx->grid[idxA];
        gtx->grid[idxA] = gtx->grid[idxB];
        gtx->grid[idxB] = temp;
    }
}
static void grid_bubbleRowsDown(GameContext* gtx, size_t startingRow) {
    // 1. if my row is empty, swap my contents with the row above me (row-1)
    // 2. row = row-1;
    for (int row = startingRow; row > PLAYFIELD_YMIN; row -= 1) {
        grid_swapRows(gtx, row, row - 1);
    }
}

static void grid_clearRow(GameContext* gtx, size_t row) {
    // 1. clear the row
    for (int col = PLAYFIELD_XMIN; col < PLAYFIELD_XMAX; col++) {
        int idx = get_grid_idx(col, row);
        gtx->grid[idx].occupied = false;
    }
}
static void grid_update(size_t dt_ms, GameContext* gtx) {
    for (int row = PLAYFIELD_YMAX - 1; row >= 0; row--) {
        while (grid_isRowFull(gtx, row)) {
            grid_clearRow(gtx, row);
            grid_bubbleRowsDown(gtx, row);
        }
    }
}
static void app_update(size_t dt_ms, GameContext* gtx) {
    const int app_TPS = 20;
    if (gtx->state == GameStateGameOver) {
        LOGERR("Game over!");
        LOGEXIT(EXIT_SUCCESS);
    }
    gtx->tick = (size_t)(ms_since_start(&ctx) / (1000.0 / app_TPS));  // 20 tps
    size_t deltaTick = gtx->tick - tick_last_frame;

    grid_update(dt_ms, gtx);
    if (!gtx->isPieceActive && gtx->droptimer_current <= 0) {
        gtx->activePiece = piecequeue_getNewPiece(&gtx->piecequeue, &gtx->activePiece);
        gtx->isPieceActive = true;
        if (!isPieceInBounds(gtx->activePiece)) {
            LOGNOTICE("Game over!");
            gtx->state = GameStateGameOver;
            return;
        }
        LOGNOTICE("Spawned new piece @%.f,%.f", vec2_unpack(gtx->activePiece.g_pos));
    }
    if (deltaTick > 0) {
        if (gtx->isPieceActive) {
            if (gtx->tick % gtx->moveDownDelay == 0) {
                attemptPieceMove(Direction_DOWN);
            }
            handleInputRequests();
        } else {  // piece is not active, decrement by dtick
            gtx->droptimer_current -= deltaTick;
        }
    }
    tick_last_frame = gtx->tick;
}

typedef enum { Event_Continue, Event_Exit } EventResult;
EventResult app_handleEvent(InputRequests* requests, SDL_Event* event) {
    requests->rotate_left = false;
    requests->rotate_right = false;

    switch (event->type) {
    case SDL_EVENT_KEY_DOWN:
        switch (event->key.key) {
        case SDLK_DOWN:
            [[fallthrough]];
        case SDLK_S:
            requests->fast_drop_pressed = true;
            break;

        case SDLK_UP:
            [[fallthrough]];
        case SDLK_W:
            requests->instant_drop_pressed = true;
            break;

        case SDLK_A:
            requests->rotate_left = true;
            break;

        case SDLK_D:
            requests->rotate_right = true;
            break;

        case SDLK_H:  // vim keybinds
            [[fallthrough]];
        case SDLK_LEFT:
            requests->move_left_pressed = true;
            break;

        case SDLK_L:  // vim keybinds
            [[fallthrough]];
        case SDLK_RIGHT:
            requests->move_right_pressed = true;
            break;

        case SDLK_R:
            game_restart();
            break;
        }
        break;

    case SDL_EVENT_QUIT:
        return Event_Exit;
        break;

    default:
        return Event_Continue;
        break;
    }
    return Event_Continue;
}

static void handleInputRequests(void) {
    if (ctx.inputReq.rotate_left) {
        attemptPieceRotation(Direction_LEFT);
        ctx.inputReq.rotate_left = false;
    }

    if (ctx.inputReq.rotate_right) {
        attemptPieceRotation(Direction_RIGHT);
        ctx.inputReq.rotate_right = false;
    }

    if (ctx.inputReq.move_left_pressed) {
        attemptPieceMove(Direction_LEFT);
        ctx.inputReq.move_left_pressed = false;
    }
    if (ctx.inputReq.move_right_pressed) {
        attemptPieceMove(Direction_RIGHT);
        ctx.inputReq.move_right_pressed = false;
    }
    if (ctx.inputReq.fast_drop_pressed) {
        attemptPieceMove(Direction_DOWN);
        ctx.inputReq.fast_drop_pressed = false;
    }
    if (ctx.inputReq.instant_drop_pressed) {
        while (attemptPieceMove(Direction_DOWN)) {}
        ctx.inputReq.instant_drop_pressed = false;
    }
}

//
/* This function runs once at shutdown. */

void overlay_setFields(void) {
    overlay_reset();
    [[maybe_unused]] vec2        mpos = { ctx.inputReq.s_mpos.x, ctx.inputReq.s_mpos.y };
    [[maybe_unused]] vec2        s_mpos = screen_to_grid(mpos);
    [[maybe_unused]] const char* m1str = (ctx.inputReq.m1_pressed ? "DOWN" : "UP");
    OVERLAY_PRINTLN("mpos: %.4f, %.4f", mpos.x, mpos.y);
    //   OVERLAY_PRINTLN("m1: %s", m1str);
    //   OVERLAY_PRINTLN("spos: %.2f %.2f", vec2_unpack(ctx.input.s_mpos));
    //    OVERLAY_PRINTLN("gpos: %.2f %.2f", vec2_unpack(s_mpos));
    OVERLAY_PRINTLN("tick: %zu", gtx.tick);
    OVERLAY_PRINTLN("rotation: %s", Direction_tostr(gtx.activePiece.rotation));
    OVERLAY_PRINTLN("gpos: %.0f, %.0f", vec2_unpack(gtx.activePiece.g_pos));
    OVERLAY_PRINTLN("active: %d", gtx.isPieceActive);

    if (ctx.perf.show_perf_in_debug) {
        OVERLAY_PRINTLN("frametime: %.4lf", dbl_rb_avg(ctx.perf.ft_rb));
        // OVERLAY_PRINTLN("framecount: %ld", ctx.frame_count);
        OVERLAY_PRINTLN("fps: %.1lf", dbl_rb_avg(ctx.perf.fps_rb));
    }
}

int app_init(int argc, char* argv[]) {
    SDL_SetAppMetadata("cstris", "1.0", "lmeldrum.cstris");
    SDL_Window*   window = NULL;
    SDL_Renderer* renderer = NULL;
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("cstris - window", DEF_WIDTH, DEF_HEIGHT,
                                     SDL_WINDOW_TRANSPARENT, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(renderer, DEF_WIDTH, DEF_HEIGHT,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    game_start(window, renderer, DEF_WIDTH, DEF_HEIGHT);
    //    SDL_SetRenderVSync(ctx.renderer, 1);
    return SDL_APP_CONTINUE; /* carry on with the program! */
}

int app_exit(int exitCode) {
    LOGLN("Exiting. (%d),", exitCode);
    // we must now deinitialize everyything manually i think
    return exitCode;
}

int main(int argc, char** argv) {
    app_init(argc, argv);
    SDL_Event event = {};
    i64       dt_ms = {};

    for (int x = PLAYFIELD_XMIN; x < PLAYFIELD_XMAX; x++) {
        const int y = 5;
        placeBlock((vec2){ x, y }, red_bands);
    }
    while (ctx.running) {
        profileFrameStats(&dt_ms);
        while (SDL_PollEvent(&event)) {
            if (app_handleEvent(&ctx.inputReq, &event) == Event_Exit) {
                ctx.running = false;
                goto EXIT_LOOP;
            }
        }

        app_update(dt_ms, &gtx);
        app_draw();

        ctx.perf.ms_lastframe = ctx.perf.ms_thisframe;
    }

EXIT_LOOP:
    return app_exit(EXIT_SUCCESS);
}
