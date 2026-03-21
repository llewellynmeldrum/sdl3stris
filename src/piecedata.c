#include "piecedata.h"
#include "logger.h"
#include "macromagic.h"
#include "sugar.h"

static const vec2 I_Piece_BoundingBoxSize = { 4, 4 };
static const vec2 I_Piece_RotationOffsets[4][4] = {
    // horizontal, 0 degrees
    {
            { 0, 1 },
            { 1, 1 },
            { 2, 1 },
            { 3, 1 },
    },
    // vertical, 90 degrees
    {
            { 2, 0 },
            { 2, 1 },
            { 2, 2 },
            { 2, 3 },
    },
    // horizontal, 180 degrees
    {
            { 0, 2 },
            { 1, 2 },
            { 2, 2 },
            { 3, 2 },
    },

    // vertical, 270 degrees
    {
            { 1, 0 },
            { 1, 1 },
            { 1, 2 },
            { 1, 3 },
    },

};
static const vec2 J_Piece_BoundingBoxSize = { 3, 3 };
static const vec2 J_Piece_RotationOffsets[4][4] = {
    // horizontal, 0 degrees
    {
            { 0, 0 },
            { 0, 1 },
            { 1, 1 },
            { 2, 1 },
    },
    // vertical, 90 degrees
    {
            { 2, 0 },
            { 1, 0 },
            { 1, 1 },
            { 1, 2 },
    },
    // horizontal, 180 degrees NOTE: UP TO HERE
    {
            { 0, 2 },
            { 1, 2 },
            { 2, 2 },
            { 3, 2 },
    },

    // vertical, 270 degrees
    {
            { 1, 0 },
            { 1, 1 },
            { 1, 2 },
            { 1, 3 },
    },

};

static const vec2 I_Piece_Offsets[4] = {
    { 0, 0 },
    { 1, 0 },
    { 2, 0 },
    { 3, 0 },
};
static const vec2 J_Piece_Offsets[] = {
    { 0, 1 },
    { 1, 1 },
    { 2, 1 },
    { 2, 0 },
};

static const vec2 L_Piece_Offsets[] = {
    { 0, 0 },
    { 0, 1 },
    { 0, 2 },
    { 1, 2 },
};
static const vec2 O_Piece_Offsets[] = {
    { 0, 0 },
    { 0, 1 },
    { 1, 0 },
    { 1, 1 },
};
static const vec2 S_Piece_Offsets[] = {
    { 0, 1 },
    { 1, 1 },
    { 1, 0 },
    { 2, 0 },
};
static const vec2 T_Piece_Offsets[] = {
    { 0, 0 },
    { 1, 0 },
    { 2, 0 },
    { 1, 1 },
};
static const vec2 Z_Piece_Offsets[] = {
    { 0, 0 },
    { 1, 0 },
    { 1, 1 },
    { 2, 1 },
};
static const vec2 Wall_Piece_Offsets[1] = {
    { 0, 0 },
};

#define DECL_PIECE(T, color, ox, oy)                                                               \
    static const PieceData PieceData_##T = { .type = PieceType_##T,                                \
                                             .colorscheme = color,                                 \
                                             .l_rot_origin = { ox, oy },                           \
                                             .offsets_len = ARRLEN(CONCAT(T, _Offsets)),           \
                                             .l_blockOffsets = CONCAT(T, _Offsets) }

// clang-format off
const vec2 o = {1,0};
DECL_PIECE(Wall_Piece, grey, 0, 1);
DECL_PIECE(I_Piece, cyan,   o.x, o.y);
DECL_PIECE(J_Piece, blue,   o.x, o.y);
DECL_PIECE(L_Piece, orange, o.x, o.y);
DECL_PIECE(O_Piece, yellow, o.x, o.y);
DECL_PIECE(S_Piece, green,  o.x, o.y);
DECL_PIECE(T_Piece, purple, o.x, o.y); // should make a cross -|-  when rotated fast
DECL_PIECE(Z_Piece, red,    o.x, o.y);

// this is just used to make writing them easier, all are expanded
#define unwrap(T)                                                                                  \
    case PieceType_##T:                                                                            \
        return &(CONCAT(PieceData_, T));                                                           \
        break

const PieceData* get_piece_data(PieceType T) {
    switch (T) {
        unwrap(I_Piece);
        unwrap(Wall_Piece);
        unwrap(J_Piece);
        unwrap(L_Piece);
        unwrap(O_Piece);
        unwrap(S_Piece);
        unwrap(T_Piece);
        unwrap(Z_Piece);
    default:
        LOGERR("Unimplemented, expect a segfault");
        return NULL;
        break;
    }
}
// most of these offsets are derived from [this website](https://tetris.wiki/Super_Rotation_System)
