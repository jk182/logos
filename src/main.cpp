#include "attacks.h"
#include "bitboard.h"
#include "board.h"
#include "evaluation.h"
#include "magic.h"
#include "move.h"
#include "movegen.h"
#include "play.h"
#include "search.h"
#include "testMovegen.h"

#include <iostream>
#include <limits.h>
#include <stdint.h>

int main() {
	initSlidingAttacks();
	// uint64_t bb = 0x8040201001020409ull;
	// printBitboard(bb);
	/*
	boardFromFEN(&b, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	boardFromFEN(&b, "6k1/2p2ppp/2Npb3/1r6/4P3/P1P5/r6P/1K1R3R w - - 1 23");
	*/
	// printBitboard(rookAttacks(&b, false));
	/*
	Board b;
	clearBoard(&b);
	boardFromFEN(&b, "rnbqkbnr/pppp1ppp/8/4p3/8/1P6/P1PPPPPP/RNBQKBNR w KQkq - 0 2");
	printBitboard(bishopAttacks(&b, true));
	boardFromFEN(&b, "rnbqkbnr/pppp1ppp/8/4p3/8/BP6/P1PPPPPP/RN1QKBNR b KQkq - 1 2");
	printBitboard(bishopAttacks(&b, true));
	*/
	// std::cout << perft(2, &b) << "\n";
	/*
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
	// generateAllMagics();
	// testMagics();
	testMovegen();
	uint64_t edges = A_FILE | H_FILE | RANK_1 | RANK_8;
	// uint64_t *subsets = enumerateSubsets((C_FILE ^ RANK_5) & ~edges, 15);
	// generateRookMagic(53);
	return 0;
}
