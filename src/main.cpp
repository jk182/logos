#include "attacks.h"
#include "bitboard.h"
#include "board.h"
#include "evaluation.h"
#include "magic.h"
#include "move.h"
#include "movegen.h"
#include "play.h"
#include "search.h"
#include "testMovegen.h"
#include "testSearch.h"
#include "testZobrist.h"
#include "thread.h"
#include "zobrist.h"

#include <iostream>
#include <limits.h>
#include <stdint.h>

int main() {
	initSlidingAttacks();
	initZobrist();
	// generateAllMagics();
	// testMagics();
	// testMovegen();
	testSearch();
	// testZobrist();

	// Board b;
	// boardFromFEN(&b, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
 	// boardFromFEN(&b, "4r1k1/2pb1ppp/p1n2b2/3P3P/3P4/P3PBP1/1r1NN3/2R1K2R b - - 0 36");
	// Thread *thread = createThread(&b);
	// std::cout << search(thread, 7, -MATE_SCORE, MATE_SCORE, -1) << "\n";
	// printMove(findBestMove(thread, 6));
	// printTTInfo(&(thread->tt));
	// printMove(findBestMove(thread, 3));
	// Thread *thread2 = createThread(&b);
	// std::cout << alphaBeta(thread2, 6, -MATE_SCORE, MATE_SCORE) << "\n";
	// printMove(findBestMove(thread, 7));
	// std::cout << thread->nodes << "\n";
	// std::cout << perft(6, &b) << "\n";
	// boardFromFEN(&b, "8/4k3/8/3q4/8/4N3/4K3/8 w - - 0 1");
	// Thread *thread2 = createThread(&b);
	// printBoard(&b);
 	// boardFromFEN(&b, "r1bqk2r/pppp2pp/2n2n2/1Nb5/5B2/8/PPP1PPPP/R2QKBNR b KQkq - 5 6");
	// printBoard(&b);
	// printMove(findBestMove(&b, 4));
	// makeMove(&b, move);
	// printMove(findBestMove(&b, 4));
	return 0;
}
