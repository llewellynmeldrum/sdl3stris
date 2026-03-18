// #include "piecedata.h"
#include "piecedata.h"
#include "sdlwrappers.h"
#include "sugar.h"

static const vec2 I_Piece_Offsets[4] = {
    { 0, 0 },
    { 0, 1 },
    { 0, 2 },
    { 0, 3 },
};
static const vec2 J_Piece_Offsets[] = {
    { 0, 0 },
    { 0, 1 },
    { 0, 2 },
    { -1, 2 },
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
#define CONCAT(a, b) a##b

#define DECL_PIECE(T, color, ox, oy)                                                               \
    static const PieceDef PieceDef_##T = { .type = PieceType_##T,                                  \
                                           .colors = color,                                        \
                                           .l_origin = { ox, oy },                                 \
                                           .offsets_len = ARRLEN(CONCAT(T, _Offsets)),             \
                                           .offsets = CONCAT(T, _Offsets) }

DECL_PIECE(Wall_Piece, grey, 0, 1);
DECL_PIECE(I_Piece, cyan, 0, 1);
DECL_PIECE(J_Piece, blue, 0, 1);
DECL_PIECE(L_Piece, orange, 0, 1);
DECL_PIECE(O_Piece, yellow, 0, 1);
DECL_PIECE(S_Piece, green, 0, 1);
DECL_PIECE(T_Piece, purple, 0, 1);
DECL_PIECE(Z_Piece, red, 0, 1);

// this is just used to make writing them easier, all are expanded
#define unwrap(T)                                                                                  \
    case PieceType_##T:                                                                            \
        return &(CONCAT(PieceDef_, T));                                                            \
        break

const PieceDef* get_piece_def(PieceType T) {
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
        fprintf(stderr, "Unimplemented, do I_PIECE. Expect a segfault");
        return NULL;
        break;
    }
}
