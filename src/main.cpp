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

#include <iostream>
#include <limits.h>
#include <stdint.h>

int main() {
	initSlidingAttacks();
	// generateAllMagics();
	// testMagics();
	testMovegen();
	
	// Board b;
	// boardFromFEN(&b, "2k4r/1pprb1q1/3p4/p2P1Q2/8/2N4p/PPP1BPPP/R4RK1 w - - 0 22");
	// printBoard(&b);
	// printMove(findBestMove(&b, 4));
	// printMove(findBestMove(&b, 5));
	return 0;
}
