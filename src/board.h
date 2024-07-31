#pragma once

#include <stdint.h>


struct Board {
	uint64_t pieces[12];
	bool turn;
	int halfMoveCounter;
	int fullMoveCounter;
	int epSquare;
	int castling;
};

void boardFromFEN(Board *board, const char *fen);
void clearBoard(Board *board);
void printBoard(Board *board);
uint64_t whitePieces(Board *board);
uint64_t blackPieces(Board *board);
uint64_t occupiedSquares(Board *board);
bool isLegalPosition(Board board);
