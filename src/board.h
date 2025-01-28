#pragma once

#include <stdint.h>


enum {
	WHITE = true,
	BLACK = false,
	HISTORY_LENGTH = 512
};


struct Board {
	uint64_t pieces[12];
	bool turn;
	int halfMoveCounter;
	int fullMoveCounter;
	int numMoves;
	int epSquare;
	uint16_t castling;
	uint64_t hash;
	uint64_t history[HISTORY_LENGTH];
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
bool isLegalPosition(Board *board);
int getPieceAtSquare(Board *board, int square);
Undo generateUndo(Board *board, uint16_t move);
bool isDraw(Board *board);
bool isCheck(Board *board);
bool isCheckmate(Board *board);
bool isGameOver(Board *board);
bool isLegalMove(Board board, uint16_t move);
bool isSamePosition(Board *b1, Board *b2);
