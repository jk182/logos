#include "attacks.h"
#include "bitboard.h"
#include "board.h"
#include "magic.h"
#include "move.h"
#include "movegen.h"
#include "types.h"

#include <stdint.h>
#include <iostream>


uint16_t* generatePawnMoves(uint64_t pawnBB, uint64_t friendly, uint64_t occupied, bool turn, int epSquare, uint16_t *moves) {
	uint64_t enemy = friendly ^ occupied;
	int startSquare;

	if (turn) {
		while (pawnBB) {
			startSquare = popLSB(&pawnBB);
			if (startSquare >= 48) {
				// pawn promotions
				if ((1ull << (startSquare+8) & occupied) == 0) {
					// N=1, B=2, R=3, Q=4
					for (int p = 1; p <= 4; p++) {
						*(moves++) = encodeMove(startSquare, startSquare+8, p);
					}
				}
				// captures
				if (startSquare != 48 && ((1ull << (startSquare+7) & enemy) != 0)) {
					for (int p = 1; p <= 4; p++) {
						*(moves++) = encodeMove(startSquare, startSquare+7, p);
					}
				}
				if (startSquare != 55 && ((1ull << (startSquare+9) & enemy) != 0)) {
					for (int p = 1; p <= 4; p++) {
						*(moves++) = encodeMove(startSquare, startSquare+9, p);
					}
				}
			} else {
				if ((1ull << (startSquare+8) & occupied) == 0) {
					*(moves++) = encodeMove(startSquare, startSquare+8);
					if (startSquare >= 8 && startSquare <= 15) {
						if ((1ull << (startSquare+16) & occupied) == 0) {
							*(moves++) = encodeMove(startSquare, startSquare+16);
						}
					}
				}
				if (startSquare % 8 != 0) {
					if (((1ull << (startSquare+7)) & enemy) != 0) {
						*(moves++) = encodeMove(startSquare, startSquare+7);
					} else if (startSquare+7 == epSquare) {
						*(moves++) = encodeEPMove(startSquare, startSquare+7);
					}
				}
				if (startSquare % 8 != 7) {
					if (((1ull << (startSquare+9)) & enemy) != 0) {
						*(moves++) = encodeMove(startSquare, startSquare+9);
					} else if (startSquare+9 == epSquare) {
						*(moves++) = encodeEPMove(startSquare, startSquare+9);
					}
				}
			}
		}
	} else {
		while (pawnBB) {
			startSquare = popLSB(&pawnBB);
			if (startSquare <= 15) {
				// pawn promotions
				if ((1ull << (startSquare-8) & occupied) == 0) {
					// N=1, B=2, R=3, Q=4
					for (int p = 1; p <= 4; p++) {
						*(moves++) = encodeMove(startSquare, startSquare-8, p);
					}
				}
				// captures
				if (startSquare != 15 && ((1ull << (startSquare-7) & enemy) != 0)) {
					for (int p = 1; p <= 4; p++) {
						*(moves++) = encodeMove(startSquare, startSquare-7, p);
					}
				}
				if (startSquare != 8 && ((1ull << (startSquare-9) & enemy) != 0)) {
					for (int p = 1; p <= 4; p++) {
						*(moves++) = encodeMove(startSquare, startSquare-9, p);
					}
				}
			} else {
				if ((1ull << (startSquare-8) & occupied) == 0) {
					*(moves++) = encodeMove(startSquare, startSquare-8);
					if (startSquare <= 55 && startSquare >= 48) {
						if ((1ull << (startSquare-16) & occupied) == 0) {
							*(moves++) = encodeMove(startSquare, startSquare-16);
						}
					}
				}
				if (startSquare % 8 != 7) {
					if (((1ull << (startSquare-7)) & enemy) != 0) {
						*(moves++) = encodeMove(startSquare, startSquare-7);
					} else if (startSquare-7 == epSquare) {
						*(moves++) = encodeEPMove(startSquare, startSquare-7);
					}
				}
				if (startSquare % 8 != 0) {
					if (((1ull << (startSquare-9)) & enemy) != 0) {
						*(moves++) = encodeMove(startSquare, startSquare-9);
					} else if (startSquare-9 == epSquare) {
						*(moves++) = encodeEPMove(startSquare, startSquare-9);
					}
				}
			}
		}
	}
	return moves;
}


uint16_t* generateKnightMoves(uint64_t knightBB, uint16_t *moves, uint64_t friendly) {
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


uint16_t* generateBishopMoves(uint64_t bishopBB, uint16_t *moves, uint64_t friendly, uint64_t occupied)  {
	uint64_t movesBB;
	int square;
	Magic table;
	
	while (bishopBB) {
		square = popLSB(&bishopBB);
		table = BISHOP_TABLES[square];
		movesBB = table.table[(((occupied | EDGES) & table.mask) * table.magic) >> (64-table.index)] & ~friendly;
		while (movesBB) {
			*(moves++) = encodeMove(square, popLSB(&movesBB));
		}
	}
	return moves;
}


uint16_t* generateRookMoves(uint64_t rookBB, uint16_t *moves, uint64_t friendly, uint64_t occupied) {
	uint64_t movesBB;
	uint16_t move;
	int square;
	uint64_t edges;
	Magic table;

	while (rookBB) {
		square = popLSB(&rookBB);
		table = ROOK_TABLES[square];
		edges = ((A_FILE | H_FILE) & ~getFile(square)) | ((RANK_1 | RANK_8) & ~getRank(square));
		movesBB = table.table[(((occupied | edges) & table.mask) * table.magic) >> (64-table.index)] & ~friendly;

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


uint16_t* generateQueenMoves(uint64_t queenBB, uint16_t *moves, uint64_t friendly, uint64_t occupied) {
	uint64_t movesBB;
	int square;
	Magic table;
	uint64_t edges;

	while (queenBB) {
		square = popLSB(&queenBB);
		table = BISHOP_TABLES[square];
		movesBB = table.table[(((occupied | EDGES) & table.mask) * table.magic) >> (64-table.index)];

		table = ROOK_TABLES[square];
		edges = ((A_FILE | H_FILE) & ~getFile(square)) | ((RANK_1 | RANK_8) & ~getRank(square));
		movesBB |= table.table[(((occupied | edges) & table.mask) * table.magic) >> (64-table.index)];
		movesBB &= ~friendly;

		while (movesBB) {
			*(moves++) = encodeMove(square, popLSB(&movesBB));
		}
	}
	return moves;
}


uint16_t* generateKingMoves(uint64_t kingBB, uint16_t *moves, uint64_t friendly) {
	int square = getLSB(&kingBB);
	uint64_t movesBB = KING_ATTACKS[square] & ~friendly;

	while (movesBB) {
		*(moves++) = encodeMove(square, popLSB(&movesBB));
	}
	return moves;
}


uint16_t* generateKingMoves(Board *board, uint16_t *moves) {
	uint64_t kingBB = board->turn ? board->pieces[W_KING] : board->pieces[B_KING];
	int square = getLSB(&kingBB);
	uint64_t friendly = board->turn ? whitePieces(board) : blackPieces(board);
	uint64_t threats = getAttacks(board, !board->turn);
	uint64_t movesBB = KING_ATTACKS[square] & ~friendly & ~threats;
	// int endSquare;

	while (movesBB) {
		*(moves++) = encodeMove(square, popLSB(&movesBB));
		/*
		endSquare = popLSB(&movesBB);
		if (((1ull << endSquare) & threats) == 0) {
			*(moves++) = encodeMove(square, endSquare);
		}
		*/
	}
	return moves;
}


uint16_t* generateCastlingMoves(Board *board, uint16_t *moves, uint64_t occupied) {
	if (board->turn == WHITE && ((board->castling & (W_KS_CASTLING | W_QS_CASTLING)) != 0)) {
		uint64_t b_attacks = getAttacks(board, false);
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
		uint64_t w_attacks = getAttacks(board, true);
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


uint16_t* generateAllMoves(Board *board, uint16_t *moves) {
	uint64_t friendly = board->turn ? whitePieces(board) : blackPieces(board);
	uint64_t occupied = occupiedSquares(board);

	uint64_t pawnBB = board->turn ? board->pieces[W_PAWN] : board->pieces[B_PAWN];
	uint64_t knightBB = board->turn ? board->pieces[W_KNIGHT] : board->pieces[B_KNIGHT];
	uint64_t bishopBB = board->turn ? board->pieces[W_BISHOP] : board->pieces[B_BISHOP];
	uint64_t rookBB = board->turn ? board->pieces[W_ROOK] : board->pieces[B_ROOK];
	uint64_t queenBB = board->turn ? board->pieces[W_QUEEN] : board->pieces[B_QUEEN];
	uint64_t kingBB = board->turn ? board->pieces[W_KING] : board->pieces[B_KING];
	moves = generatePawnMoves(pawnBB, friendly, occupied, board->turn, board->epSquare, moves);
	moves = generateKnightMoves(knightBB, moves, friendly);
	moves = generateBishopMoves(bishopBB, moves, friendly, occupied);
	moves = generateRookMoves(rookBB, moves, friendly, occupied);
	moves = generateQueenMoves(queenBB, moves, friendly, occupied);
	// moves = generateKingMoves(kingBB, moves, friendly|getAttacks(board, !board->turn));
	moves = generateKingMoves(kingBB, moves, friendly);
	// moves = generateKingMoves(board, moves);
	moves = generateCastlingMoves(board, moves, occupied);

	return moves;
}


uint16_t* generatePawnCaptures(uint64_t pawnBB, uint64_t enemy, bool turn, uint16_t *moves) {
	uint64_t moveBB;
	int endSquare;
	if (turn) {
		moveBB = (pawnBB << 7) & enemy & ~H_FILE;
		while (moveBB) {
			endSquare = popLSB(&moveBB);
			// if (endSquare < 56) {
		}
	}
	return moves;
}


uint16_t* generateNoisyMoves(Board *board, uint16_t *moves) {
	uint16_t *noisyMoves = new uint16_t[MAX_MOVES];
	uint16_t *endMove = generateAllMoves(board, moves);
	int limit = endMove-moves;
	uint16_t move;
	Undo undo;
	int index = 0;
	bool isCap = false;

	for (int i = 0; i < limit; i++) {
		move = *(moves+i);
		isCap = isCapture(board, move);
		makeMove(board, move, &undo);
		if (isCap || isCheck(board)) {
			*(noisyMoves++) = move;
			index++;
		}
		unmakeMove(board, move, &undo);
	}
	return noisyMoves-index;
}


uint16_t* moveOrdering(Board *board, uint16_t *moves, int limit) {
	int index = 0;
	uint16_t move;

	for (int i = 0; i < limit; i++) {
		move = *(moves+i);
		if (isCapture(board, move)) {
			if ((getPieceAtSquare(board, getStartSquare(move)) % 6) < (getPieceAtSquare(board, getEndSquare(move)) % 6)) {
				*(moves+i) = *(moves+index);
				*(moves+index) = move;
				index++;
			}
		}
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
	uint16_t *end = generateAllMoves(board, moves);
	Undo undo;
	int limit = end-moves;
	uint16_t move;

	for (int i = 0; i < limit; i++) {
		move = *(moves+i);
		// undo = generateUndo(board, move);
		makeMove(board, move, &undo);
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
