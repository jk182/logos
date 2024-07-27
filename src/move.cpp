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


uint16_t makeMove(int startSqIndex, int endSqIndex) {
	uint16_t move = startSqIndex | (endSqIndex << 6);
	return move;
}


uint16_t makeMove(int startSqIndex, int endSqIndex, int promotionPiece) {
	uint16_t move = makeMove(startSqIndex, endSqIndex);
	move |= promotionPiece << 12;
	return move;
}


void playMove(Board *board, uint16_t move) {
	int startSquare = (move & (~(1ull<<15))) & 0x3Full;
	int endSquare = (move & (~(1ull<<15))) >> 6;
	uint64_t startBB = 1ull << startSquare;
	uint64_t endBB = 1ull << endSquare;
	int piece;
	bool turn = board->turn;

	board -> epSquare = -1;
	if ((move & (1ull << 15)) == 0) {
		// move is not castling
		if (! turn) {
			board->fullMoveCounter++;
		}
		for (int i = 0; i < 12; i++) {
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
					break;
				}
			}
		}
		board->pieces[piece] ^= endBB;
		// Handling castling rights
		if (piece == W_KING) {
			board->castling &= 0b1100ull;
		}
		if (piece == B_KING) {
			board->castling &= 0b0011ull;
		}
		if (piece == W_ROOK) {
			if (startSquare == 0) {
				board->castling &= 0b1110ull;
			} else if (startSquare == 7) {
				board->castling &= 0b1101ull;
			}
		}
		if (piece == B_ROOK) {
			if (startSquare == 56) {
				board->castling &= 0b1011ull;
			} else if (startSquare == 63) {
				board->castling &= 0b0111ull;
			}
		}
	} else {

	}
}
