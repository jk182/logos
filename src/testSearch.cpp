#include "board.h"
#include "move.h"
#include "search.h"


#include <cassert>
#include <iostream>
#include <string.h>


void testBestMoves() {
	std::cout << "Starting best move tests...\n";
	Board b1;
	boardFromFEN(&b1, "8/6k1/p2p1r2/1ppBb1p1/4Pn2/P1P2R2/1P5r/3Q1RK1 b - - 13 39");
	assert(findBestMove(&b1, 5) == encodeUCIMove(b1, strdup("h2g2")));
	boardFromFEN(&b1, "2b2r2/5pk1/p4R2/3pN1Pp/P2P4/8/1p5K/1Qq5 w - - 4 39");
	assert(findBestMove(&b1, 5) == encodeUCIMove(b1, strdup("f6f7")));
	boardFromFEN(&b1, "r2q1rk1/pQp1b1pp/7n/4N3/4P3/1PN4P/P5P1/R1B2RK1 b - - 0 16");
	assert(findBestMove(&b1, 5) == encodeUCIMove(b1, strdup("d8d4")));
}


void speedTest() {
	std::cout << "Starting speed test...\n";
	Board board;
	boardFromFEN(&board, "nrk1qbbr/pppp2pp/2n2p2/4p3/2P1P3/1N3P2/PP1P2PP/1RKNQBBR b KQkq - 0 4");
	findBestMove(&board, 6);
}


void testSearch() {
	testBestMoves();
	speedTest();
	std::cout << "All search tests completed!\n";
}
