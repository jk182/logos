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
	if (board->turn == WHITE) {
		value = INT_MIN;
		for (int i = 0; i < end-moves; i++) {
			move = *(moves+i);
			makeMove(board, move);
			value = std::max(value, alphaBeta(board, depth-1, alpha, beta));
			alpha = std::max(alpha, value);
			if (alpha >= beta) {
				break;
			}
		}
	} else {
		value = INT_MAX;
		for (int i = 0; i < end-moves; i++) {
			move = *(moves+i);
			makeMove(board, move);
			value = std::min(value, alphaBeta( board, depth-1, alpha, beta));
			beta = std::min(beta, value);
			if (beta <= alpha) {
				break;
			}
		}
	}
}
