#include <limits.h>
#include <algorithm>

#include "board.h"
#include "evaluation.h"
#include "move.h"
#include "movegen.h"


int alphaBeta(Board *board, int depth, int alpha, int beta) {
	if (depth == 0 || isGameOver(board)) {
		return evaluate(board);
	}
	int value;
	uint16_t moveArr[128];
	uint16_t *moves = moveArr;
	uint16_t *end = generateAllMoves(*board, moves);
	uint16_t move;
	Undo undo;

	if (board->turn == WHITE) {
		value = INT_MIN;
		for (int i = 0; i < end-moves; i++) {
			move = *(moves+i);
			undo = generateUndo(board, move);
			makeMove(board, move);
			value = std::max(value, alphaBeta(board, depth-1, alpha, beta));
			alpha = std::max(alpha, value);
			unmakeMove(board, move, &undo);
			if (alpha >= beta) {
				break;
			}
		}
	} else {
		value = INT_MAX;
		for (int i = 0; i < end-moves; i++) {
			move = *(moves+i);
			undo = generateUndo(board, move);
			makeMove(board, move);
			value = std::min(value, alphaBeta( board, depth-1, alpha, beta));
			beta = std::min(beta, value);
			unmakeMove(board, move, &undo);
			if (beta <= alpha) {
				break;
			}
		}
	}
}


int iterativeDeepening(Board *board, int depth) {
	if (depth == 0 || isGameOver(board)) {
		return evaluate(board);
	}
	int score;
	for (int d = 0; d <= depth; d++) {
		score = alphaBeta(board, d, INT_MIN, INT_MAX);
	}
	return score;
}
