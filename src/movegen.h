#pragma once


#include "board.h"

uint16_t* generatePawnMoves(Board board, uint16_t *moves);
uint16_t* generateKnightMoves(Board board, uint16_t *moves);
uint16_t* generateBishopMoves(Board board, uint16_t *moves);
uint16_t* generateRookMoves(Board board, uint16_t *moves);
uint16_t* generateQueenMoves(Board board, uint16_t *moves);
uint16_t* generateKingMoves(Board board, uint16_t *moves);
uint16_t* generateAllMoves(Board board, uint16_t *moves);
int perft(int depth, Board *board);
