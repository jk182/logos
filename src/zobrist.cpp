#include "types.h"
#include "zobrist.h"

#include <stdint.h>


uint64_t zobristKeys[PIECES][64];
uint64_t zobristEpKeys[8];
uint64_t zobristCastlingKeys[16];
uint64_t zobristTurnKey;

uint64_t randXOR() {
	static uint64_t seed = 9284357ull;

	seed ^= seed >> 12;
	seed ^= seed << 25;
	seed ^= seed >> 27;

	return seed * 2685821657736338717ull;
}

void initZobrist() {
	for (int piece = W_PAWN; piece <= B_KING; piece++) {
		for (int sq = 0; sq < 64; sq++) {
			zobristKeys[piece][sq] = randXOR();
		}
	}
	for (int file = 0; file < 8; file++) {
		zobristEpKeys[file] = randXOR();
	}
	for (int castling = 0; castling < 16; castling++) {
		zobristCastlingKeys[castling] = randXOR();
	}
	zobristTurnKey = randXOR();
}
