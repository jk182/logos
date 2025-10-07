#pragma once

#include "board.h"


struct Thread {
	Board board;
};

Thread* createThread();
Thread* createThread(Board *board);
