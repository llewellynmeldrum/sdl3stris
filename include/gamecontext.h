#pragma once
#include "game/draw.h"
#include "game/piecequeue.h"
#include "gridcell.h"
#include "logger.h"
#include "numeric.h"
#include "piecedata.h"
#include "sugar.h"
#include <stdlib.h>
#define DEF_CELLCOUNT (int)(DEF_COLS * DEF_ROWS)
#define DROPTIMER_INITIAL 10

static inline PieceType random_piece_type() {
    return (PieceType)urand(PieceType_START, PieceType_COUNT);
}
static inline int get_grid_idx(size_t x, size_t y) {
    return y * DEF_ROWS + x;
}
static inline int get_grid_idxv(vec2 v) {
    int idx = v.y * DEF_ROWS + v.x;
    assert(idx < DEF_CELLCOUNT || "Attempted OOB access of grid.");
    return idx;
}

typedef struct Piece {
    PieceType T;
    Direction rotation;
    vec2      g_pos;
} Piece;

typedef enum GameState {
    GameStateStartMenu,
    GameStatePaused,
    GameStateActive,
    GameStateGameOver,
} GameState;

typedef struct GameContext {
    GameState  state;
    PieceQueue piecequeue;
    size_t     tick;  // gametick = 20 per second, gametick = (ms_since_start / 1000)*20
    GridCell   grid[DEF_CELLCOUNT];
    double     droptimer;
    size_t     droptimer_current;
    Piece      activePiece;
    size_t     moveDownDelay;
    vec2       g_dropPos;
    bool       isPieceActive;
} GameContext;

static inline vec2 get_initial_position(PieceType T) {
    vec2 g_spawn = { PLAYFIELD_XMIN + 3, 0 - 1 };
    switch (T) {
    case PieceType_I_Piece:
        g_spawn.y -= 1;
        break;
    default:
        break;
    }
    return g_spawn;
}

static inline Direction get_initial_rotation(PieceType T) {
    switch (T) {
    case PieceType_I_Piece:
        [[fallthrough]];
    case PieceType_Z_Piece:
        [[fallthrough]];
    case PieceType_S_Piece:
        return Direction_UP;

    case PieceType_J_Piece:
        [[fallthrough]];
    case PieceType_L_Piece:
        [[fallthrough]];
    case PieceType_T_Piece:
        return Direction_DOWN;
        break;
    default:
        return Direction_DOWN;  // idk
    }
}

static inline GameContext init_GameContext() {
    // 1. create piece_queue
    // 2. set gametick
    // 3. set gamestate
    GameContext gtx = (GameContext){
        .state = GameStateActive,
        .piecequeue = pq_create(),
        .tick = 0,
        .droptimer = DROPTIMER_INITIAL,  // ticks between piecePlace and newPieceSpawned
                                         // starts counting down when there is no piece in the air
        .activePiece = {},
        .moveDownDelay = 20,  // how many ticks before a piece moves down by 1 automatically
        .isPieceActive = true,
        .g_dropPos = { (DEF_COLS / 2),
                       0 },  // this should be different, (or at least offset) for each piece.
        .grid = {},
    };
    gtx.activePiece.T = random_piece_type();
    gtx.activePiece.g_pos = get_initial_position(gtx.activePiece.T);
    gtx.activePiece.rotation = get_initial_rotation(gtx.activePiece.T);
    LOGNOTICE("DROPIECE @%.f,%.f", vec2_unpack(gtx.activePiece.g_pos));
    for (int i = 0; i < ARRLEN(gtx.grid); i++) {
        gtx.grid[i] = (GridCell){
            .colorscheme = NULL,
            .occupied = false,
        };
    }
    for (int i = 0; i < ARRLEN(gtx.grid); i++) {
        gtx.grid[i] = (GridCell){
            .colorscheme = NULL,
            .occupied = false,
        };
    }
    srand(time(NULL));
    for (int i = 0; i < PQ_CAPACITY; i++) {
        pq_push(&gtx.piecequeue, random_piece_type());
        // LOGLN("%d", gtx.piecequeue.head->type);
    }
    return gtx;
}
extern GameContext gtx;
