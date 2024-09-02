#pragma once

#include "board.h"


int alphaBeta(Board *board, int depth, int alpha, int beta);
int iterativeDeepening(Board *board, int depth);
uint16_t findBestMove(Board *board, int depth);
