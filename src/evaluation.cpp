#include "bitboard.h"
#include "board.h"
#include "evaluation.h"
#include "types.h"

#include <iostream>

const int PIECE_VALUES[6] = {100, 320, 330, 500, 900, 0};

const int PAWN_OFFSET[64] = {
	  0,  0,  0,  0,  0,  0,  0,  0,
	 50, 50, 50, 50, 50, 50, 50, 50,
	 10, 10, 20, 30, 30, 20, 10, 10,
	  5,  5, 10, 25, 25, 10,  5,  5,
	  0,  0,  0, 20, 20,  0,  0,  0,
	  5,  0,  0,  0,  0,-10,  0,  0,
	  5, 10, 10,-20,-20, 10, 10,  5,
	  0,  0,  0,  0,  0,  0,  0,  0 
};

const int KNIGHT_OFFSET[64] = {
	-50,-40,-30,-30,-30,-30,-40,-50,
	-40,-20,  0,  0,  0,  0,-20,-40,
	-30,  0, 10, 15, 15, 10,  0,-30,
	-30,  5, 15, 20, 20, 15,  5,-30,
	-30,  0, 15, 20, 20, 15,  0,-30,
	-30,  0, 10, 15, 15, 10,  0,-30,
	-40,-20,  0,  5,  5,  0,-20,-40,
	-50,-40,-30,-30,-30,-30,-40,-50
};

const int BISHOP_OFFSET[64] = {
	-20,-10,-10,-10,-10,-10,-10,-20,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-10,  0,  5, 10, 10,  5,  0,-10,
	-10,  5,  5, 10, 10,  5,  5,-10,
	-10,  0, 10, 10, 10, 10,  0,-10,
	-10, 10, 10, 10, 10, 10, 10,-10,
	-10,  5,  0,  0,  0,  0,  5,-10,
	-20,-10,-10,-10,-10,-10,-10,-20
};

const int ROOK_OFFSET[64] = {
	  0,  0,  0,  0,  0,  0,  0,  0,
	  5, 10, 10, 10, 10, 10, 10,  5,
	 -5,  0,  0,  0,  0,  0,  0, -5,
	 -5,  0,  0,  0,  0,  0,  0, -5,
	 -5,  0,  0,  0,  0,  0,  0, -5,
	 -5,  0,  0,  0,  0,  0,  0, -5,
	 -5,  0,  0,  0,  0,  0,  0, -5,
	  0,  0,  0,  5,  5,  0,  0,  0
};

const int QUEEN_OFFSET[64] = {
	-20,-10,-10, -5, -5,-10,-10,-20,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-10,  0,  5,  5,  5,  5,  0,-10,
	 -5,  0,  5,  5,  5,  5,  0, -5,
	 -5,  0,  5,  5,  5,  5,  0, -5,
	-10,  0,  5,  5,  5,  5,  0,-10,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-20,-10,-10, -5, -5,-10,-10,-20
};

const int KING_OFFSET[64] = {
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-20,-30,-30,-40,-40,-30,-30,-20,
	-10,-20,-20,-20,-20,-20,-20,-10,
	 20, 20,  0,  0,  0,  0, 20, 20,
	 20, 30, 10,  0,  0, 10, 30, 20
};

const int *PIECE_OFFSETS[6] = {
	PAWN_OFFSET, KNIGHT_OFFSET, BISHOP_OFFSET, ROOK_OFFSET, QUEEN_OFFSET, KING_OFFSET
};


int countMaterial(Board *board) {
	int material = 0;
	int currMaterial;
	uint64_t bb;
	int square;
	
	for (int p = 0; p < PIECES; p++) {
		currMaterial = 0;
		bb = board->pieces[p];
		square = popLSB(&bb);
		while (square >= 0) {
		       	if (p <= W_KING) {
				currMaterial += PIECE_VALUES[p%(PIECES/2)] + PIECE_OFFSETS[p%(PIECES/2)][63-square];
			} else {
				currMaterial -= PIECE_VALUES[p%(PIECES/2)] + PIECE_OFFSETS[p%(PIECES/2)][square];
			}
			square = popLSB(&bb);
		}
		material += currMaterial;
	}
	return material;
}


int evaluate(Board *board) {
	if (isDraw(board)) {
		return 0;
	}
	if (isCheckmate(board)) {
		if (board->turn) {
			return -MATE_SCORE;
		} else {
			return MATE_SCORE;
		}
	}
	int material = countMaterial(board);
	return material;
}
