#include "game/draw.h"
#include "game/internal_draw.h"  // src/game/internal_draw.c
#include "gamecontext.h"
#include "logger.h"
#include "piecedata.h"
#include "primitives.h"
#include "sugar.h"
#include "timing.h"
#include "types.h"
#include <math.h>

void g_showPieceRotationOrigin(const vec2 g_topLeftPos, PieceType T);
void g_showPieceBoundingBox(const vec2 g_topLeftPos, PieceType T) {
    PieceData* pd = get_piece_data(T);
    g_drawBlockSpecial(g_topLeftPos, pd->l_boundingBox.width * BLOCK_SZ, WHITE, 0.1);
}

void g_drawBlock(vec2 g_pos, double extent, const ColorScheme colorscheme) {
    vec2 s_topLeftPos = grid_to_screen(g_pos);
    s_drawBlock(s_topLeftPos, extent, colorscheme);
}

void drawPlayField(void) {
    drawGrid((vec2){ PLAYFIELD_XMIN, PLAYFIELD_YMIN }, (vec2){ PLAYFIELD_XMAX, PLAYFIELD_YMAX });
    for (int y = PLAYFIELD_YMIN; y < PLAYFIELD_YMAX; y++) {
        for (int x = PLAYFIELD_XMIN; x < PLAYFIELD_XMAX; x++) {
            int idx = get_grid_idx(x, y);
            if (gtx.grid[idx].occupied) {
                g_drawBlock((vec2){ x, y }, BLOCK_SZ, gtx.grid[idx].colorscheme);
            }
        }
    }
}
void g_drawPiece(vec2 g_topLeftPos, PieceType T, int temp_rot) {
    PieceData* piece = get_piece_data(T);

    g_showPieceBoundingBox(g_topLeftPos, T);
    for (int i = 0; i < OFFSET_LEN; i++) {
        size_t idx = GET_BLOCK_IDX(temp_rot, i);
        vec2   g_blockPos = local_to_grid(piece->l_blockOffsets[idx], g_topLeftPos);
        vec2   s_blockPos = grid_to_screen(g_blockPos);
        //        LOGLN("Block @[%.2f,%.2f]", g_blockPos.x, g_blockPos.y);
        s_drawBlock(s_blockPos, BLOCK_SZ, piece->colorscheme);
    }
    g_showPieceRotationOrigin(g_topLeftPos, T);
}
void g_showPieceRotationOrigin(const vec2 g_topLeftPos, PieceType T) {
    PieceData* pd = get_piece_data(T);
    vec2       g_rot_origin = vec2_add(g_topLeftPos, pd->l_rot_origin);

    g_drawBlockSpecial(g_rot_origin, BLOCK_SZ, WHITE, 0.5);
}
void s_drawPieceSpecial(vec2 s_pos, const vec2* offsets, const ColorScheme colorscheme,
                        float opacity) {
    for (int i = 0; i < OFFSET_LEN; i++) {
        vec2 offset = offsets[i];
        s_drawBlockSpecial((vec2){ s_pos.x + BLOCK_SZ * offset.x, s_pos.y + BLOCK_SZ * offset.y },
                           BLOCK_SZ, colorscheme, opacity);
    }
}

void drawGrid(vec2 g_min, vec2 g_max) {
    u64 width = g_max.x - g_min.x;
    u64 height = g_max.y - g_min.y;

    SDL_FColor previous_color;
    getcolor(previous_color);
    setcolor(GREY(128));

    {
        // horizontal lines
        vec2 start = { g_min.x, g_min.y };
        vec2 end = { g_min.x + width, g_min.y };
        for (int y = 1; y < height; y++) {
            start.y += 1;
            end.y += 1;
            vec2 s_start = vec2_mul(start, BLOCK_SZ);
            vec2 s_end = vec2_mul(end, BLOCK_SZ);
            DrawLine(s_start, s_end);
        }
    }
    {
        // vertical lines
        vec2 start = { g_min.x, g_min.y };
        vec2 end = { g_min.x, g_min.y + height };
        for (int x = 1; x < width; x++) {
            start.x += 1;
            end.x += 1;
            vec2 s_start = vec2_mul(start, BLOCK_SZ);
            vec2 s_end = vec2_mul(end, BLOCK_SZ);
            DrawLine(s_start, s_end);
        }
    }
    setcolor(previous_color);
}

void drawWalls(void) {
    return;
    PieceData* wall_piece = get_piece_data(PieceType_Wall_Piece);
    // draw floor
    {
        vec2 o = { 0, ctx.rows - PLAYFIELD_YMIN };
        for (int x = 0; x < DEF_COLS; x++) {
            for (int y = 0; y < PLAYFIELD_YMIN; y++) {
                vec2 pos = { o.x + x, o.y + y };
                pos = vec2_mul(pos, BLOCK_SZ);
                s_drawBlock(pos, BLOCK_SZ, wall_piece->colorscheme);
            }
        }
    }
    {
        // draw left wall
        vec2 g_leftWall = { 0, 0 };
        vec2 g_rightWall = { ctx.cols - PLAYFIELD_XMIN, 0 };
        vec2 s_leftWall = grid_to_screen(g_leftWall);
        vec2 s_rightWall = grid_to_screen(g_rightWall);
        for (int x = 0; x < PLAYFIELD_XMIN; x += BLOCK_SZ) {
            for (int y = 0; y < DEF_ROWS; y += BLOCK_SZ) {
                vec2 s_leftWall = { s_leftWall.x + x, s_leftWall.y + y };
                vec2 s_rightWall = { PLAYFIELD_WIDTH + s_rightWall.x + x, s_rightWall.y + y };
                s_drawBlock(s_leftWall, BLOCK_SZ, wall_piece->colorscheme);
                s_drawBlock(s_rightWall, BLOCK_SZ, wall_piece->colorscheme);
            }
        }
    }
}
