#include "bitboard.h"
#include "board.h"
#include "move.h"

#include <stdint.h>

int main() {
	uint64_t bb = 0x8040201001020408ull;
	// printBitboard(bb);
	Board b;
	clearBoard(&b);
	boardFromFEN(&b, "1nbqkbn1/rppppppr/p6p/8/8/P6P/RPPPPPPR/1NBQKBN1 w - e2 2 5");
	printBoard(&b);
	printMove(0xFC0);
	return 0;
}
