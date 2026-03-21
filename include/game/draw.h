#pragma once
#include "piecedata.h"
#include "sdlwrappers.h"
#include "sugar.h"

// src/game/draw.c
// void g_drawPiece(vec2 g_topLeftPos, PieceType T);
void g_drawPiece(vec2 g_topLeftPos, PieceType T, int temp_rot);
void drawGrid(vec2 tl, vec2 br);
void drawWalls(void);

static inline vec2 snapToGrid(const vec2 s_pos) {
    i64 ipx = round(s_pos.x) / BLOCK_SZ;
    i64 ipy = round(s_pos.y) / BLOCK_SZ;
    return (vec2){ ipx * BLOCK_SZ, ipy * BLOCK_SZ };
}

static const i64 PLAYFIELD_X0 = 5;
static const i64 PLAYFIELD_GXOFFSET = 5;
static const i64 PLAYFIELD_Y0 = 0;
static const i64 SIDE_WALL_WIDTH = 5;
static const i64 PLAYFIELD_WIDTH = 10;
static const i64 PLAYFIELD_HEIGHT = 20;
static const i64 PLAYFIELD_X1 = PLAYFIELD_X0 + PLAYFIELD_WIDTH;
static const i64 PLAYFIELD_Y1 = PLAYFIELD_Y0 + PLAYFIELD_HEIGHT;
static const i64 PLAYFIELD_GYOFFSET = DEF_ROWS - PLAYFIELD_HEIGHT;

#define OFFSET_LEN 4  // number of blocks per piece
