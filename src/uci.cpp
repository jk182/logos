#include "attacks.h"
#include "board.h"
#include "evaluation.h"
#include "move.h"
#include "movegen.h"
#include "search.h"
#include "thread.h"
#include "uci.h"
#include "zobrist.h"

#include <bitset>
#include <chrono>
#include <iostream>
#include <sstream>
#include <string.h>
#include <vector>


int main() {
	initSlidingAttacks();
	initZobrist();

	Board board;
	clearBoard(&board);
	boardFromFEN(&board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	Thread *thread = createThread(&board);

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
			clearBoard(&(thread->board));
			boardFromFEN(&(thread->board), "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
		} else if (command == "isready") {
			std::cout << "readyok\n";
		} else if (command.starts_with("position")) {
			changePosition(command, thread);
		} else if (command.starts_with("go")) {
			uint16_t move = searchPosition(command, thread);
			std::cout << "bestmove " << decodeMoveToUCI(move) << "\n";
		} else if (command.starts_with("debug")) {
			printBoard(&(thread->board));
		}
	}
	return 0;
}


void changePosition(std::string command, Thread *thread) {
	Board *board = &(thread->board);
	std::size_t space = command.find(" ");
	uint16_t move;
	if (space != std::string::npos) {
		std::string token = command.substr(space+1);
		if (token.starts_with("fen")) {
			space = token.find(" ");
			if (space != std::string::npos) {
				clearBoard(board);
				boardFromFEN(board, token.substr(space+1).c_str());
				// TODO: allow moves after FEN
				initNodeStack(thread);
				return;
			}
		} else if (token.starts_with("startpos")) {
			clearBoard(board);
			boardFromFEN(board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
		}
		space = token.find(" ");
		std::stringstream stream(token);
		std::string s;
		Undo undo;
		while (std::getline(stream, s, ' ')) {
			if (s == "startpos" || s == "moves") {
				continue;
			}
			move = encodeUCIMove(*board, s.data());
			makeMove(board, move, &undo);
			if (board->halfMoveCounter == 0) {
				board->numMoves = 0;
			}
		}
	}
	initNodeStack(thread);
}


uint16_t searchPosition(std::string command, Thread *thread) {
	int depth = -1;
	int time = -1;
	std::stringstream stream(command);
	std::string c;
	std::vector<std::string> vec;

	Board board = thread->board;

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
		if (time > 300000) {
			return timedSearch(thread, 10000);
		} else if (time > 180000) {
			return timedSearch(thread, 5000);
		} else if (time > 120000) {
			return timedSearch(thread, 4000);
		} else if (time > 60000) {
			return timedSearch(thread, 3000);
		} else if (time > 40000) {
			return timedSearch(thread, 2000);
		} else if (time > 30000) {
			return timedSearch(thread, 1000);
		} else if (time > 15000) {
			return timedSearch(thread, 500);
		} else if (time > 0) {
			return timedSearch(thread, 100);
		} else {
			std::cout << "No timed search" << "\n";
			depth = 5;
		}
	}
		
	std::cout << "Depth: " << depth << "\n";
	uint16_t move = findGameMove(thread, depth);
	if (! isLegalMove(board, move)) {
		printBoard(&board);
		std::cout << "ILLEGAL MOVE!\n";
		move = findGameMove(thread, depth-1);
		if (isLegalMove(board, move)) {
			return move;
		}
		uint16_t *moves = new uint16_t[MAX_MOVES];
		uint16_t *end = generateAllMoves(&board, moves);
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


uint16_t timedSearch(Thread *thread, int thinkingTime) {
	// The thinking time is in milliseconds
	int maxDepth = 30;
	Board *board = &(thread->board);

	auto startTime = std::chrono::system_clock::now();
	std::chrono::duration<double> depthTime;
	std::chrono::duration<double> totalTime;
	auto depthStart = std::chrono::system_clock::now();
	auto depthEnd = std::chrono::system_clock::now();

	uint16_t move;
	Undo undo;
	Node *nodes = new Node[thread->nodeStackHeight];
	Node node;
	int factor = board->turn ? 1 : -1;
	int beta = MATE_SCORE+10;

	for (int depth = 1; depth <= maxDepth; depth++) {
		int alpha = -MATE_SCORE-10;
		for (int i = 0; i < thread->nodeStackHeight; i++) {
			node = *(thread->nodeStack+i);
			move = node.move;
			makeMove(board, move, &undo);
			// if (isCheckmate(board)) { return move; }
			node.eval = -search(thread, depth-1, alpha, beta, -factor);
			updateTranspositionTable(&(thread->tt), board, depth-1, node.eval, node.move, EXACT_FLAG);
			unmakeMove(board, move, &undo);
			node.depth = depth;
			*(nodes+i) = node;

			alpha = std::max(alpha, node.eval);

			depthEnd = std::chrono::system_clock::now();
			totalTime = depthEnd-depthStart;
			if (totalTime.count() * 1000 > 0.95 * thinkingTime) {
				updateNodeStack(thread, nodes);
				std::cout << "Thinking time: " << thinkingTime/1000 << " Move time: " << totalTime.count() << " depth (partial): " << depth << "\n";
				std::cout << "Eval: " << (thread->nodeStack+0)->eval << "\n";
				return movePicker(thread);
			}
		}
		updateNodeStack(thread, nodes);

		depthEnd = std::chrono::system_clock::now();
		depthTime = depthEnd-depthStart;
		totalTime = depthEnd-startTime;
		if (totalTime.count() * 1000 > 0.85 * thinkingTime) {
			std::cout << "Thinking time: " << thinkingTime/1000 << " Move time: " << totalTime.count() << " depth: " << depth << "\n";
			std::cout << "Eval: " << (thread->nodeStack+0)->eval << "\n";
			return movePicker(thread);
		}
	}
	return movePicker(thread);
}
