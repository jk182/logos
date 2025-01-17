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

	Board *history = new Board[512];
	Board *end;

	std::string command;
	while (true) {
		std::getline(std::cin, command);
		if (command == "quit") {
			break;
		} else if (command == "uci") {
			std::cout << "id name Logos\n";
			std::cout << "id author Julian Kern\n";

			std::cout << "uciok\n";
		} else if (command == "ucinewgame") {
			clearBoard(&board);
			boardFromFEN(&board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
		} else if (command == "isready") {
			std::cout << "readyok\n";
		} else if (command.starts_with("position")) {
			changePosition(command, &board);
			history = new Board[512];
			end = updateHistory(command, board, history);
		} else if (command.starts_with("go")) {
			uint16_t move = searchPosition(command, board, history, history-end);
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
				clearBoard(board);
				boardFromFEN(board, token.substr(space+1).c_str());
			}
		} else if (token.starts_with("startpos")) {
			clearBoard(board);
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


Board* updateHistory(std::string command, Board board, Board *history) {
	// history = new Board[256];
	std::size_t space = command.find(" ");
	uint16_t move;
	int index = 0;
	if (space != std::string::npos) {
		std::string token = command.substr(space+1);
		if (token.starts_with("fen")) {
			space = token.find(" ");
			if (space != std::string::npos) {
				boardFromFEN(&board, token.substr(space+1).c_str());
			}
		} else if (token.starts_with("startpos")) {
			boardFromFEN(&board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
		}
		space = token.find(" ");
		std::stringstream stream(token);
		std::string s;
		while (std::getline(stream, s, ' ')) {
			if (s == "startpos" || s == "moves") {
				continue;
			}
			move = encodeUCIMove(board, s.data());
			makeMove(&board, move);
			*(history+index) = board;
			index++;
		}
	}
	return history-index;
}


uint16_t searchPosition(std::string command, Board board, Board *history, int length) {
	int depth = -1;
	int time = -1;
	std::stringstream stream(command);
	std::string c;
	std::vector<std::string> vec;
	while (stream >> c) {
		vec.push_back(c);
	}
	for (std::size_t word = 0; word < vec.size(); word++) {
		if (vec[word] == "depth") {
			depth = std::stoi(vec[word+1]);
		} 
		if ((vec[word] == "wtime" && board.turn == WHITE) || (vec[word] == "btime" && board.turn == BLACK)) {
			time = std::stoi(vec[word+1]);
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
		
	std::cout << "Depth: " << depth << "\n";
	uint16_t move = findGameMove(&board, depth, history, length);
	if (! isLegalMove(board, move)) {
		printBoard(&board);
		std::cout << "ILLEGAL MOVE!\n";
		move = findGameMove(&board, depth-1, history, length);
		if (isLegalMove(board, move)) {
			return move;
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
