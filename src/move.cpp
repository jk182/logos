#include <iostream>

#include "move.h"
#include "board.h"
#include "bitboard.h"
#include "types.h"


void printMove(uint16_t move) {
	// TODO: add promotion piece
	char* startSquare = getSquareName(move & 0x3F);
	char* endSquare = getSquareName((move & 0xFC0) >> 6);
	std::cout << startSquare << "->" << endSquare << "\n";
}


int getStartSquare(uint16_t move) {
	return (move & 0x3Full);
}


int getEndSquare(uint16_t move) {
	return ((move >> 6) & 0x3Full);
}


bool moveIsPromotion(uint16_t move) {
	int prom = move & (0x7 << 12);
	return (prom > 0 && prom < 5);
}


uint16_t encodeMove(int startSqIndex, int endSqIndex) {
	return startSqIndex | (endSqIndex << 6);
}


uint16_t encodeMove(int startSqIndex, int endSqIndex, int promotionPiece) {
	uint16_t move = encodeMove(startSqIndex, endSqIndex);
	move |= promotionPiece << 12;
	return move;
}


uint16_t encodeCastlingMove(int startSqIndex, int endSqIndex) {
	uint16_t move = encodeMove(startSqIndex, endSqIndex);
	move |= 1ull << 15;
	return move;
}


uint16_t encodeEPMove(int startSqIndex, int endSqIndex) {
	uint16_t move = encodeMove(startSqIndex, endSqIndex);
	move |= ENPASSANT;
	return move;
}


bool isEnPassant(uint16_t move) {
	return (move & ENPASSANT) == ENPASSANT;
}


void makeMove(Board *board, uint16_t move) {
	if (move == NULL_MOVE) {
		if (! board->turn) {
			board->fullMoveCounter++;
		}
		board->turn = !board->turn;
		return;
	}
	int startSquare = getStartSquare(move);
	int endSquare = getEndSquare(move);
	uint64_t startBB = 1ull << startSquare;
	uint64_t endBB = 1ull << endSquare;
	int piece;
	int startPiece;
	int endPiece;
	bool turn = board->turn;

	board->turn = !turn;
	board->epSquare = -1;

	startPiece = turn ? W_PAWN : B_PAWN;
	endPiece = turn ? W_KING : B_KING;

	if ((move & REM_KS_CASTLING) != 0) {
		if (turn) {
			board->castling &= ~W_KS_CASTLING;
		} else {
			board->castling &= ~B_KS_CASTLING;
		}
	}
	if ((move & REM_QS_CASTLING) != 0) {
		if (turn) {
			board->castling &= ~W_QS_CASTLING;
		} else {
			board->castling &= ~B_QS_CASTLING;
		}
	}

	if (isEnPassant(move)) {
		board->halfMoveCounter = 0;
		piece = turn ? W_PAWN : B_PAWN;
		board->pieces[piece] ^= startBB ^ endBB;
		if (turn) {
			board->pieces[B_PAWN] ^= (endBB >> 8);
		} else {
			board->pieces[W_PAWN] ^= (endBB << 8);
		}
		return;
	}
	if ((move & (1ull << 15)) == 0) {
		// move is not castling
		if (! turn) {
			board->fullMoveCounter++;
		}
		for (int i = startPiece; i <= endPiece; i++) {
			if ((board->pieces[i] & startBB) != 0) {
				piece = i;
				break;
			}
		}
		if (piece == W_PAWN || piece == B_PAWN) {
			board->halfMoveCounter = 0;
			int mult = turn ? 1 : -1;
			if (endSquare == startSquare+16*mult) {
				board->epSquare = startSquare+8*mult;
			}
		}
		board->pieces[piece] ^= startBB;
		// Detecting captures
		if ((occupiedSquares(board) & endBB) != 0) {
			board->halfMoveCounter = 0;
			for (int i = (startPiece+6)%12; i <= (endPiece+6)%12; i++) {
				if ((board->pieces[i] & endBB) != 0) {
					board->pieces[i] ^= endBB;
					break;
				}
			}
		}
		board->pieces[piece] ^= endBB;
		// Handling castling rights
		switch (piece) {
			case W_KING:
				board->castling &= (B_KS_CASTLING | B_QS_CASTLING);
				break;
			case B_KING:
				board->castling &= (W_KS_CASTLING | W_QS_CASTLING);
				break;
			case W_ROOK:
				if (startSquare == 0) {
					board->castling &= 0b1110ull;
				} else if (startSquare == 7) {
					board->castling &= 0b1101ull;
				}
				break;
			case B_ROOK:
				if (startSquare == 56) {
					board->castling &= 0b1011ull;
				} else if (startSquare == 63) {
					board->castling &= 0b0111ull;
				}
				break;
		}
	} else {
		if (turn) {
			board->castling &= (B_KS_CASTLING | B_QS_CASTLING);
			if (endSquare == 1) {
				// Castling kingside
				board->pieces[W_KING] ^= 0x000000000000000A;
				board->pieces[W_ROOK] ^= 0x0000000000000005;
			} else {
				// Castling queenside
				board->pieces[W_KING] ^= 0x0000000000000028;
				board->pieces[W_ROOK] ^= 0x0000000000000090;
			}
		} else {
			board->castling &= (W_KS_CASTLING | W_QS_CASTLING);
			if (endSquare == 57) {
				// Castling kingside
				board->pieces[B_KING] ^= 0x0A00000000000000;
				board->pieces[B_ROOK] ^= 0x0500000000000000;
			} else {
				// Castling queenside
				board->pieces[B_KING] ^= 0x2800000000000000;
				board->pieces[B_ROOK] ^= 0x9000000000000000;
			}
		}
	}
}


void unmakeMove(Board *board, uint16_t move, Undo *undo) {
	if (board->turn) {
		board->fullMoveCounter--;
	}
	if (move == NULL_MOVE) {
		board->turn = ~board->turn;
		return;
	}
	int startSquare = getStartSquare(move);
	int endSquare = getEndSquare(move);
	uint64_t startBB = 1ull << startSquare;
	uint64_t endBB = 1ull << endSquare;
	int piece;
	bool turn = board->turn;

	board->turn = !turn;
	board->halfMoveCounter = undo->halfMoveCounter;
	board->epSquare = isEnPassant(move) ? endSquare : -1;
	board->castling = undo->castling;
	
	if ((move & (1ull << 15)) == 0) {
		if (isEnPassant(move)) {
			piece = board->turn ? W_PAWN : B_PAWN;
			board->pieces[piece] ^= startBB ^ endBB;
			if (board->turn) {
				board->pieces[B_PAWN] ^= (endBB >> 8);
			} else {
				board->pieces[W_PAWN] ^= (endBB << 8);
			}
			return;
		}
		if (moveIsPromotion(move)) {
			piece = board->turn ? W_PAWN : B_PAWN;
			board->pieces[piece] ^= startBB;
			int promPiece = ((move >> 12) & 0xF);
			board->pieces[piece+promPiece] ^= endBB;
			if (undo->capturedPiece >= 0) {
				board->pieces[undo->capturedPiece] ^= endBB;
			}
			return;
		}
		for (int i = 0; i < 12; i++) {
			if ((board->pieces[i] & endBB) != 0) {
				piece = i;
				break;
			}
		}
		board->pieces[piece] ^= startBB ^ endBB;
		if (undo->capturedPiece > -1) {
			board->pieces[undo->capturedPiece] ^= endBB;
		}
	} else {
		if (board->turn) {
			board->pieces[W_KING] ^= startBB | endBB;
			if (endSquare == 1) {
				board->pieces[W_ROOK] ^= 0x5;
			} else {
				board->pieces[W_ROOK] ^= 0x90;
			}
		} else {
			board->pieces[B_KING] ^= startBB | endBB;
			if (endSquare == 57) {
				board->pieces[B_ROOK] ^= (1ull << 58) | (1ull << 56);
			} else {
				board->pieces[B_ROOK] ^= (1ull << 53) | (1ull << 60);
			}
		}
	}
}
