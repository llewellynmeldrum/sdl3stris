#include "logger.h"
#include "macromagic.h"
#include "sugar.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "piecedata.h"
// The rotations are based on this [website](https://tetris.wiki/images/3/3d/SRS-pieces.png)

static const vec2 I_Piece_BoundingBox = { 4, 4 };
static const vec2 I_Piece_RotationOffsets[ROTATION_COUNT * BLOCKS_PER_PIECE] = {
    // horizontal, 0 degrees
    { 0, 1 },
    { 1, 1 },
    { 2, 1 },
    { 3, 1 },
    // vertical, 90 degrees
    { 2, 0 },
    { 2, 1 },
    { 2, 2 },
    { 2, 3 },
    // horizontal, 180 degrees
    { 0, 2 },
    { 1, 2 },
    { 2, 2 },
    { 3, 2 },

    // vertical, 270 degrees
    { 1, 0 },
    { 1, 1 },
    { 1, 2 },
    { 1, 3 },
};
static const vec2 J_Piece_BoundingBox = { 3, 3 };
static const vec2 J_Piece_RotationOffsets[ROTATION_COUNT * BLOCKS_PER_PIECE] = {
    // horizontal, 0 degrees
    { 0, 0 },
    { 0, 1 },
    { 1, 1 },
    { 2, 1 },
    // vertical, 90 degrees
    { 2, 0 },
    { 1, 0 },
    { 1, 1 },
    { 1, 2 },
    // horizontal, 180 degrees
    { 0, 1 },
    { 1, 1 },
    { 2, 1 },
    { 2, 2 },
    // vertical, 270 degrees
    { 1, 0 },
    { 1, 1 },
    { 1, 2 },
    { 0, 2 },
};

static const vec2 L_Piece_BoundingBox = { 3, 3 };
static const vec2 L_Piece_RotationOffsets[ROTATION_COUNT * BLOCKS_PER_PIECE] = {
    // horizontal, 0 degrees
    { 0, 1 },
    { 1, 1 },
    { 2, 1 },
    { 2, 0 },
    // vertical, 90 degrees
    { 1, 0 },
    { 1, 1 },
    { 1, 2 },
    { 2, 2 },
    // horizontal, 180 degrees
    { 0, 2 },
    { 0, 1 },
    { 1, 1 },
    { 2, 1 },

    // vertical, 270 degrees
    { 0, 0 },
    { 1, 0 },
    { 1, 1 },
    { 1, 2 },

};
static const vec2 O_Piece_BoundingBox = { 4, 3 };
static const vec2 O_Piece_RotationOffsets[ROTATION_COUNT * BLOCKS_PER_PIECE] = {
    // horizontal, 0 degrees
    { 1, 0 },
    { 2, 0 },
    { 1, 1 },
    { 2, 1 },
    // vertical, 90 degrees
    { 1, 0 },
    { 2, 0 },
    { 1, 1 },
    { 2, 1 },
    // horizontal, 180 degrees
    { 1, 0 },
    { 2, 0 },
    { 1, 1 },
    { 2, 1 },

    // vertical, 270 degrees
    { 1, 0 },
    { 2, 0 },
    { 1, 1 },
    { 2, 1 },

};
static const vec2 S_Piece_BoundingBox = { 3, 3 };
static const vec2 S_Piece_RotationOffsets[ROTATION_COUNT * BLOCKS_PER_PIECE] = {
    // horizontal 0 degrees
    { 0, 1 },
    { 1, 1 },
    { 1, 0 },
    { 2, 0 },
    // vertical 90 degrees
    { 1, 0 },
    { 1, 1 },
    { 2, 1 },
    { 2, 2 },
    // horizontal 180 degrees
    { 0, 2 },
    { 1, 2 },
    { 1, 1 },
    { 2, 1 },
    // vertical 270 degrees
    { 0, 0 },
    { 0, 1 },
    { 1, 1 },
    { 1, 2 },
};
static const vec2 T_Piece_BoundingBox = { 3, 3 };
static const vec2 T_Piece_RotationOffsets[ROTATION_COUNT * BLOCKS_PER_PIECE] = {
    // horizontal 0 degrees
    { 0, 1 },
    { 1, 0 },
    { 1, 1 },
    { 2, 1 },
    // vertical 90 degrees
    { 1, 0 },
    { 1, 1 },
    { 1, 2 },
    { 2, 1 },
    // horizontal 180 degrees
    { 0, 1 },
    { 1, 1 },
    { 2, 1 },
    { 1, 2 },
    // vertical 270 degrees
    { 1, 1 },
    { 1, 2 },
    { 1, 0 },
    { 0, 1 },
};
static const vec2 Z_Piece_BoundingBox = { 3, 3 };
static const vec2 Z_Piece_RotationOffsets[ROTATION_COUNT * BLOCKS_PER_PIECE] = {
    // horizontal 0 degrees
    { 0, 0 },
    { 1, 0 },
    { 1, 1 },
    { 2, 1 },
    // vertical 90 degrees
    { 1, 2 },
    { 1, 1 },
    { 2, 1 },
    { 2, 0 },
    // horizontal 180 degrees
    { 0, 1 },
    { 1, 1 },
    { 1, 2 },
    { 2, 2 },
    // vertical 270 degrees
    { 1, 1 },
    { 1, 0 },
    { 0, 2 },
    { 0, 1 },
};
#define DECL_PIECE(T, color, ox, oy)                                                               \
    static const PieceData PieceData_##T = { .type = PieceType_##T,                                \
                                             .colorscheme = color,                                 \
                                             .rotation_count = 4,                                  \
                                             .l_rot_origin = { ox, oy },                           \
                                             .l_boundingBox = CONCAT(T, _BoundingBox),             \
                                             .l_blockOffsets = CONCAT(T, _RotationOffsets) }

const vec2 o = { 0, 0 };

DECL_PIECE(I_Piece, cyan, 1.5, 1.5);
DECL_PIECE(J_Piece, blue, 1, 1);
DECL_PIECE(L_Piece, orange, 1, 1);
DECL_PIECE(O_Piece, yellow, 1.5, 0.5);
DECL_PIECE(S_Piece, green, 1, 1);
DECL_PIECE(T_Piece, purple, 1, 1);  // should make a cross -|-  when rotated fast
DECL_PIECE(Z_Piece, red, 1, 1);

#define piece_data_TOSTR(expr) #expr
const char* pttostr(PieceType T) {
    switch (T) {
    case PieceType_I_Piece:
        return piece_data_TOSTR(PieceData_I_Piece);
        break;
    case PieceType_J_Piece:
        return piece_data_TOSTR(PieceData_J_Piece);
        break;
    case PieceType_L_Piece:
        return piece_data_TOSTR(PieceData_L_Piece);
        break;
    case PieceType_O_Piece:
        return piece_data_TOSTR(PieceData_O_Piece);
        break;
    case PieceType_S_Piece:
        return piece_data_TOSTR(PieceData_S_Piece);
        break;
    case PieceType_T_Piece:
        return piece_data_TOSTR(PieceData_T_Piece);
        break;
    case PieceType_Z_Piece:
        return piece_data_TOSTR(PieceData_Z_Piece);
        break;
    default:
        LOGERR("Unimplemented, expect a segfault");
        return NULL;
        break;
    }
}
const PieceData* get_piece_data(PieceType T) {
    switch (T) {
    case PieceType_I_Piece:
        return &PieceData_I_Piece;
        break;
    case PieceType_J_Piece:
        return &PieceData_J_Piece;
        break;
    case PieceType_L_Piece:
        return &PieceData_L_Piece;
        break;
    case PieceType_O_Piece:
        return &PieceData_O_Piece;
        break;
    case PieceType_S_Piece:
        return &PieceData_S_Piece;
        break;
    case PieceType_T_Piece:
        return &PieceData_T_Piece;
        break;
    case PieceType_Z_Piece:
        return &PieceData_Z_Piece;
        break;
    default:
        LOGERR("Unimplemented, expect a segfault");
        return NULL;
        break;
    }
}
// most of these offsets are derived from [this website](https://tetris.wiki/Super_Rotation_System)
