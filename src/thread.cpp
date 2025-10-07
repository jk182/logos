#include "board.h"
#include "thread.h"


Thread* createThread() {
	Thread* thread = new Thread;
	Board board;
	clearBoard(&board);
	thread->board = board;
	return thread;
}
