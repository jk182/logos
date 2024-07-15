#pragma once

#include <stdint.h>


enum {
	A_FILE = 0x8080808080808080ull,
	B_FILE = 0x4040404040404040ull,
	C_FILE = 0x2020202020202020ull,
	D_FILE = 0x1010101010101010ull,
	E_FILE = 0x0808080808080808ull,
	F_FILE = 0x0404040404040404ull,
	G_FILE = 0x0202020202020202ull,
	H_FILE = 0x0101010101010101ull,

	RANK_1 = 0x00000000000000FFull,
	RANK_2 = 0x000000000000FF00ull,
	RANK_3 = 0x0000000000FF0000ull,
	RANK_4 = 0x00000000FF000000ull,
	RANK_5 = 0x000000FF00000000ull,
	RANK_6 = 0x0000FF0000000000ull,
	RANK_7 = 0x00FF000000000000ull,
	RANK_8 = 0xFF00000000000000ull
};


bool testBit(uint64_t bb, int i);
void printBitboard(uint64_t bb);
int getSquare(char file, int rank);
char* getSquareName(int square);
int popLSB(uint64_t *bb);
