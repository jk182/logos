#pragma once

#include "board.h"
#include "thread.h"


int qsearch(Thread *thread, int depth, int alpha, int beta);
int alphaBeta(Thread *thread, int depth, int alpha, int beta);
int iterativeDeepening(Thread *thread, int depth);
uint16_t findBestMove(Thread *thread, int depth);
uint16_t findGameMove(Thread *thread, int depth);
