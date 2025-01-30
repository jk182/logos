#pragma once

#include "board.h"


int qsearch(Board *board, int depth, int alpha, int beta);
int alphaBeta(Board *board, int depth, int alpha, int beta);
int iterativeDeepening(Board *board, int depth);
uint16_t findBestMove(Board *board, int depth);
uint16_t findGameMove(Board *board, int depth);
