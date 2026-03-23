#include "vec2.h"
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
void          set_debug_overlay(void);
static size_t tick_last_frame;

// clang-format off -> really doesnt like xmacros
#define Direction__LIST                                                                            \
    X(Direction_UP)                                                                                \
    X(Direction_LEFT)                                                                              \
    X(Direction_DOWN)                                                                              \
    X(Direction_RIGHT)

#define X(dir) dir,
typedef enum Direction { Direction__LIST Direction_COUNT } Direction;
#undef X

#define X(dir) #dir,
const char* Direction_str[Direction_COUNT] = {
    Direction__LIST  //
};
#undef X
// clang-format on -> really doesnt like xmacros

typedef struct Piece {
    PieceType T;
    Direction rotation;
    vec2      g_pos;
} Piece;

typedef struct Cell {
    bool        occupied;
    ColorScheme colorscheme;
} Cell;

typedef struct GridState {
} GridState;

typedef enum GameState {
    GameStateStartMenu,
    GameStatePaused,
    GameStateActive,
} GameState;

typedef struct GameContext {
    GameState  state;
    PieceQueue piecequeue;
    size_t     tick;  // gametick = 20 per second, gametick = (ms_since_start / 1000)*20
    Cell       grid[DEF_CELLCOUNT];
    double     droptimer;
    Piece      activePiece;
    size_t     dropDelay;
    vec2       g_dropPos;
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
        .droptimer = 5000.0,  // ms between piecePlace and newPieceSpawned
                              // starts counting down when there is no piece in the air
        .activePiece = { .g_pos = { 5, 0 } },
        .dropDelay = 20,  // how many ticks inbetween a drop
        .g_dropPos = { 0, DEF_COLS / 2 },
        .grid = {}
    };
    srand(time(NULL));
    for (int i = 0; i < PQ_CAPACITY; i++) {
        pq_push(&gtx.piecequeue, random_piece());
        // LOGLN("%d", gtx.piecequeue.head->type);
    }
    return gtx;
}
GameContext gtx;

static void start_game(SDL_Window* win, SDL_Renderer* rend, size_t w, size_t h) {
    tick_last_frame = 0;
    LOGLN("start_game(%p, %p, %zu, %zu)", win, rend, w, h);
    ctx = init_ctx(win, rend, w, h);
    gtx = init_GameContext();
}
static void restart_game(void) {
    start_game(ctx.window, ctx.renderer, ctx.w, ctx.h);
}
void profileStats(i64* res_param_dt_ms) {
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
    set_debug_overlay();

    *res_param_dt_ms = (i64)dt;
}

void drawPieceQueue(PieceQueue pq) {
    PieceQueueNode* cur = pq.head;
    vec2            g_queue_pos = { ctx.cols - PLAYFIELD_XMIN + 1, 2 };
    LOGERR("This function is commented out.");
    for (int i = 0; i < pq.size; i++) {
        PieceData* pd = get_piece_data(cur->type);
        g_drawPiece(g_queue_pos, cur->type, 0);
        g_queue_pos.y += 4;
        cur = cur->prev;  // moving down the queue towards the back
    }
}

#define DO_ON_KEYPRESS(pressed, action)                                                            \
    if (pressed) {                                                                                 \
        action;                                                                                    \
        pressed = false;                                                                           \
    }

#define constrain(expr, lo, hi) ((expr) < lo ? lo : (expr > hi ? hi : expr))
#define mod(val, modulus) (fabs(val) % modulus)

Cell* getCell(Cell* playfield, vec2 g_pos) {
    vec2   p_pos = { g_pos.x - PLAYFIELD_XMIN, g_pos.y - PLAYFIELD_YMIN };
    size_t idx = p_pos.y * DEF_COLS + p_pos.x;
    return &playfield[idx];
}

void setCell(Cell* playfield, vec2 g_pos, Cell val) {
    vec2   p_pos = { g_pos.x - PLAYFIELD_XMIN, g_pos.y - PLAYFIELD_YMIN };
    size_t idx = p_pos.y * DEF_COLS + p_pos.x;
    playfield[idx] = val;
}
// checks to see if any of the pieces block sit outside of the PLAYFIELD.
static bool isPieceInBounds(Piece p) {
    PieceData* pd = get_piece_data(p.T);
    vec2       g_topLeftPos = p.g_pos;
    for (int blk_idx = 0; blk_idx < BLOCKS_PER_PIECE; blk_idx++) {
        size_t idx = GET_BLOCK_IDX(gtx.activePiece.rotation, blk_idx);
        vec2   g_piecePos = local_to_grid(pd->l_blockOffsets[idx], g_topLeftPos);
        if (g_piecePos.x >= PLAYFIELD_XMAX ||  //
            g_piecePos.x < PLAYFIELD_XMIN ||   //
            g_piecePos.y >= PLAYFIELD_YMAX ||  //
            g_piecePos.y < PLAYFIELD_YMIN      //
        ) {
            LOGERR("%.f,%.f is out of bounds!", vec2_unpack(g_piecePos));
            return false;
        }
    }
    return true;
}

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
    PieceData* pd = get_piece_data(gtx.activePiece.T);
    vec2       g_topLeftPos = gtx.activePiece.g_pos;
    Direction  rotationBefore = gtx.activePiece.rotation;

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
        LOGFATAL("Rotation of %s is unsupported.", Direction_str[dir]);
        break;
    }
}
void attemptPieceMove(Direction dir) {
    PieceData* pd = get_piece_data(gtx.activePiece.T);
    vec2       g_topLeftPos = gtx.activePiece.g_pos;

    switch (dir) {
    case Direction_UP:
        LOGFATAL("Bro defies gravity");
        break;
    case Direction_DOWN:
        bool cellsBelowAreFree = true;
        // check for free space
        for (int blk_idx = 0; blk_idx < BLOCKS_PER_PIECE; blk_idx++) {
            size_t idx = GET_BLOCK_IDX(gtx.activePiece.rotation, blk_idx);
            vec2   g_piecePos = local_to_grid(pd->l_blockOffsets[idx], g_topLeftPos);
            Cell*  cell = getCell(gtx.grid, g_piecePos);

            if (cell->occupied || g_piecePos.y >= PLAYFIELD_YMAX) {
                cellsBelowAreFree = false;
                break;
            }
        }
        if (cellsBelowAreFree) {
            gtx.activePiece.g_pos.y++;
        } else {
            // PLACE_PIECE(gtx.activePiece);
            //      cells are made occupied, set to the color of the piece.
            //      active piece becomes null
        }

        break;

    case Direction_LEFT:
        gtx.activePiece.g_pos.x--;
        if (!isPieceInBounds(gtx.activePiece)) {
            gtx.activePiece.g_pos.x++;
        }
        break;

    case Direction_RIGHT:
        gtx.activePiece.g_pos.x++;
        if (!isPieceInBounds(gtx.activePiece)) {
            gtx.activePiece.g_pos.x--;
        }

        break;
    default:
        LOGFATAL("Unknown direction (%d) supplied.", dir);
        break;
    }
}
void updateGameContext(i64 dt_ms, GameContext* gtx) {
    // go through state, all that other shit and build new frames gamecontext based on inputs
    // handle input
    gtx->tick = (size_t)(ms_since_start() / (50.0));  // 20 tps
    // LOGEXPR(ms_since_start());
    if (gtx->state == GameStateActive) {
        if (gtx->tick != tick_last_frame) {
            if (gtx->tick % gtx->dropDelay == 0) {
                attemptPieceMove(Direction_DOWN);
            }
        }
    }

    if (ctx.input.rotate_left_pressed) {
        attemptPieceRotation(Direction_LEFT);
        ctx.input.rotate_left_pressed = false;
    }

    if (ctx.input.rotate_right_pressed) {
        attemptPieceRotation(Direction_RIGHT);
        ctx.input.rotate_right_pressed = false;
    }

    if (ctx.input.move_left_pressed) {
        attemptPieceMove(Direction_LEFT);
        ctx.input.move_left_pressed = false;
    }
    if (ctx.input.move_right_pressed) {
        attemptPieceMove(Direction_RIGHT);
        ctx.input.move_right_pressed = false;
    }

    DO_ON_KEYPRESS(ctx.input.fast_drop_pressed,
                   gtx->activePiece.g_pos.y =
                           constrain(gtx->activePiece.g_pos.y + 1, 0, PLAYFIELD_HEIGHT))

    tick_last_frame = gtx->tick;
}
SDL_AppResult SDL_AppIterate(void* _) {
    i64 dt_ms = {};
    profileStats(&dt_ms);
    updateGameContext(dt_ms, &gtx);

    setcolor(ctx.draw.clear_color);
    SDL_RenderClear(ctx.renderer);
    g_drawPiece(gtx.activePiece.g_pos, PieceType_I_Piece, gtx.activePiece.rotation);
    drawDebugOverlay(true);
    drawGrid((vec2){ PLAYFIELD_XMIN, PLAYFIELD_YMIN }, (vec2){ PLAYFIELD_XMAX, PLAYFIELD_YMAX });

    SDL_RenderPresent(ctx.renderer);
    ctx.perf.ms_lastframe = ctx.perf.ms_thisframe;
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppInit(void** _, int argc, char* argv[]) {
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

    start_game(window, renderer, DEF_WIDTH, DEF_HEIGHT);
    //    SDL_SetRenderVSync(ctx.renderer, 1);
    return SDL_APP_CONTINUE; /* carry on with the program! */
}

// clang-format off
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    (void)appstate;
    ctx.input.rotate_left_pressed = false;
    ctx.input.rotate_right_pressed = false;
    switch (event->type) {
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        ctx.input.m1_pressed = (event->button.button == 1) ? true : ctx.input.m1_pressed;
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
        case SDLK_S:        ctx.input.fast_drop_pressed = true;                             break;
        case SDLK_W:        ctx.input.instant_drop_pressed = true;                          break;
        case SDLK_A:        ctx.input.rotate_left_pressed = true;                           break;
        case SDLK_D:        ctx.input.rotate_right_pressed = true;                          break;
        case SDLK_LEFT:     ctx.input.move_left_pressed = true;                              break;
        case SDLK_RIGHT:    ctx.input.move_right_pressed = true;                             break;
        case SDLK_R:        restart_game();                                                 break;
        }
        break;

    case SDL_EVENT_KEY_UP:
        switch(event->key.key){
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
    [[maybe_unused]] vec2        mpos = { ctx.input.s_mpos.x, ctx.input.s_mpos.y };
    [[maybe_unused]] vec2        s_mpos = screen_to_grid(mpos);
    [[maybe_unused]] const char* m1str = (ctx.input.m1_pressed ? "DOWN" : "UP");
    OVERLAY_PRINTLN("mpos: %.4f, %.4f", mpos.x, mpos.y);
    //   OVERLAY_PRINTLN("m1: %s", m1str);
    //   OVERLAY_PRINTLN("spos: %.2f %.2f", vec2_unpack(ctx.input.s_mpos));
    //    OVERLAY_PRINTLN("gpos: %.2f %.2f", vec2_unpack(s_mpos));
    OVERLAY_PRINTLN("tick: %zu", gtx.tick);
    OVERLAY_PRINTLN("rotation: %s", Direction_str[gtx.activePiece.rotation]);
    OVERLAY_PRINTLN("gpos: %.0f, %.0f", vec2_unpack(gtx.activePiece.g_pos));

    if (ctx.perf.show_perf_in_debug) {
        OVERLAY_PRINTLN("frametime: %.4lf", dbl_rb_avg(ctx.perf.ft_rb));
        //        OVERLAY_PRINTLN("framecount: %ld", ctx.frame_count);
        OVERLAY_PRINTLN("fps: %.1lf", dbl_rb_avg(ctx.perf.fps_rb));
    }
}
