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
 	// boardFromFEN(&b, "rnb1k1nr/pppp2pp/3q4/8/1b6/P3PN2/1PPB1PPP/RN1QKB1R b KQkq - 0 8");
	// printBoard(&b);
	// printMove(findBestMove(&b, 4));
	// uint16_t move = findBestMove(&b, 6);
	// printMove(move);
	// makeMove(&b, move);
	// printMove(findBestMove(&b, 4));
	return 0;
}
