#include "attacks.h"
#include "bitboard.h"
#include "board.h"
#include "move.h"
#include "movegen.h"
#include "types.h"

#include <stdint.h>
#include <iostream>


uint16_t* generatePawnMoves(Board *board, uint16_t *moves) {
	uint64_t wPieces = whitePieces(board);
	uint64_t bPieces = blackPieces(board);
	uint64_t occupied = wPieces | bPieces;
	uint64_t empty = ~occupied;
	uint64_t movesBB;
	uint64_t pawnBB;
	int endSquare;
	uint64_t epBB;
	int epSquare = board->epSquare;

	if (board->turn == WHITE) {
		pawnBB = board->pieces[W_PAWN];
		
		// Double pawn moves
		movesBB = ((pawnBB & RANK_2) << 16) & empty;
		// Make sure that pawns don't jump over pieces on the third rank
		movesBB &= ~(occupied & RANK_3) << 8;
		while (movesBB) {
			endSquare = popLSB(&movesBB);
			*(moves++) = encodeMove(endSquare-16, endSquare);
		}

		// Single pawn moves
		movesBB = (pawnBB << 8) & empty;
		while (movesBB) {
			endSquare = popLSB(&movesBB);
			if (endSquare >= 56) {
				// Adding the promoting pieces
				// N=1, B=2, R=3, Q=4
				for (int p = 1; p <= 4; p++) {
					*(moves++) = encodeMove(endSquare-8, endSquare, p);
				}
			} else {
				*(moves++) = encodeMove(endSquare-8, endSquare);
			}
		}

		// Pawn captures
		epBB = epSquare > -1 ? 1ull << epSquare : 0ull;
		movesBB = (pawnBB & (~H_FILE)) << 7;
		if ((movesBB & epBB) != 0) {
			*(moves++) = encodeEPMove(epSquare-7, epSquare);
		}
		movesBB &= bPieces;
		while (movesBB) {
			endSquare = popLSB(&movesBB);
			if (endSquare >= 56) {
				// Pawn promotions
				for (int p = 1; p <= 4; p++) {
					*(moves++) = encodeMove(endSquare-7, endSquare, p);
				}
			} else {
				*(moves++) = encodeMove(endSquare-7, endSquare);
			}
		}
		movesBB = (pawnBB & (~A_FILE)) << 9;
		if ((movesBB & epBB) != 0) {
			*(moves++) = encodeEPMove(epSquare-9, epSquare);
		}
		movesBB &= bPieces;
		while (movesBB) {
			endSquare = popLSB(&movesBB);
			if (endSquare >= 56) {
				for (int p = 1; p <= 4; p++) {
					*(moves++) = encodeMove(endSquare-9, endSquare, p);
				}
			} else {
				*(moves++) = encodeMove(endSquare-9, endSquare);
			}
		}
	} else {
		pawnBB = board->pieces[B_PAWN];
		
		// Double pawn moves
		movesBB = ((pawnBB & RANK_7) >> 16) & empty;
		// Make sure that pawns don't jump over pieces on the third rank
		movesBB &= ~(occupied & RANK_6) >> 8;
		while (movesBB) {
			endSquare = popLSB(&movesBB);
			*(moves++) = encodeMove(endSquare+16, endSquare);
		}

		// Single pawn moves
		movesBB = (pawnBB >> 8) & empty;
		while (movesBB) {
			endSquare = popLSB(&movesBB);
			if (endSquare <= 7) {
				// Adding the promoting pieces
				// N=1, B=2, R=3, Q=4
				for (int p = 1; p <= 4; p++) {
					*(moves++) = encodeMove(endSquare+8, endSquare, p);
				}
			} else {
				*(moves++) = encodeMove(endSquare+8, endSquare);
			}
		}

		// Pawn captures
		epBB = epSquare > -1 ? 1ull << epSquare : 0ull;
		movesBB = (pawnBB & (~H_FILE)) >> 9;
		if ((movesBB & epBB) != 0) {
			*(moves++) = encodeEPMove(epSquare+9, epSquare);
		}
		movesBB &= wPieces;
		while (movesBB) {
			endSquare = popLSB(&movesBB);
			if (endSquare <= 7) {
				for (int p = 1; p <= 4; p++) {
					*(moves++) = encodeMove(endSquare+9, endSquare, p);
				}
			} else {
				*(moves++) = encodeMove(endSquare+9, endSquare);
			}
		}
		movesBB = (pawnBB & (~A_FILE)) >> 7;
		if ((movesBB & epBB) != 0) {
			*(moves++) = encodeEPMove(epSquare+7, epSquare);
		}
		movesBB &= wPieces;
		while (movesBB) {
			endSquare = popLSB(&movesBB);
			if (endSquare <= 7) {
				for (int p = 1; p <= 4; p++) {
					*(moves++) = encodeMove(endSquare+7, endSquare, p);
				}
			} else {
				*(moves++) = encodeMove(endSquare+7, endSquare);
			}
		}
	}
	return moves;
}


uint16_t* generateKnightMoves(Board *board, uint16_t *moves, uint64_t friendly) {
	int piece = board->turn ? W_KNIGHT : B_KNIGHT;
	uint64_t knightBB = board->pieces[piece];
	uint64_t movesBB;
	int square;
	
	while (knightBB) {
		square = popLSB(&knightBB);
		movesBB = KNIGHT_ATTACKS[square] & ~friendly;
	
		while (movesBB) {
			*(moves++) = encodeMove(square, popLSB(&movesBB));
		}
	}
	return moves;
}


uint64_t slidingMoves(Board *board, int piece, uint64_t mask) {
	// Generates sliding moves along a file, rank or diagonal (specified by the mask)
	uint64_t movesBB;
	uint64_t friendly = board->turn ? whitePieces(board) : blackPieces(board);

	movesBB = slidingAttacks(board, piece, mask);

	movesBB &= ~friendly;
	return movesBB;
}


uint16_t* generateBishopMoves(Board *board, uint16_t *moves, uint64_t friendly, uint64_t occupied)  {
	int piece = board->turn ? W_BISHOP : B_BISHOP;
	uint64_t bishopBB = board->pieces[piece];
	uint64_t movesBB;
	int square;
	
	while (bishopBB) {
		square = popLSB(&bishopBB);
		movesBB = bishopAttacks(occupied, square) & ~friendly;
		while (movesBB) {
			*(moves++) = encodeMove(square, popLSB(&movesBB));
		}
	}
	return moves;
}


uint16_t* generateRookMoves(Board *board, uint16_t *moves, uint64_t friendly, uint64_t occupied) {
	int piece = board->turn ? W_ROOK : B_ROOK;
	uint64_t rookBB = board->pieces[piece];
	uint64_t movesBB;
	uint16_t move;
	int square;

	while (rookBB) {
		square = popLSB(&rookBB);
		movesBB = rookAttacks(occupied, square) & ~friendly;

		while (movesBB) {
			move = encodeMove(square, popLSB(&movesBB));
			if (square == 0 || square == 56) {
				move |= REM_KS_CASTLING;
			} else if (square == 7 || square == 63) {
				move |= REM_QS_CASTLING;
			}
			*(moves++) = move;
		}
	}
	return moves;
}


uint16_t* generateQueenMoves(Board *board, uint16_t *moves, uint64_t friendly, uint64_t occupied) {
	int piece = board->turn ? W_QUEEN : B_QUEEN;
	uint64_t queenBB = board->pieces[piece];
	uint64_t movesBB;
	int square;

	while (queenBB) {
		square = popLSB(&queenBB);
		movesBB = bishopAttacks(occupied, square);
		movesBB |= rookAttacks(occupied, square);
		movesBB &= ~friendly;

		while (movesBB) {
			*(moves++) = encodeMove(square, popLSB(&movesBB));
		}
	}
	return moves;
}


uint16_t* generateKingMoves(Board *board, uint16_t *moves, uint64_t friendly) {
	int piece = board->turn ? W_KING : B_KING;
	uint64_t king = board->pieces[piece];
	int square = popLSB(&king);
	
	uint64_t movesBB = KING_ATTACKS[square] & ~friendly;

	while (movesBB) {
		*(moves++) = encodeMove(square, popLSB(&movesBB));
	}
	return moves;
}


uint16_t* generateCastlingMoves(Board *board, uint16_t *moves, uint64_t occupied) {
	if (board->turn == WHITE && ((board->castling & (W_KS_CASTLING | W_QS_CASTLING)) != 0)) {
		uint64_t b_attacks = getAttacks(*board, false);
		if ((board->castling & W_KS_CASTLING) == W_KS_CASTLING && (occupied & 0x6) == 0) {
			if ((b_attacks & 0xE) == 0) {
				*(moves++) = encodeCastlingMove(3, 1);
			}
		} 
		if ((board->castling & W_QS_CASTLING) == W_QS_CASTLING && (occupied & 0x70) == 0) {
			if ((b_attacks & 0x38) == 0) {
				*(moves++) = encodeCastlingMove(3, 5);
			}
		}
	} else if (board->turn == BLACK && ((board->castling & (B_KS_CASTLING | B_QS_CASTLING)) != 0)) {
		uint64_t w_attacks = getAttacks(*board, true);
		if ((board->castling & B_KS_CASTLING) == B_KS_CASTLING && (occupied & 0x0600000000000000) == 0) {
			if ((w_attacks & 0x0E00000000000000) == 0) {
				*(moves++) = encodeCastlingMove(59, 57);
			}
		}
	       	if ((board->castling & B_QS_CASTLING) == B_QS_CASTLING && (occupied & 0x7000000000000000) == 0) {
			if ((w_attacks & 0x3800000000000000) == 0) {
				*(moves++) = encodeCastlingMove(59, 61);
			}
		}
	}
	
	return moves;
}


uint16_t* generateAllMoves(Board board, uint16_t *moves) {
	uint64_t friendly = board.turn ? whitePieces(&board) : blackPieces(&board);
	uint64_t occupied = occupiedSquares(&board);
	moves = generatePawnMoves(&board, moves);
	moves = generateKnightMoves(&board, moves, friendly);
	moves = generateBishopMoves(&board, moves, friendly, occupied);
	moves = generateRookMoves(&board, moves, friendly, occupied);
	moves = generateQueenMoves(&board, moves, friendly, occupied);
	moves = generateKingMoves(&board, moves, friendly);
	moves = generateCastlingMoves(&board, moves, occupied);

	return moves;
}


uint16_t* moveOrdering(Board *board, uint16_t *moves, int limit) {
	// uint16_t *orderedMoves = new uint16_t[limit];
	int index = 0;
	uint16_t move;

	for (int i = 0; i < limit; i++) {
		move = *(moves+i);
		if (isCapture(board, move)) {
			if ((getPieceAtSquare(board, getStartSquare(move)) % 6) < (getPieceAtSquare(board, getEndSquare(move)) % 6)) {
				/*
				*(orderedMoves+index) = move;
				*(orderedMoves+i) = *(moves+index);
				*/
				*(moves+i) = *(moves+index);
				*(moves+index) = move;
				index++;
			}
		}
	        /*	
		else {
			*(orderedMoves+i) = move;
		}
		*/
	}
	return moves;
}


int perft(int depth, Board *board) {
	if (depth == 0) {
		return 1;
	}
	/*
	if (isGameOver(board)) {
		return 0;
	}
	*/
	int nodes = 0;
	uint16_t *moves = new uint16_t[MAX_MOVES];
	uint16_t *end = generateAllMoves(*board, moves);
	Undo undo;
	int limit = end-moves;
	uint16_t move;

	for (int i = 0; i < limit; i++) {
		move = *(moves+i);
		undo = generateUndo(board, move);
		makeMove(board, move);
		if (isLegalPosition(board)) {
			/*
			if (depth == 2) {
				printMove(move);
				// printBoard(board);
			}
			*/
			nodes += perft(depth-1, board);
		}
		unmakeMove(board, move, &undo);
	}
	/*
	if (depth == 1) {
		std::cout << nodes << "\n";
	}
	*/
	return nodes;
}
