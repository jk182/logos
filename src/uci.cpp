#include "board.h"
#include "move.h"
#include "movegen.h"
#include "search.h"
#include "uci.h"

#include <iostream>


int main() {
	Board board;
	clearBoard(&board);
	boardFromFEN(&board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

	std::string command;
	while (true) {
		std::getline(std::cin, command);
		if (command == "quit") {
			break;
		} else if (command == "uci") {
			std::cout << "id name Logos\n";
			std::cout << "id author Julian Kern\n";

			std::cout << "uciok\n";
		} else if (command == "isready") {
			std::cout << "readyok\n";
		} else if (command.starts_with("position")) {
			changePosition(command, &board);
		} else if (command.starts_with("go")) {
			searchPosition(command, &board);
		}
	}
	return 0;
}


void changePosition(std::string command, Board* board) {
	std::size_t space = command.find(" ");
	if (space != std::string::npos) {
		std::string token = command.substr(space+1);
		if (token.starts_with("fen")) {
			space = token.find(" ");
			if (space != std::string::npos) {
				boardFromFEN(board, token.substr(space+1).c_str());
			}
		} else if (token == "startpos") {
			boardFromFEN(board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
		}
		space = token.find(" ");
		while (space != std::string::npos) {
			token = token.substr(space+1);
			space = token.find(" ");
		}
	}
}


void searchPosition(std::string command, Board* board) {
	std::size_t space = command.find(" ");
}
