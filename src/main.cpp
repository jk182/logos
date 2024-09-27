#include "attacks.h"
#include "bitboard.h"
#include "board.h"
#include "evaluation.h"
#include "move.h"
#include "movegen.h"
#include "play.h"
#include "search.h"
#include "testMovegen.h"

#include <iostream>
#include <limits.h>
#include <stdint.h>

int main() {
	// uint64_t bb = 0x8040201001020409ull;
	// printBitboard(bb);
	/*
	Board b;
	clearBoard(&b);
	boardFromFEN(&b, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	boardFromFEN(&b, "6k1/2p2ppp/2Npb3/1r6/4P3/P1P5/r6P/1K1R3R w - - 1 23");
	printBoard(&b);

	uint16_t move = 0x0649ull;
	uint16_t moveArr[40];
	*/
	// uint16_t *moves = generateAllMoves(b, moveArr);
	// while (uint16_t m = *(moves++)) {
 	//	printMove(m);
	// }
	// generatePawnMoves(&b, moves);
	// generateBishopMoves(b, moves);
	// generateQueenMoves(b, moves);
	// generateRookMoves(&b, moves);
	// uint16_t *kMoves = generateKingMoves(&b, moves);
	// generatePawnMoves(&b, generateKnightMoves(&b, kMoves));
	/*
	for (int d = 1; d <= 3; d++) {
		std::cout << "Depth " << d << ": " << perft(d, &b) << "\n";
	}
	std::cout << evaluate(&b) << "\n";
	std::cout << alphaBeta(&b, 4, INT_MIN, INT_MAX) << "\n";
	printMove(findBestMove(&b, 5));
	*/
	// playGame(&b, 5);
	testMovegen();
	return 0;
}
