#include <iostream>

#include "move.h"
#include "board.h"
#include "bitboard.h"
#include "types.h"


void printMove(uint16_t move) {
	char* startSquare = getSquareName(move & 0x3F);
	char* endSquare = getSquareName((move & 0xFC0) >> 6);
	int promPiece = (move & 0x7000) >> 12;
	if (promPiece > 0 && promPiece < 5) {
		std::cout << startSquare << "->" << endSquare << " " << promPiece << "\n";
	} else {
		std::cout << startSquare << "->" << endSquare << "\n";
	}
}


int getStartSquare(uint16_t move) {
	return (move & 0x3Full);
}


int getEndSquare(uint16_t move) {
	return ((move >> 6) & 0x3Full);
}


bool moveIsPromotion(uint16_t move) {
	int prom = (move & 0x7000) >> 12;
	return (prom > 0 && prom < 5);
}


int getPromotionPiece(uint16_t move) {
	return ((move & 0x7000) >> 12);
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
	move |= 0x8000;
	return move;
}


uint16_t encodeEPMove(int startSqIndex, int endSqIndex) {
	uint16_t move = encodeMove(startSqIndex, endSqIndex);
	move |= ENPASSANT;
	return move;
}


uint16_t encodeUCIMove(Board *board, char *UCImove) {
	// TODO: castling and promotion
	int startSquare = getSquare(UCImove[0], UCImove[1]-'1');
	int endSquare = getSquare(UCImove[2], UCImove[3]-'1');
	if (startSquare == 3 && 
			((endSquare == 1 && (board->castling & W_KS_CASTLING) == W_KS_CASTLING) || endSquare == 5 && (board->castling & W_QS_CASTLING) == W_QS_CASTLING)) {
		return encodeCastlingMove(startSquare, endSquare);
	}
	if (startSquare == 59 && 
			((endSquare == 57 && (board->castling & B_KS_CASTLING) == B_KS_CASTLING) || endSquare == 61 && (board->castling & B_QS_CASTLING) == B_QS_CASTLING)) {
		return encodeCastlingMove(startSquare, endSquare);
	}
	if (strlen(UCImove) == 5) {
		int promPiece = 0;
		switch (UCImove[4]){
			case 'n':
				promPiece = 1;
				break;
			case 'b':
				promPiece = 2;
				break;
			case 'r':
				promPiece = 3;
				break;
			case 'q':
				promPiece = 4;
				break;
		}
		return encodeMove(startSquare, endSquare, promPiece);
	}
	return encodeMove(startSquare, endSquare);
}


bool isEnPassant(uint16_t move) {
	return (move & ENPASSANT) == ENPASSANT;
}


bool isCastling(uint16_t move) {
	return (move & 0x8000) == 0x8000;
}


void makeMove(Board *board, uint16_t move) {
	if (! board->turn) {
		board->fullMoveCounter++;
	}
	if (move == NULL_MOVE) {
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


	if (! isCastling(move)) {
		// move is not castling
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
			for (int i = 0; i < 12; i++) {
				if ((board->pieces[i] & endBB) != 0) {
					board->pieces[i] ^= endBB;
					// Removing castling rights if a rook is captured
					if (i == W_ROOK) {
						if (endSquare == 0) {
							board->castling &= ~W_KS_CASTLING;
						} else if (endSquare == 7) {
							board->castling &= ~W_QS_CASTLING;
						}
					} else if (i == B_ROOK) {
						if (endSquare == 56) {
							board->castling &= ~B_KS_CASTLING;
						} else if (endSquare == 63) {
							board->castling &= ~B_QS_CASTLING;
						}
					}
					break;
				}
			}
		}
		if (moveIsPromotion(move)) {
			board->halfMoveCounter = 0;
			piece = turn ? W_PAWN : B_PAWN;
			board->pieces[piece+getPromotionPiece(move)] ^= endBB;
			return;
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
					board->castling &= ~W_KS_CASTLING;
				} else if (startSquare == 7) {
					board->castling &= ~W_QS_CASTLING;
				}
				break;
			case B_ROOK:
				if (startSquare == 56) {
					board->castling &= ~B_KS_CASTLING;
				} else if (startSquare == 63) {
					board->castling &= ~B_QS_CASTLING;
				}
				break;
		}
	} else {
		if (endSquare == 1) {
			board->castling &= (B_KS_CASTLING | B_QS_CASTLING);
			// Castling kingside
			board->pieces[W_KING] ^= 0x000000000000000A;
			board->pieces[W_ROOK] ^= 0x0000000000000005;
		} else if (endSquare == 5) {
			board->castling &= (B_KS_CASTLING | B_QS_CASTLING);
			// Castling queenside
			board->pieces[W_KING] ^= 0x0000000000000028;
			board->pieces[W_ROOK] ^= 0x0000000000000090;
		} else if (endSquare == 57) {
			board->castling &= (W_KS_CASTLING | W_QS_CASTLING);
			// Castling kingside
			board->pieces[B_KING] ^= 0x0A00000000000000;
			board->pieces[B_ROOK] ^= 0x0500000000000000;
		} else if (endSquare == 61) {
			board->castling &= (W_KS_CASTLING | W_QS_CASTLING);
			// Castling queenside
			board->pieces[B_KING] ^= 0x2800000000000000;
			board->pieces[B_ROOK] ^= 0x9000000000000000;
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
	
	if (moveIsPromotion(move)) {
		piece = board->turn ? W_PAWN : B_PAWN;
		board->pieces[piece] ^= startBB;
		board->pieces[piece+getPromotionPiece(move)] ^= endBB;
		if (undo->capturedPiece >= 0) {
			board->pieces[undo->capturedPiece] ^= endBB;
		}
		return;
	}

	if (! isCastling(move)) {
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
		if (endSquare == 1) {
			board->pieces[W_KING] ^= startBB | endBB;
			board->pieces[W_ROOK] ^= 0x5;
		} else if (endSquare == 5) {
			board->pieces[W_KING] ^= startBB | endBB;
			board->pieces[W_ROOK] ^= 0x90;
		} else if (endSquare == 57) {
			board->pieces[B_KING] ^= startBB | endBB;
			board->pieces[B_ROOK] ^= (1ull << 58) | (1ull << 56);
		} else if (endSquare == 61) {
			board->pieces[B_KING] ^= startBB | endBB;
			board->pieces[B_ROOK] ^= (1ull << 63) | (1ull << 60);
		}
	}
}
