#include "board.h"
#include "thread.h"


Thread* createThread() {
	Thread *thread = new Thread;
	Board board;
	clearBoard(&board);
	thread->board = board;
	thread->eval = 0;
	thread->nodes = 0ull;
	thread->depth = 0;
	thread->seldepth = 0;
	return thread;
}


Thread* createThread(Board *board) {
	Thread *thread = new Thread;
	thread->board = *board;
	thread->eval = 0;
	thread->nodes = 0ull;
	thread->depth = 0;
	thread->seldepth = 0;
	return thread;
}
