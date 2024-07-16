#include <stdint.h>
#include <iostream>

#include "move.h"
#include "movegen.h"
#include "board.h"
#include "types.h"
#include "bitboard.h"


uint16_t* generatePawnMoves(Board *board, uint16_t *moves) {
	uint64_t wPieces = whitePieces(board);
	uint64_t bPieces = blackPieces(board);
	uint64_t movesBB = 0ull;
	uint64_t pawnBB;
	int endSquare;
	uint16_t move;
	uint64_t epBB;

	if (board->turn) {
		pawnBB = board->pieces[W_PAWN];
		
		// Double pawn moves
		movesBB ^= (pawnBB & RANK_2) << 16;
		movesBB &= ~(wPieces | bPieces);
		// Make sure that pawns don't jump over pieces on the third rank
		movesBB &= ~((wPieces | bPieces) & RANK_3) << 8;
		while (movesBB) {
			endSquare = popLSB(&movesBB);
			move = makeMove(endSquare-16, endSquare);
			*(moves++) = move;
		}

		// Single pawn moves
		movesBB ^= pawnBB << 8;
		movesBB &= ~(wPieces | bPieces);
		while (movesBB) {
			endSquare = popLSB(&movesBB);
			if (endSquare >= 56) {
				// Adding the promoting pieces
				// N=1, B=2, R=3, Q=4
				for (int p = 1; p <= 4; p++) {
					move = makeMove(endSquare-8, endSquare, p);
					*(moves++) = move;
				}
			} else {
				move = makeMove(endSquare-8, endSquare);
				*(moves++) = move;
			}
		}

		// Pawn captures
		epBB = board->epSquare > -1 ? 1ull << board->epSquare : 0ull;
		movesBB ^= (pawnBB & (~H_FILE)) << 7;
		movesBB &= (bPieces |epBB);
		while (movesBB) {
			endSquare = popLSB(&movesBB);
			if (endSquare >= 56) {
				for (int p = 1; p <= 4; p++) {
					move = makeMove(endSquare-7, endSquare, p);
					*(moves++) = move;
				}
			} else {
				move = makeMove(endSquare-7, endSquare);
				*(moves++) = move;
			}
		}
		movesBB ^= (pawnBB & (~A_FILE)) << 9;
		movesBB &= (bPieces | epBB);
		while (movesBB) {
			endSquare = popLSB(&movesBB);
			if (endSquare >= 56) {
				for (int p = 1; p <= 4; p++) {
					move = makeMove(endSquare-9, endSquare, p);
					*(moves++) = move;
				}
			} else {
				move = makeMove(endSquare-9, endSquare);
				*(moves++) = move;
			}
		}
	} else {
		pawnBB = board->pieces[B_PAWN];
		
		// Double pawn moves
		movesBB ^= (pawnBB & RANK_7) >> 16;
		movesBB &= ~(wPieces | bPieces);
		// Make sure that pawns don't jump over pieces on the third rank
		movesBB &= ~((wPieces | bPieces) & RANK_6) >> 8;
		while (movesBB) {
			endSquare = popLSB(&movesBB);
			move = makeMove(endSquare+16, endSquare);
			*(moves++) = move;
		}

		// Single pawn moves
		movesBB ^= pawnBB >> 8;
		movesBB &= ~(wPieces | bPieces);
		while (movesBB) {
			endSquare = popLSB(&movesBB);
			if (endSquare <= 7) {
				// Adding the promoting pieces
				// N=1, B=2, R=3, Q=4
				for (int p = 1; p <= 4; p++) {
					move = makeMove(endSquare+8, endSquare, p);
					*(moves++) = move;
				}
			} else {
				move = makeMove(endSquare+8, endSquare);
				*(moves++) = move;
			}
		}

		// Pawn captures
		epBB = board->epSquare > -1 ? 1ull << board->epSquare : 0ull;
		movesBB ^= (pawnBB & (~H_FILE)) >> 9;
		movesBB &= (wPieces | epBB);
		while (movesBB) {
			endSquare = popLSB(&movesBB);
			if (endSquare <= 7) {
				for (int p = 1; p <= 4; p++) {
					move = makeMove(endSquare+9, endSquare, p);
					*(moves++) = move;
				}
			} else {
				move = makeMove(endSquare+9, endSquare);
				*(moves++) = move;
			}
		}
		movesBB ^= (pawnBB & (~A_FILE)) >> 7;
		movesBB &= (wPieces | epBB);
		while (movesBB) {
			endSquare = popLSB(&movesBB);
			if (endSquare <= 7) {
				for (int p = 1; p <= 4; p++) {
					move = makeMove(endSquare+7, endSquare, p);
					*(moves++) = move;
				}
			} else {
				move = makeMove(endSquare+7, endSquare);
				*(moves++) = move;
			}
		}
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
