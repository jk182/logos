#include "attacks.h"
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
	boardFromFEN(&b, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	printBoard(&b);

	uint16_t move = 0x0649ull;
	uint16_t *moves = &move;
	// generateAllMoves(b, moves);
	// while (uint16_t m = *(moves++)) {
 	//	printMove(m);
	// }
	// generatePawnMoves(&b, moves);
	// generateBishopMoves(b, moves);
	// generateQueenMoves(b, moves);
	// generateRookMoves(&b, moves);
	// uint16_t *kMoves = generateKingMoves(&b, moves);
	// generatePawnMoves(&b, generateKnightMoves(&b, kMoves));
	std::cout << perft(1, &b) << "\n";
	return 0;
}
