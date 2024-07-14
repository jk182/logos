#include <assert.h>
#include <iostream>
#include <stdint.h>

#include "bitboard.h"

// Note that the squares are indexed h1=0, g1=2, ..., b8=62, a8=63

bool testBit(uint64_t bb, int i) {
	assert(i >= 0 && i < 64);
	return bb & (1ull << i);
}


void printBitboard(uint64_t bb) {
	for (int rank = 7; rank >= 0; rank--) {
		for (int file = 7; file >= 0; file--) {
			if (testBit(bb, rank*8+file)) {
				std::cout << "1";
			} else {
				std::cout << "0";
			}
		}
		std::cout << "\n";
	}
}


int getSquare(char file, int rank) {
	return 8*rank + (7-(file-'a'));
}


char* getSquareName(int square) {
	char* ret = new char;
	if (square > 63 || square < 0) {
		ret =  strdup("-");
	} else {
		int rank = square/8 + 1;
		char file = 'h' - (square%8);
		ret = strcat(new char(file), new char(rank + '0'));
	}

	return ret;
}


int getLSB(uint64_t *bb) {
	for (int i = 0; i < 64; i++) {
		if ((*bb & (1ull << i)) != 0) return i;
	}
	return -1;
}
