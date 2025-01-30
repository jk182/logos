#pragma once


#include "board.h"

uint16_t* generateAllMoves(Board *board, uint16_t *moves);
uint16_t* moveOrdering(Board *board, uint16_t *moves, int limit);
int perft(int depth, Board *board);
