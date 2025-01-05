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
 	// boardFromFEN(&b, "r1bqk2r/pppp2pp/2n2n2/1Nb5/5B2/8/PPP1PPPP/R2QKBNR b KQkq - 5 6");
	// printBoard(&b);
	// printMove(findBestMove(&b, 4));
	// makeMove(&b, move);
	// printMove(findBestMove(&b, 4));
	return 0;
}
