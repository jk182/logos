#include "bitboard.h"
#include "board.h"
#include "move.h"
#include "movegen.h"

#include <stdint.h>
#include <iostream>

int main() {
	uint64_t bb = 0x8040201001020409ull;
	// printBitboard(bb);
	Board b;
	clearBoard(&b);
	boardFromFEN(&b, "1nbqkbn1/rppppppr/p6p/8/3RB3/P6P/RPPP1PPR/1NBQKBN1 w - e2 2 5");
	printBoard(&b);
	printMove(0xFC0);

	uint16_t move = 0;
	uint16_t *moves = &move;
	// generatePawnMoves(&b, moves);
	generateBishopMoves(&b, moves);
	// generateRookMoves(&b, moves);
	// uint16_t *kMoves = generateKingMoves(&b, moves);
	// generatePawnMoves(&b, generateKnightMoves(&b, kMoves));
	while (uint16_t m = *(moves++)) {
		printMove(m); 
	}
	return 0;
}
