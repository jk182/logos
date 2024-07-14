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
	boardFromFEN(&b, "1nbqkbn1/rppppppr/p6p/8/8/P6P/RPPPPPPR/1NBQKBN1 w - e2 2 5");
	printBoard(&b);
	printMove(0xFC0);
	std::cout << getLSB(&bb) << "\n";

	uint16_t move = 1;
	uint16_t *moves = &move;
	generateKingMoves(&b, moves);
	while (uint16_t m = *(moves++)) {
		std::cout << m << "\n";
	}
	return 0;
}
