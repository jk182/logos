#include "attacks.h"
#include "bitboard.h"
#include "board.h"
#include "move.h"
#include "movegen.h"
#include "types.h"

#include <stdint.h>
#include <iostream>


uint16_t* generatePawnMoves(Board board, uint16_t *moves) {
	uint64_t wPieces = whitePieces(&board);
	uint64_t bPieces = blackPieces(&board);
	uint64_t movesBB = 0ull;
	uint64_t pawnBB;
	int endSquare;
	uint16_t move;
	uint64_t epBB;

	if (board.turn) {
		pawnBB = board.pieces[W_PAWN];
		
		// Double pawn moves
		movesBB = (pawnBB & RANK_2) << 16;
		movesBB &= ~(wPieces | bPieces);
		// Make sure that pawns don't jump over pieces on the third rank
		movesBB &= ~((wPieces | bPieces) & RANK_3) << 8;
		while (movesBB) {
			endSquare = popLSB(&movesBB);
			move = makeMove(endSquare-16, endSquare);
			*(moves++) = move;
		}

		// Single pawn moves
		movesBB = pawnBB << 8;
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
		epBB = board.epSquare > -1 ? 1ull << board.epSquare : 0ull;
		movesBB = (pawnBB & (~H_FILE)) << 7;
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
		movesBB = (pawnBB & (~A_FILE)) << 9;
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
		pawnBB = board.pieces[B_PAWN];
		
		// Double pawn moves
		movesBB = (pawnBB & RANK_7) >> 16;
		movesBB &= ~(wPieces | bPieces);
		// Make sure that pawns don't jump over pieces on the third rank
		movesBB &= ~((wPieces | bPieces) & RANK_6) >> 8;
		while (movesBB) {
			endSquare = popLSB(&movesBB);
			move = makeMove(endSquare+16, endSquare);
			*(moves++) = move;
		}

		// Single pawn moves
		movesBB = pawnBB >> 8;
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
		epBB = board.epSquare > -1 ? 1ull << board.epSquare : 0ull;
		movesBB = (pawnBB & (~H_FILE)) >> 9;
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
		movesBB = (pawnBB & (~A_FILE)) >> 7;
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


uint16_t* generateKnightMoves(Board board, uint16_t *moves) {
	int piece = board.turn ? W_KNIGHT : B_KNIGHT;
	uint64_t knightBB = board.pieces[piece];
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
	
		movesBB &= piece==W_KNIGHT ? ~whitePieces(&board) : ~blackPieces(&board);

		while (movesBB) {
			endSquare = popLSB(&movesBB);
			move = makeMove(square, endSquare);
			*(moves++) = move;
		}
	}
	return moves;
}


uint64_t slidingMoves(Board board, int piece, uint64_t mask) {
	// Generates sliding moves along a file, rank or diagonal (specified by the mask)
	uint64_t pieceBB = 1ull << piece;
	uint64_t movesBB = 0ull;
	uint64_t negative = 0ull;
	uint64_t friendly = board.turn ? whitePieces(&board) : blackPieces(&board);
	
	uint64_t occupied = occupiedSquares(&board);
	uint64_t occMask = occupied & mask;

	movesBB = occMask^(occMask-(pieceBB<<1));
	movesBB &= mask;

	negative = occMask^reverse(reverse(occMask)-(reverse(pieceBB)<<1));
	negative &= mask;
	negative &= (1ull<<piece)-1;
	movesBB |= negative;

	movesBB &= ~friendly;
	return movesBB;
}


uint16_t* generateBishopMoves(Board board, uint16_t *moves)  {
	int piece = board.turn ? W_BISHOP : B_BISHOP;
	uint64_t friendly = board.turn ? whitePieces(&board) : blackPieces(&board);
	uint64_t bishopBB = board.pieces[piece];
	uint64_t movesBB = 0ull;
	uint64_t diagonal;
	uint64_t antidiagonal;
	uint16_t move;
	int square;
	int endSquare;
	
	while (bishopBB) {
		square = popLSB(&bishopBB);
		diagonal = getDiagonal(square);
		antidiagonal = getAntidiagonal(square);
		movesBB = slidingMoves(board, square, diagonal);
		movesBB |= slidingMoves(board, square, antidiagonal);

		while (movesBB) {
			endSquare = popLSB(&movesBB);
			move = makeMove(square, endSquare);
			*(moves++) = move;
		}
	}
	return moves;
}


uint16_t* generateRookMoves(Board board, uint16_t *moves) {
	int piece = board.turn ? W_ROOK : B_ROOK;
	uint64_t friendly = board.turn ? whitePieces(&board) : blackPieces(&board);
	uint64_t rookBB = board.pieces[piece];
	uint64_t movesBB = 0ull;
	uint64_t rank;
	uint64_t file;
	uint16_t move;
	int square;
	int endSquare;

	while (rookBB) {
		square = popLSB(&rookBB);
		rank = getRank(square);
		file = getFile(square);
		movesBB = slidingMoves(board, square, file);
		movesBB |= slidingMoves(board, square, rank);

		while (movesBB) {
			endSquare = popLSB(&movesBB);
			move = makeMove(square, endSquare);
			*(moves++) = move;
		}
	}
	return moves;
}


uint16_t* generateQueenMoves(Board board, uint16_t *moves) {
	int piece = board.turn ? W_QUEEN : B_QUEEN;
	uint64_t friendly = board.turn ? whitePieces(&board) : blackPieces(&board);
	uint64_t queenBB = board.pieces[piece];
	uint64_t movesBB = 0ull;
	uint64_t rank;
	uint64_t file;
	uint64_t diagonal;
	uint64_t antidiagonal;
	uint16_t move;
	int square;
	int endSquare;

	while (queenBB) {
		square = popLSB(&queenBB);
		rank = getRank(square);
		file = getFile(square);
		diagonal = getDiagonal(square);
		antidiagonal = getAntidiagonal(square);
		movesBB = slidingMoves(board, square, file);
		movesBB |= slidingMoves(board, square, rank);
		movesBB |= slidingMoves(board, square, diagonal);
		movesBB |= slidingMoves(board, square, antidiagonal);

		while (movesBB) {
			endSquare = popLSB(&movesBB);
			move = makeMove(square, endSquare);
			*(moves++) = move;
		}
	}
	return moves;
}


uint16_t* generateKingMoves(Board board, uint16_t *moves) {
	int piece = board.turn ? W_KING : B_KING;
	uint64_t king = board.pieces[piece];
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
	movesBB &= piece==W_KING ? ~whitePieces(&board) : ~blackPieces(&board);

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


uint16_t* generateCastlingMoves(Board board, uint16_t *moves) {
	if (board.turn && ~isSquareAttacked(board, false, 3)) {
		if ((board.castling & 0x1) > 0 && (occupiedSquares(&board) & 0x6) == 0) {
			// Kingside castling is allowed and f1, g1 are empty
			if (~(isSquareAttacked(board, false, 2) || isSquareAttacked(board, false, 1))) {
				*(moves++) = makeCastlingMove(3, 1);
			}
		}
		if ((board.castling & 0x2) > 0 && (occupiedSquares(&board) & 0x70) == 0) {
			// Quenside castling is allowed and b1, c1, d1 are empty
			if (~(isSquareAttacked(board, false, 4) || isSquareAttacked(board, false, 5))) {
				*(moves++) = makeCastlingMove(3, 5);
			}
		}
	} else if (~board.turn && ~isSquareAttacked(board, true, 59)) {
		if ((board.castling & 0x4) > 0 && (occupiedSquares(&board) & 0x0600000000000000) == 0) {
			// Kingside castling is allowed and f8, g8 are empty
			if (~(isSquareAttacked(board, true, 57) || isSquareAttacked(board, true, 58))) {
				*(moves++) = makeCastlingMove(59, 57);
			}
		}
		if ((board.castling & 0x8) > 0 && (occupiedSquares(&board) & 0x7000000000000000) == 0) {
			// Queenside castling is allowed and b8, c8, d8 are empty
			if (~(isSquareAttacked(board, true, 60) || isSquareAttacked(board, true, 61))) {
				*(moves++) = makeCastlingMove(59, 61);
			}
		}
	}
	return moves;
}


uint16_t* generateAllMoves(Board board, uint16_t *moves) {
	moves = generatePawnMoves(board, moves);
	moves = generateKnightMoves(board, moves);
	moves = generateBishopMoves(board, moves);
	moves = generateRookMoves(board, moves);
	moves = generateQueenMoves(board, moves);
	moves = generateKingMoves(board, moves);
	moves = generateCastlingMoves(board, moves);

	return moves;
}


int perft(int depth, Board *board) {
	if (depth == 0) {
		return 1;
	}
	int nodes = 0;
	uint16_t move = 0x0649ull;
	uint16_t *moves = &move;
	Board b;
	clearBoard(&b);
	boardFromFEN(&b, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	generateAllMoves(b, moves);

	while (uint16_t m = *(moves++)) {
		printMove(m);
		playMove(&b, m);
		if (isLegalPosition(b)) {
			nodes += perft(depth-1, &b);
		}
		unmakeMove(&b, m);
	}
	return nodes;
}
