#include "bitboard.h"
#include "board.h"
#include "evaluation.h"
#include "move.h"
#include "movegen.h"
#include "search.h"

#include <algorithm>
#include <iostream>
#include <limits.h>


int qsearch(Board *board, int depth, int alpha, int beta) {
	if (depth <= 0) {
		return evaluate(board);
	}
	if (isCheck(board)) {
		// printBoard(board);
		return alphaBeta(board, 1, alpha, beta);
	}
	uint16_t moveArr[MAX_MOVES];
	uint16_t *moves = moveArr;
	uint16_t *endMove = generateAllMoves(*board, moves); // TODO: generate only captures
	uint16_t move;
	Undo undo;
	int value;

	if (board->turn == WHITE) {
		for (int i = 0; i < endMove-moves; i++) {
			move = *(moves+i);
			if (isCapture(board, move)) {
				undo = generateUndo(board, move);
				makeMove(board, move);
				if (isLegalPosition(*board)) {
					value = qsearch(board, depth-1, alpha, beta);
					alpha = std::max(alpha, value);
					unmakeMove(board, move, &undo);
					if (value >= beta) {
						return beta;
					}
				} else {
					unmakeMove(board, move, &undo);
				}
			}
		}
	} else {
		for (int i = 0; i < endMove-moves; i++) {
			move = *(moves+i);
			if (isCapture(board, move)) {
				undo = generateUndo(board, move);
				makeMove(board, move);
				if (isLegalPosition(*board)) {
					value = qsearch(board, depth-1, alpha, beta);
					alpha = std::min(beta, value);
					unmakeMove(board, move, &undo);
					if (value <= alpha) {
						return alpha;
					}
				} else {
					unmakeMove(board, move, &undo);
				}
			}
		}
	}
	return evaluate(board);
}


int alphaBeta(Board *board, int depth, int alpha, int beta) {
	if (isGameOver(board)) {
		return evaluate(board);
	}
	if (depth <= 0) {
		return qsearch(board, 2, alpha, beta);
	}
	int value;
	uint16_t moveArr[MAX_MOVES];
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
			if (isLegalPosition(*board)) {
				value = std::max(value, alphaBeta(board, depth-1, alpha, beta));
				alpha = std::max(alpha, value);
				unmakeMove(board, move, &undo);
				if (value >= beta) {
					break;
				}
			} else {
				unmakeMove(board, move, &undo);
			}
		}
	} else {
		value = INT_MAX;
		for (int i = 0; i < end-moves; i++) {
			move = *(moves+i);
			undo = generateUndo(board, move);
			makeMove(board, move);
			if (isLegalPosition(*board)) {
				value = std::min(value, alphaBeta(board, depth-1, alpha, beta));
				beta = std::min(beta, value);
				unmakeMove(board, move, &undo);
				if (value <= alpha) {
					break;
				}
			} else {
				unmakeMove(board, move, &undo);
			}
		}
	}
	return value;
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


uint16_t findBestMove(Board *board, int depth) {
	// TODO: adjust for side to move
	uint16_t bestMove;

	uint16_t moveArr[MAX_MOVES];
	uint16_t *moves = moveArr;
	uint16_t *end = generateAllMoves(*board, moves);
	uint16_t move;
	int value;
	int maxValue;
	Undo undo;

	for (int i = 0; i < end-moves; i++) {
		if (i == 0) {
			maxValue = -100000;
			bestMove = NULL_MOVE;
		}
		move = *(moves+i);
		undo = generateUndo(board, move);
		makeMove(board, move);
		if (isLegalPosition(*board)) {
			value = alphaBeta(board, depth-1, INT_MIN, INT_MAX);
			if (i == 0) {
				maxValue = value;
				bestMove = move;
			}
			if (value > maxValue) {
				maxValue = value;
				bestMove = move;
			}
		}
		unmakeMove(board, move, &undo);
	}
	return bestMove;
}
