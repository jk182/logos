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

	// Making sure that pieces of the same colour don't occupy the same square
	movesBB &= piece==W_KING ? ~whitePieces(board) : ~blackPieces(board);

	int square = popLSB(&king);
	int endSquare;
	uint16_t move;
	while (movesBB) {
		endSquare = popLSB(&movesBB);
		move = makeMove(square, endSquare);
		*(moves++) = move;
	}
	return moves;
}


uint16_t* generateKnightMoves(Board *board, uint16_t *moves) {
	int piece = board->turn ? W_KNIGHT : B_KNIGHT;
	uint64_t knightBB = board->pieces[piece];
	uint64_t movesBB = 0ull;
	int square;
	int endSquare;
	uint64_t currKnight;
	uint16_t move;
	
	while (knightBB) {
		square = popLSB(&knightBB);
		currKnight = 1ull << square;

		movesBB ^= (currKnight & (~(RANK_1|RANK_2|H_FILE))) >> 17;
		movesBB ^= (currKnight & (~(RANK_1|RANK_2|A_FILE))) >> 15;

		movesBB ^= (currKnight & (~(RANK_1|G_FILE|H_FILE))) >> 10;
		movesBB ^= (currKnight & (~(RANK_1|A_FILE|B_FILE))) >> 6;

		movesBB ^= (currKnight & (~(RANK_8|G_FILE|H_FILE))) << 6;
		movesBB ^= (currKnight & (~(RANK_8|A_FILE|B_FILE))) << 10;

		movesBB ^= (currKnight & (~(RANK_7|RANK_8|H_FILE))) << 15;
		movesBB ^= (currKnight & (~(RANK_7|RANK_8|A_FILE))) << 17;
	
		movesBB &= piece==W_KNIGHT ? ~whitePieces(board) : ~blackPieces(board);

		while (movesBB) {
			endSquare = popLSB(&movesBB);
			move = makeMove(square, endSquare);
			*(moves++) = move;
		}
	}
	return moves;
}
