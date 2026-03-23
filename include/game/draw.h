#pragma once
#include "piecedata.h"
#include "sdlwrappers.h"
#include "sugar.h"

// src/game/draw.c
// void g_drawPiece(vec2 g_topLeftPos, PieceType T);
void g_drawPiece(vec2 g_topLeftPos, PieceType T, int temp_rot);
void drawGrid(vec2 tl, vec2 br);
void g_drawBlock(vec2 gridPos, double len, const ColorScheme piece_colors);
void drawWalls(void);

static inline vec2 snapToGrid(const vec2 s_pos) {
    i64 ipx = round(s_pos.x) / BLOCK_SZ;
    i64 ipy = round(s_pos.y) / BLOCK_SZ;
    return (vec2){ ipx * BLOCK_SZ, ipy * BLOCK_SZ };
}

static const i64 PLAYFIELD_XMIN = 5;
static const i64 PLAYFIELD_YMIN = 0;
static const i64 PLAYFIELD_WIDTH = 10;
static const i64 PLAYFIELD_HEIGHT = 20;
static const i64 PLAYFIELD_XMAX = PLAYFIELD_XMIN + PLAYFIELD_WIDTH;
static const i64 PLAYFIELD_YMAX = PLAYFIELD_YMIN + PLAYFIELD_HEIGHT;

static const i64 OFFSET_LEN = 4;
