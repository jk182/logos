#pragma once

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
