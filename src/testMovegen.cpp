#include "board.h"
#include "movegen.h"

#include <cassert>
#include <iostream>

void startingPosition() {
	Board board;
	boardFromFEN(&board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	std::cout << "Testing starting position..." << "\n";
	assert(perft(1, &board) == 20);
	assert(perft(2, &board) == 400);
	assert(perft(3, &board) == 8902);
	assert(perft(4, &board) == 197281);
	assert(perft(5, &board) == 4865609);
}


void openings() {
	Board board;
	boardFromFEN(&board, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
	std::cout << "Testing opening positions..." << "\n";
	assert(perft(1, &board) == 48);
	assert(perft(2, &board) == 2039);
	assert(perft(3, &board) == 97862);
	assert(perft(4, &board) == 4085603);
	assert(perft(5, &board) == 193690690);
}


void pawnEndings() {
	Board board;
	boardFromFEN(&board, "8/8/8/8/8/K7/P7/k7 w - - 0 1");
	std::cout << "Testing pawn endings..." << "\n";
	assert(perft(1, &board) == 3);
	assert(perft(2, &board) == 7);
	assert(perft(3, &board) == 43);
	assert(perft(4, &board) == 199);
	assert(perft(5, &board) == 1347);
	boardFromFEN(&board, "8/2k1p3/3pP3/3P2K1/8/8/8/8 w - - 0 1");
	assert(perft(1, &board) == 7);
	assert(perft(2, &board) == 35);
	assert(perft(3, &board) == 210);
	assert(perft(4, &board) == 1091);
	assert(perft(5, &board) == 7028);
	boardFromFEN(&board, "8/3k4/3p4/8/3P4/3K4/8/8 b - - 0 1");
	assert(perft(1, &board) == 8);
	assert(perft(2, &board) == 61);
	assert(perft(3, &board) == 411);
	assert(perft(4, &board) == 3213);
	assert(perft(5, &board) == 21637);
}


void promotions() {
	Board board;
	boardFromFEN(&board, "8/Pk6/8/8/8/8/6Kp/8 w - - 0 1");
	std::cout << "Testing pawn promotions..." << "\n";
	assert(perft(1, &board) == 11);
	assert(perft(2, &board) == 97);
	assert(perft(3, &board) == 887);
	assert(perft(4, &board) == 8048);
	assert(perft(5, &board) == 90606);
	boardFromFEN(&board, "8/PPPk4/8/8/8/8/4Kppp/8 w - - 0 1");
	assert(perft(1, &board) == 18);
	assert(perft(2, &board) == 270);
	assert(perft(3, &board) == 4699);
	assert(perft(4, &board) == 79355);
	assert(perft(5, &board) == 1533145);
	boardFromFEN(&board, "n1n5/1Pk5/8/8/8/8/5Kp1/5N1N w - - 0 1");
	assert(perft(1, &board) == 24);
	assert(perft(2, &board) == 421);
	assert(perft(3, &board) == 7421);
	assert(perft(4, &board) == 124608);
	assert(perft(5, &board) == 2193768);
	boardFromFEN(&board, "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1");
	assert(perft(1, &board) == 24);
	assert(perft(2, &board) == 496);
	assert(perft(3, &board) == 9483);
	assert(perft(4, &board) == 182838);
	assert(perft(5, &board) == 3605103);
}


void testMovegen() {
	startingPosition();
	openings();
	pawnEndings();
	promotions();
	std::cout << "All test completed!" << "\n";
}
