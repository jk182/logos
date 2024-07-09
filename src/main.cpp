#include "bitboard.h"
#include "board.h"

#include <stdint.h>

int main() {
	uint64_t bb = 0x8040201001020408ull;
	printBitboard(bb);
	Board b;
	clearBoard(&b);
	printBoard(&b);
	return 0;
}
