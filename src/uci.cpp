#include "attacks.h"
#include "board.h"
#include "move.h"
#include "movegen.h"
#include "search.h"
#include "uci.h"

#include <iostream>
#include <sstream>
#include <string.h>
#include <vector>


int main() {
	initSlidingAttacks();
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
			uint16_t move = searchPosition(command, board);
			std::cout << "bestmove " << decodeMoveToUCI(move) << "\n";
		} else if (command.starts_with("debug")) {
			printBoard(&board);
		}
	}
	return 0;
}


void changePosition(std::string command, Board* board) {
	std::size_t space = command.find(" ");
	uint16_t move;
	if (space != std::string::npos) {
		std::string token = command.substr(space+1);
		if (token.starts_with("fen")) {
			space = token.find(" ");
			if (space != std::string::npos) {
				boardFromFEN(board, token.substr(space+1).c_str());
			}
		} else if (token.starts_with("startpos")) {
			boardFromFEN(board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
		}
		space = token.find(" ");
		std::stringstream stream(token);
		std::string s;
		while (std::getline(stream, s, ' ')) {
			if (s == "startpos" || s == "moves") {
				continue;
			}
			move = encodeUCIMove(*board, s.data());
			makeMove(board, move);
		}
	}
}


uint16_t searchPosition(std::string command, Board board) {
	int depth = -1;
	int time = -1;
	std::stringstream stream(command);
	std::string c;
	std::vector<std::string> vec;
	while (stream >> c) {
		vec.push_back(c);
	}
	for (int i = 0; i < vec.size(); i++) {
		if (vec[i] == "depth") {
			depth = std::stoi(vec[i+1]);
		} else if ((vec[i] == "wtime" && board.turn) || (vec[i] == "btime" && !board.turn)) {
			time = std::stoi(vec[i+1]);
		}
	}
	if (depth < 0) {
		if (time > 60000) {
			depth = 5;
		} else if (time > 30000) {
			depth = 4;
		} else if (time > 0) {
			depth = 3;
		} else {
			depth = 5;
		}
	}
		
	uint16_t move = findBestMove(&board, depth);
	if (! isLegalMove(board, move)) {
		printBoard(&board);
		printMove(move);
		uint16_t newMove = findBestMove(&board, depth-1);
		if (isLegalMove(board, newMove)) {
			return newMove;
		}
		uint16_t *moves = new uint16_t[MAX_MOVES];
		uint16_t *end = generateAllMoves(board, moves);
		int limit = end-moves;
		for (int i = 0; i < limit; i++) {
			move = *(moves+i);
			if (isLegalMove(board, move)) {
				break;
			}
		}
	} 
	return move;
}
