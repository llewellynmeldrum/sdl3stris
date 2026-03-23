#pragma once
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

typedef struct Piece {
    PieceType T;
    Direction rotation;
    vec2      g_pos;
} Piece;

typedef enum GameState {
    GameStateStartMenu,
    GameStatePaused,
    GameStateActive,
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
        .activePiece = { .g_pos = { 5, 0 } },
        .moveDownDelay = 20,  // how many ticks before a piece moves down by 1 automatically
        .isPieceActive = true,
        .g_dropPos = { (DEF_COLS / 2),
                       0 },  // this should be different, (or at least offset) for each piece.
        .grid = {},
    };
    LOGNOTICE("DROPIECE @%.f,%.f", vec2_unpack(gtx.activePiece.g_pos));
    LOGLN("testing");
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
