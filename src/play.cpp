#include "board.h"
#include "move.h"
#include "search.h"

#include <iostream>

void playGame(Board *board, int depth) {
	uint16_t engineMove;
	std::string UCIstr;
	char *UCI;
	uint16_t userMove;
	while (! isGameOver(board)) {
		engineMove = findBestMove(board, depth);
		makeMove(board, engineMove);
		printMove(engineMove);
		printBoard(board);
		std::cout << "Enter your move:" << "\n";
		std::cin >> UCIstr;
		UCI = UCIstr.data();
		userMove = encodeUCIMove(board, UCI);
		makeMove(board, userMove);
		printMove(userMove);
	}	
}
