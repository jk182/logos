#include "attacks.h"
#include "move.h"
#include "board.h"
#include "bitboard.h"
#include "types.h"
#include "zobrist.h"

#include <iostream>
#include <string.h>


void printMove(uint16_t move) {
	if (move == NULL_MOVE) {
		std::cout << "0000\n";
		return;
	}
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
	return encodeMove(startSqIndex, endSqIndex) | (promotionPiece << 12);
}


uint16_t encodeCastlingMove(int startSqIndex, int endSqIndex) {
	return encodeMove(startSqIndex, endSqIndex) | 0x8000;
}


uint16_t encodeEPMove(int startSqIndex, int endSqIndex) {
	return encodeMove(startSqIndex, endSqIndex) | ENPASSANT;
}


uint16_t encodeUCIMove(Board board, char *UCImove) {
	int startSquare = getSquare(UCImove[0], UCImove[1]-'1');
	int endSquare = getSquare(UCImove[2], UCImove[3]-'1');
	if (startSquare == 3 && 
			((endSquare == 1 && (board.castling & W_KS_CASTLING) == W_KS_CASTLING) || (endSquare == 5 && (board.castling & W_QS_CASTLING) == W_QS_CASTLING))) {
		return encodeCastlingMove(startSquare, endSquare);
	}
	if (startSquare == 59 && 
			((endSquare == 57 && (board.castling & B_KS_CASTLING) == B_KS_CASTLING) || (endSquare == 61 && (board.castling & B_QS_CASTLING) == B_QS_CASTLING))) {
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
	int pawn = board.turn ? W_PAWN : B_PAWN;
	if (board.epSquare == endSquare && getPieceAtSquare(&board, startSquare) == pawn) {
		return encodeEPMove(startSquare, endSquare);
	}
	return encodeMove(startSquare, endSquare);
}


char* decodeMoveToUCI(uint16_t move) {
	char* startSquare = getSquareName(getStartSquare(move));
	char* endSquare = getSquareName(getEndSquare(move));
	char* out = strcat(startSquare, endSquare);
	int promPiece = (move & 0x7000) >> 12;
	if (promPiece > 0 && promPiece < 5) {
		if (promPiece == 1) {
			out = strcat(out, "n");
		} else if (promPiece == 2) {
			out = strcat(out, "b");
		} else if (promPiece == 3) {
			out = strcat(out, "r");
		} else if (promPiece == 4) {
			out = strcat(out, "q");
		}
	}
	return out;
}


bool isCapture(Board *board, uint16_t move) {
	return (occupiedSquares(board) & (1ull << getEndSquare(move))) != 0;
}


bool isEnPassant(uint16_t move) {
	return (move & ENPASSANT) == ENPASSANT;
}


bool isCastling(uint16_t move) {
	return (move & 0x8000) == 0x8000;
}


void makeMove(Board *board, uint16_t move, Undo *undo) {
	// Generating the Undo
	undo->castling = board->castling;
	undo->halfMoveCounter = board->halfMoveCounter;
	undo->capturedPiece = getPieceAtSquare(board, getEndSquare(move));

	board->history[board->numMoves++] = board->hash;
	if (! board->turn) {
		board->fullMoveCounter++;
	}

	if (board->epSquare != -1) {
		board->hash ^= zobristEpKeys[board->epSquare % 8];
	}
	board->hash ^= zobristTurnKey;

	bool turn = board->turn;
	board->turn = !turn;
	board->epSquare = -1;
	board->halfMoveCounter++;

	if (move == NULL_MOVE) {
		return;
	}
	int startSquare = getStartSquare(move);
	int endSquare = getEndSquare(move);
	uint64_t startBB = 1ull << startSquare;
	uint64_t endBB = 1ull << endSquare;
	int piece = -1;

	if (isEnPassant(move)) {
		board->halfMoveCounter = 0;
		piece = turn ? W_PAWN : B_PAWN;
		board->pieces[piece] ^= startBB ^ endBB;
		if (turn) {
			board->pieces[B_PAWN] ^= (endBB >> 8);
		} else {
			board->pieces[W_PAWN] ^= (endBB << 8);
		}
		board->hash ^= zobristKeys[piece][startSquare] ^ zobristKeys[piece][endSquare];
		return;
	}


	if (! isCastling(move)) {
		int startPiece = turn ? W_PAWN : B_PAWN;
		int endPiece = turn ? W_KING : B_KING;
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
				board->hash ^= zobristEpKeys[board->epSquare % 8];
			}
		}
		board->pieces[piece] ^= startBB;
		board->hash ^= zobristKeys[piece][startSquare];
		// Detecting captures
		if (isCapture(board, move)) {
			board->halfMoveCounter = 0;
			int capturedPiece = getPieceAtSquare(board, endSquare);
			board->pieces[capturedPiece] ^= endBB;

			if (capturedPiece == W_ROOK) {
				board->hash ^= zobristCastlingKeys[board->castling];
				if (endSquare == 0) {
					board->castling &= ~W_KS_CASTLING;
				} else if (endSquare == 7) {
					board->castling &= ~W_QS_CASTLING;
				}
				board->hash ^= zobristCastlingKeys[board->castling];
			} else if (capturedPiece == B_ROOK) {
				board->hash ^= zobristCastlingKeys[board->castling];
				if (endSquare == 56) {
					board->castling &= ~B_KS_CASTLING;
				} else if (endSquare == 63) {
					board->castling &= ~B_QS_CASTLING;
				}
				board->hash ^= zobristCastlingKeys[board->castling];
			}
			board->hash ^= zobristKeys[capturedPiece][endSquare];
		}
		if (moveIsPromotion(move)) {
			board->halfMoveCounter = 0;
			piece = turn ? W_PAWN : B_PAWN;
			board->pieces[piece+getPromotionPiece(move)] ^= endBB;
			board->hash ^= zobristKeys[piece+getPromotionPiece(move)][endSquare];
			return;
		}
		board->pieces[piece] ^= endBB;
		board->hash ^= zobristKeys[piece][endSquare];
		// Handling castling rights
		switch (piece) {
			case W_KING:
				board->hash ^= zobristCastlingKeys[board->castling];
				board->castling &= (B_KS_CASTLING | B_QS_CASTLING);
				board->hash ^= zobristCastlingKeys[board->castling];
				break;
			case B_KING:
				board->hash ^= zobristCastlingKeys[board->castling];
				board->castling &= (W_KS_CASTLING | W_QS_CASTLING);
				board->hash ^= zobristCastlingKeys[board->castling];
				break;
			case W_ROOK:
				if (startSquare == 0) {
					board->hash ^= zobristCastlingKeys[board->castling];
					board->castling &= ~W_KS_CASTLING;
					board->hash ^= zobristCastlingKeys[board->castling];
				} else if (startSquare == 7) {
					board->hash ^= zobristCastlingKeys[board->castling];
					board->castling &= ~W_QS_CASTLING;
					board->hash ^= zobristCastlingKeys[board->castling];
				}
				break;
			case B_ROOK:
				if (startSquare == 56) {
					board->hash ^= zobristCastlingKeys[board->castling];
					board->castling &= ~B_KS_CASTLING;
					board->hash ^= zobristCastlingKeys[board->castling];
				} else if (startSquare == 63) {
					board->hash ^= zobristCastlingKeys[board->castling];
					board->castling &= ~B_QS_CASTLING;
					board->hash ^= zobristCastlingKeys[board->castling];
				}
				break;
		}
	} else {
		board->hash ^= zobristCastlingKeys[board->castling];
		if (endSquare == 1) {
			board->castling &= (B_KS_CASTLING | B_QS_CASTLING);
			// Castling kingside
			board->pieces[W_KING] ^= 0x000000000000000A;
			board->pieces[W_ROOK] ^= 0x0000000000000005;
			board->hash ^= zobristKeys[W_KING][3] ^ zobristKeys[W_KING][1];
			board->hash ^= zobristKeys[W_ROOK][0] ^ zobristKeys[W_ROOK][2];
		} else if (endSquare == 5) {
			board->castling &= (B_KS_CASTLING | B_QS_CASTLING);
			// Castling queenside
			board->pieces[W_KING] ^= 0x0000000000000028;
			board->pieces[W_ROOK] ^= 0x0000000000000090;
			board->hash ^= zobristKeys[W_KING][3] ^ zobristKeys[W_KING][5];
			board->hash ^= zobristKeys[W_ROOK][7] ^ zobristKeys[W_ROOK][4];
		} else if (endSquare == 57) {
			board->castling &= (W_KS_CASTLING | W_QS_CASTLING);
			// Castling kingside
			board->pieces[B_KING] ^= 0x0A00000000000000;
			board->pieces[B_ROOK] ^= 0x0500000000000000;
			board->hash ^= zobristKeys[B_KING][59] ^ zobristKeys[B_KING][57];
			board->hash ^= zobristKeys[B_ROOK][56] ^ zobristKeys[B_ROOK][58];
		} else if (endSquare == 61) {
			board->castling &= (W_KS_CASTLING | W_QS_CASTLING);
			// Castling queenside
			board->pieces[B_KING] ^= 0x2800000000000000;
			board->pieces[B_ROOK] ^= 0x9000000000000000;
			board->hash ^= zobristKeys[B_KING][59] ^ zobristKeys[B_KING][61];
			board->hash ^= zobristKeys[B_ROOK][63] ^ zobristKeys[B_ROOK][60];
		}
		board->hash ^= zobristCastlingKeys[board->castling];
	}
}


void unmakeMove(Board *board, uint16_t move, Undo *undo) {
	board->numMoves--;
	if (board->turn) {
		board->fullMoveCounter--;
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
	int capturedPiece = undo->capturedPiece;
	bool turn = board->turn;

	board->turn = !turn;
	board->halfMoveCounter = undo->halfMoveCounter;
	board->epSquare = isEnPassant(move) ? endSquare : -1;
	board->castling = undo->castling;

	board->hash = board->history[board->numMoves];
	
	if (moveIsPromotion(move)) {
		piece = board->turn ? W_PAWN : B_PAWN;
		board->pieces[piece] ^= startBB;
		board->pieces[piece+getPromotionPiece(move)] ^= endBB;
		if (capturedPiece >= 0) {
			board->pieces[capturedPiece] ^= endBB;
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
		piece = getPieceAtSquare(board, endSquare);
		board->pieces[piece] ^= startBB ^ endBB;
		if (capturedPiece > -1) {
			board->pieces[capturedPiece] ^= endBB;
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
