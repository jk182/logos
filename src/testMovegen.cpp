#include "board.h"
#include "movegen.h"

#include <cassert>

void startingPosition() {
	Board board;
	boardFromFEN(&board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	assert(perft(1, &board) == 20);
	assert(perft(2, &board) == 400);
	assert(perft(3, &board) == 8902);
	assert(perft(4, &board) == 197281);
	assert(perft(5, &board) == 4865609);
}


void testMovegen() {
	startingPosition();
}
