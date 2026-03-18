
#include "game/draw.h"
#include "piecedata.h"
#include "primitives.h"
#include "sugar.h"
#include "types.h"

// thinking about rotation, i observed that tetros seem to have an 'origin' point.
// This is a point within their own grid about which they rotate.
// For example, a I block rotates like this:
// |0█00|    |0000|
// |0▒00| -> |█▒██|
// |0█00|    |0000|
// |0█00|    |0000|
// The origin point' is highlighted as '▒'
// It might be wise to refer to tetrominoes/pieces by their origin point.
// Thus the draw commands should probably take this origin?
void g_drawBlock(vec2 gridPos, double len, const SDL_FColor piece_colors[4]);

// #define DRAW_BOUNDING_BOX
void drawTetroBounds(PieceType T, const vec2* offsets, const vec2 g_topLeftPos) {
#define OFFSET_LEN 4
    vec2 l_min = {};
    vec2 l_max = {};
    for (int i = 0; i < OFFSET_LEN; i++) {
        vec2 offset = offsets[i];
        l_min.x = fmin(l_min.x, offset.x);
        l_min.y = fmin(l_min.y, offset.y);
        l_max.x = fmax(l_max.x, offset.x);
        l_max.y = fmax(l_max.y, offset.y);
    }
    l_min.x--;
    l_min.y--;
    l_max.x++;
    l_max.y++;  // local to grid = g_pos+local

    vec2 g_min = vec2_add(l_min, g_topLeftPos);
    vec2 g_max = vec2_add(l_max, g_topLeftPos);

    if (g_min.x == g_max.x)
        g_max.x++;
    printf("%lf,%lf, %lf,%lf\n", vec2_unpack(g_min), vec2_unpack(g_max));
    PieceDef* pd = get_piece_def(T);
    vec2      g_origin = vec2_add(g_topLeftPos, pd->l_origin);
    //    drawGrid(g_min, g_max);
    g_drawBlock(g_max, BLOCK_SZ, grey);
    g_drawBlock(g_min, BLOCK_SZ, grey);
    g_drawBlock(g_origin, BLOCK_SZ, WHITE);
}
void drawPiece(vec2 g_topLeftPos, PieceType T) {
    PieceDef* piece = get_piece_def(T);
    vec2      s_pos = vec2_mul(g_topLeftPos, BLOCK_SZ);
    drawTetro(s_pos, piece->offsets, piece->colors);
    drawTetroBounds(T, piece->offsets, g_topLeftPos);
}
void drawPieceOrigin(vec2 g_originPos, PieceType T) {
    PieceDef* piece = get_piece_def(T);
    vec2      g_topLeftPos = vec2_sub(g_originPos, piece->l_origin);
    vec2      s_pos = vec2_mul(g_topLeftPos, BLOCK_SZ);
    drawTetro(s_pos, piece->offsets, piece->colors);
    drawTetroBounds(T, piece->offsets, g_topLeftPos);
}

vec2 snapToGrid(const vec2 screenPos) {
    i64 ipx = round(screenPos.x) / BLOCK_SZ;
    i64 ipy = round(screenPos.y) / BLOCK_SZ;
    return (vec2){ ipx * BLOCK_SZ, ipy * BLOCK_SZ };
}

// draw tetro expects screen position in terms of the 'top left' of the 'sprite'
void drawTetro(vec2 s_pos, const vec2* offsets, const SDL_FColor* colors) {
#define OFFSET_LEN 4
    for (int i = 0; i < OFFSET_LEN; i++) {
        vec2 offset = offsets[i];
        s_drawBlock((vec2){ s_pos.x + BLOCK_SZ * offset.x, s_pos.y + BLOCK_SZ * offset.y },
                    BLOCK_SZ, colors);
    }
}

void drawWalls() {
    return;
    PieceDef* wall_piece = get_piece_def(PieceType_Wall_Piece);
    // draw floor
    {
        vec2 o = { 0, ctx.rows - PLAYFIELD_GYOFFSET };
        for (int x = 0; x < DEF_COLS; x++) {
            for (int y = 0; y < PLAYFIELD_GYOFFSET; y++) {
                vec2 pos = { o.x + x, o.y + y };
                pos = vec2_mul(pos, BLOCK_SZ);
                s_drawBlock(pos, BLOCK_SZ, wall_piece->colors);
            }
        }
    }
    {
        // draw left wall
        vec2 o = { 0, 0 };
        for (int x = 0; x < PLAYFIELD_GXOFFSET; x++) {
            for (int y = 0; y < DEF_ROWS; y++) {
                vec2 pos = { o.x + x, o.y + y };
                pos = vec2_mul(pos, BLOCK_SZ);
                s_drawBlock(pos, BLOCK_SZ, wall_piece->colors);
            }
        }
    }
    {
        // draw right wall
        vec2 o = { ctx.cols - PLAYFIELD_GXOFFSET, 0 };
        for (int x = 0; x < PLAYFIELD_GXOFFSET; x++) {
            for (int y = 0; y < DEF_ROWS; y++) {
                vec2 pos = { o.x + x, o.y + y };
                pos = vec2_mul(pos, BLOCK_SZ);
                s_drawBlock(pos, BLOCK_SZ, wall_piece->colors);
            }
        }
    }
}
void s_drawBlock(vec2 screenPos, double len, const SDL_FColor piece_colors[4]) {
    double bord = len * 0.14;  // border width
    {
        // MAIN QUAD
        vec2 quad_verts[] = {
            screenPos,
            { screenPos.x + len, screenPos.y },
            { screenPos.x + len, screenPos.y + len },
            { screenPos.x, screenPos.y + len },
        };
        DrawQuad(quad_verts, piece_colors[2]);
    }
    {
        // LEFT EDGE
        vec2 o = { screenPos.x, screenPos.y };
        vec2 quad_verts[] = {
            o,
            { o.x + bord, o.y + bord },
            { o.x + bord, o.y + len - bord },
            { o.x, o.y + len },
        };
        DrawQuad(quad_verts, piece_colors[1]);
    }
    {
        // RIGHT EDGE
        vec2 o = { screenPos.x + len - bord, screenPos.y };
        vec2 quad_verts[] = {
            { o.x, o.y + bord },
            { o.x + bord, o.y },
            { o.x + bord, o.y + len },
            { o.x, o.y + len - bord },
        };
        DrawQuad(quad_verts, piece_colors[1]);
    }
    {
        // TOP EDGE
        vec2 o = { screenPos.x, screenPos.y };
        vec2 quad_verts[] = {
            { o.x, o.y },
            { o.x + len, o.y },
            { o.x + len - bord, o.y + bord },
            { o.x + bord, o.y + bord },
        };
        DrawQuad(quad_verts, piece_colors[3]);
    }
    {
        // BOT EDGE
        vec2 o = { screenPos.x, screenPos.y + len };
        vec2 quad_verts[] = {
            { o.x, o.y },
            { o.x + bord, o.y - bord },
            { o.x + len - bord, o.y - bord },
            { o.x + len, o.y },
        };
        DrawQuad(quad_verts, piece_colors[0]);
    }
}
void g_drawBlock(vec2 gridPos, double len, const SDL_FColor piece_colors[4]) {
    vec2   screenPos = vec2_mul(gridPos, BLOCK_SZ);
    double bord = len * 0.14;  // border width
    {
        // MAIN QUAD
        vec2 quad_verts[] = {
            screenPos,
            { screenPos.x + len, screenPos.y },
            { screenPos.x + len, screenPos.y + len },
            { screenPos.x, screenPos.y + len },
        };
        DrawQuad(quad_verts, piece_colors[2]);
    }
    {
        // LEFT EDGE
        vec2 o = { screenPos.x, screenPos.y };
        vec2 quad_verts[] = {
            o,
            { o.x + bord, o.y + bord },
            { o.x + bord, o.y + len - bord },
            { o.x, o.y + len },
        };
        DrawQuad(quad_verts, piece_colors[1]);
    }
    {
        // RIGHT EDGE
        vec2 o = { screenPos.x + len - bord, screenPos.y };
        vec2 quad_verts[] = {
            { o.x, o.y + bord },
            { o.x + bord, o.y },
            { o.x + bord, o.y + len },
            { o.x, o.y + len - bord },
        };
        DrawQuad(quad_verts, piece_colors[1]);
    }
    {
        // TOP EDGE
        vec2 o = { screenPos.x, screenPos.y };
        vec2 quad_verts[] = {
            { o.x, o.y },
            { o.x + len, o.y },
            { o.x + len - bord, o.y + bord },
            { o.x + bord, o.y + bord },
        };
        DrawQuad(quad_verts, piece_colors[3]);
    }
    {
        // BOT EDGE
        vec2 o = { screenPos.x, screenPos.y + len };
        vec2 quad_verts[] = {
            { o.x, o.y },
            { o.x + bord, o.y - bord },
            { o.x + len - bord, o.y - bord },
            { o.x + len, o.y },
        };
        DrawQuad(quad_verts, piece_colors[0]);
    }
}
void drawGrid(vec2 tl, vec2 br) {
    u64 width = br.x - tl.x;
    u64 height = br.y - tl.y;

    printf("%llu\n", height);
    printf("%lf,%lf, %lf,%lf\n", vec2_unpack(tl), vec2_unpack(br));
    printf("%llu\n", width);
    SDL_FColor previous_color;
    getcolor(previous_color);
    setcolor(gr(128));

    {
        // horizontal lines
        vec2 start = { tl.x, tl.y };
        vec2 end = { tl.x + width, tl.y };
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
        vec2 start = { tl.x, tl.y };
        vec2 end = { tl.x, tl.y + height };
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
