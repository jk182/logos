#include <stdint.h>
#include <iostream>

#include "move.h"
#include "movegen.h"
#include "board.h"
#include "types.h"
#include "bitboard.h"


uint16_t* generateKingMoves(Board *board, uint16_t *moves) {
	int piece = board->turn ? W_KING : B_KING;
	uint64_t king = board->pieces[piece];
	uint64_t movesBB = 0ull;

	movesBB ^= (king & (~A_FILE)) << 1;
	movesBB ^= (king & (~H_FILE)) >> 1;
	movesBB ^= (king & (~RANK_1)) >> 8;
	movesBB ^= (king & (~RANK_8)) << 8;

	movesBB ^= (king & (~(A_FILE|RANK_1))) >> 7;
	movesBB ^= (king & (~(A_FILE|RANK_8))) << 9;
	movesBB ^= (king & (~(H_FILE|RANK_1))) >> 9;
	movesBB ^= (king & (~(H_FILE|RANK_8))) << 7;

	printBitboard(movesBB);	
	int square = popLSB(&king);
	while (movesBB) {
		int endSquare = popLSB(&movesBB);
		uint16_t move = makeMove(square, endSquare);
		*(moves++) = move;
	}
	return moves;
}
