#include <stdint.h>
#include <iostream>

#include "board.h"
#include "bitboard.h"


void clearBoard(Board *board) {
	for (int i = 0; i < 12; i++) {
		board->pieces[i] = 0;
	}
	board->turn = true;
	board->moveCounter = 0;
	board->epSquare = -1;
	board->castling = 0;
}

void boardFromFEN(Board *board, const char *fen) {
	clearBoard(board);	
}

void printBoard(Board *board) {
	for (uint64_t bb : board->pieces) {
		printBitboard(bb);
		std::cout << "----------" << "\n";
	}
	std::cout << board->turn << ", ";
	std::cout << board->moveCounter << ", ";
	std::cout << board->epSquare << ", ";
	std::cout << board->castling << "\n";
}
