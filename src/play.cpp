#include "board.h"
#include "move.h"
#include "search.h"

#include <iostream>

void playGame(Board *board, int depth) {
	uint16_t engineMove;
	uint16_t userMove;
	while (! isGameOver(board)) {
		engineMove = findBestMove(board, depth);
		makeMove(board, engineMove);
		printMove(engineMove);
		printBoard(board);
		std::cout << "Enter your move:" << "\n";
		std::cin >> userMove;
		makeMove(board, userMove);
	}	
}
