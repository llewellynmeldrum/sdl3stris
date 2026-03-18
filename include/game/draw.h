#pragma once
#include "piecedata.h"
#include "sdlwrappers.h"

void drawPieceOrigin(vec2 g_originPos, PieceType T);
void drawPiece(vec2 g_topLeftPos, PieceType T);

void             s_drawBlock(vec2 screenPos, double len, const SDL_FColor piece_colors[4]);
void             drawBlockOutlineV(vec2 screenPos, double len, const SDL_FColor piece_colors[4]);
void             draw_block(double tlx, double tly, double len, const SDL_FColor piece_colors[4]);
void             drawGrid(vec2 tl, vec2 br);
void             drawWalls(void);
void             drawTetro(vec2 s_pos, const vec2* offsets, const SDL_FColor* colors);
vec2             snapToGrid(const vec2 screenPos);
static const i64 PLAYFIELD_X0 = 5;
static const i64 PLAYFIELD_GXOFFSET = 5;
static const i64 PLAYFIELD_Y0 = 0;
static const i64 SIDE_WALL_WIDTH = 5;
static const i64 PLAYFIELD_WIDTH = 10;
static const i64 PLAYFIELD_HEIGHT = 20;
static const i64 PLAYFIELD_X1 = PLAYFIELD_X0 + PLAYFIELD_WIDTH;
static const i64 PLAYFIELD_Y1 = PLAYFIELD_Y0 + PLAYFIELD_HEIGHT;
static const i64 PLAYFIELD_GYOFFSET = DEF_ROWS - PLAYFIELD_HEIGHT;
