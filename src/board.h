#pragma once

#include <stdint.h>


enum {
	WHITE = true,
	BLACK = false
};


struct Board {
	uint64_t pieces[12];
	bool turn;
	int halfMoveCounter;
	int fullMoveCounter;
	int epSquare;
	uint16_t castling;
};

struct Undo {
	uint16_t castling;
	int halfMoveCounter;
	int capturedPiece;
};

void boardFromFEN(Board *board, const char *fen);
void clearBoard(Board *board);
void printBoard(Board *board);
uint64_t whitePieces(Board *board);
uint64_t blackPieces(Board *board);
uint64_t occupiedSquares(Board *board);
bool isLegalPosition(Board board);
int getPieceAtSquare(Board *board, int square);
Undo generateUndo(Board *board, uint16_t move);
bool isGameOver(Board *board);
