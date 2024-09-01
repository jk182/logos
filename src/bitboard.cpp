#include <assert.h>
#include <iostream>
#include <stdint.h>

#include "bitboard.h"

// Note that the squares are indexed h1=0, g1=2, ..., b8=62, a8=63

const uint64_t FILES[8] = {H_FILE, G_FILE, F_FILE, E_FILE, D_FILE, C_FILE, B_FILE, A_FILE};
const uint64_t RANKS[8] = {RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8};
const uint64_t DIAGONALS[15] = {DIAG_1, DIAG_2, DIAG_3, DIAG_4, DIAG_5, DIAG_6, DIAG_7, DIAG_8, DIAG_9, DIAG_10, DIAG_11, DIAG_12, DIAG_13, DIAG_14, DIAG_15};
const uint64_t ANTIDIAGONALS[15] = {ADIAG_1, ADIAG_2, ADIAG_3, ADIAG_4, ADIAG_5, ADIAG_6, ADIAG_7, ADIAG_8, ADIAG_9, ADIAG_10, ADIAG_11, ADIAG_12, ADIAG_13, ADIAG_14, ADIAG_15};


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
				std::cout << ".";
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
	auto index = std::countr_zero(*bb);
	if (index == 64) {
		return -1;
	}
	*bb &= *bb - 1;
	return index;
}


uint64_t getRank(int square) {
	return RANKS[square/8];
}


uint64_t getFile(int square) {
	return FILES[square%8];
}


uint64_t reverse(uint64_t bb) {
	uint64_t reversed = 0ull;
	while (bb) {
		reversed |= 1ull << (63-popLSB(&bb));
	}
	return reversed;
}


uint64_t getDiagonal(int square) {
	return DIAGONALS[7-(square%8)+square/8];
}


uint64_t getAntidiagonal(int square) {
	return ANTIDIAGONALS[7-(square%8)+7-square/8];
}
