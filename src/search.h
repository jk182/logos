#pragma once

#include "board.h"
#include "thread.h"


int search(Thread *thread, int depth, int alpha, int beta, int color);
int qsearch(Thread *thread, int depth, int alpha, int beta, int  color);
int iterativeDeepening(Thread *thread, int depth);
uint16_t findBestMove(Thread *thread, int depth);
uint16_t findGameMove(Thread *thread, int depth);
