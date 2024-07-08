#include "bitboard.h"
#include <stdint.h>

int main() {
	uint64_t bb = 0x8040201001020408ull;
	printBitboard(bb);
	return 0;
}
