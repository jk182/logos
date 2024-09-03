#pragma once

#include "board.h"

// Move format: 
// 	bits 0-5:	start square
// 	bits 6-11:	end square
// 	bits 12-14:	promotion piece (1-4)
// 			5: remove KS castling, 6: remove QS castling
// 			7: en-passant move
// 	bit 15:		castling move

enum {
	NULL_MOVE = 0,
	W_KS_CASTLING = 0x1,
	W_QS_CASTLING = 0x2,
	B_KS_CASTLING = 0x4,
	B_QS_CASTLING = 0x8,

	REM_KS_CASTLING = 0x5000,
	REM_QS_CASTLING = 0x6000,
	ENPASSANT = 0x7000
};



void printMove(uint16_t move);
int getStartSquare(uint16_t move);
int getEndSquare(uint16_t move);
bool moveIsPromotion(uint16_t move);
uint16_t encodeMove(int startSqIndex, int endSqIndex);
uint16_t encodeMove(int startSqIndex, int endSqIndex, int promotionPiece);
uint16_t encodeCastlingMove(int startSqIndex, int endSqIndex);
uint16_t encodeEPMove(int startSqInex, int endSqIndex);
uint16_t encodeUCIMove(Board *board, char *UCImove);
bool isEnPassant(uint16_t move);
void makeMove(Board *board, uint16_t move);
void unmakeMove(Board *board, uint16_t move, Undo *undo);
