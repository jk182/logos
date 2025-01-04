#pragma once

#include "board.h"

enum {
	PAWN_VALUE = 100,
	KNIGHT_VALUE = 320,
	BISHOP_VALUE = 330,
	ROOK_VALUE = 500,
	QUEEN_VALUE = 900
};

enum {
	MATE_SCORE = 100000
};

int evaluate(Board *board);
