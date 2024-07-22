#include <assert.h>
#include <iostream>
#include <stdint.h>

#include "bitboard.h"

// Note that the squares are indexed h1=0, g1=2, ..., b8=62, a8=63

const uint64_t FILES[8] = {H_FILE, G_FILE, F_FILE, E_FILE, D_FILE, C_FILE, B_FILE, A_FILE};
const uint64_t RANKS[8] = {RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8};

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


int popLSB(uint64_t *bb) {
	for (int i = 0; i < 64; i++) {
		if ((*bb & (1ull << i)) != 0){
			*bb ^= (1ull << i);
			return i;
		}
	}
	return -1;
}


uint64_t getRank(int square) {
	return RANKS[square/8];
}


uint64_t getFile(int square) {
	return FILES[square%8];
}


uint64_t reverse(uint64_t *bb) {
	uint64_t original = *bb;
	uint64_t reversed = 0ull;
	for(int i = 0; i < 64; i++) {
		reversed <<= 1;
		if ((original & 1) == 1) {
			reversed ^= 1;
		}
		original >>= 1;
	}
	return reversed;
}
