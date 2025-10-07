#pragma once

#include "board.h"
#include "move.h"


struct Thread {
	Board board;
	int eval;
	uint64_t nodes;
	uint16_t PV[MAX_MOVES];
	int depth, seldepth;
	Undo undo;
};

Thread* createThread();
Thread* createThread(Board *board);
