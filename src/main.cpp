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
	// generateAllMagics();
	// testMagics();
	testMovegen();
	
	// Board b;
	// clearBoard(&b);
	// boardFromFEN(&b, "r1bqk2r/2p1bppp/p1np1n2/1p2p3/4P3/1B3N2/PPPP1PPP/RNBQR1K1 w kq - 0 8");
	// std::cout << alphaBeta(&b, 7, INT_MIN, INT_MAX) << "\n";
	// boardFromFEN(&b, "2rq1rk1/3nb1pp/p1ppbn2/1p2pp2/2P2P2/1PNPP1P1/PB1QN1BP/R4R1K b - - 1 13");
	// std::cout << alphaBeta(&b, 7, INT_MIN, INT_MAX) << "\n";
	// boardFromFEN(&b, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	// boardFromFEN(&b, "r5k1/p1p4p/2p2Q2/7q/5P2/4P3/PP4R1/1R4K1 b - - 0 32");
	// printMove(findBestMove(&b, 5));
	// playGame(&b, 5);
	// printMove(0b110010010110);
	// std::cout << isCapture(&b, 0b110010010110) << "\n";
	// boardFromFEN(&b, "3k4/3q4/8/8/8/3Q4/3K4/8 w - - 0 1");
	// std::cout << alphaBeta(&b, 1, INT_MIN, INT_MAX) << "\n";
	return 0;
}
