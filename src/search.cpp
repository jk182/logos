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
	int standingPat = evaluate(board);

	if (board->turn == WHITE) {
		if (standingPat >= beta) {
			return beta;
		}
		if (standingPat > alpha) {
			alpha = standingPat;
		}
	} else {
		if (standingPat <= alpha) {
			return alpha;
		}
		if (standingPat < beta) {
			beta = standingPat;
		}
	}

	uint16_t *moves = new uint16_t[MAX_MOVES];
	uint16_t *endMove = generateAllMoves(*board, moves); // TODO: generate only captures
	uint16_t move;
	Undo undo;
	int score;

	for (int i = 0; i < endMove-moves; i++) {
		move = *(moves+i);
		if (isCapture(board, move)) {
			undo = generateUndo(board, move);
			makeMove(board, move);
			if (isLegalPosition(board)) {
				score = qsearch(board, depth-1, alpha, beta);
				unmakeMove(board, move, &undo);
				if (board->turn != WHITE) {	// Reverse side to move since a move was made
					if (score >= beta) {
						return beta;
					}
					if (score > alpha) {
						alpha = score;
					}
				} else {
					if (score <= alpha) {
						return alpha;
					}
					if (score < beta) {
						beta = score;
					}
				}
			} else {
				unmakeMove(board, move, &undo);
			}
		}
	}
	return board->turn!=WHITE ? alpha : beta;
}


int alphaBeta(Board *board, int depth, int alpha, int beta) {
	if (depth <= 0) {
		return qsearch(board, 2, alpha, beta);
	}
	if (isGameOver(board)) {
		return evaluate(board);
	}
	int value;
	uint16_t *moves = new uint16_t[MAX_MOVES];
	uint16_t *end = generateAllMoves(*board, moves);
	uint16_t *orderedMoves = moveOrdering(board, moves);
	uint16_t move;
	Undo undo;

	if (board->turn == WHITE) {
		value = INT_MIN;
		for (int i = 0; i < end-moves; i++) {
			move = *(orderedMoves+i);
			undo = generateUndo(board, move);
			makeMove(board, move);
			if (isLegalPosition(board)) {
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
			move = *(orderedMoves+i);
			undo = generateUndo(board, move);
			makeMove(board, move);
			if (isLegalPosition(board)) {
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
	uint16_t bestMove;

	uint16_t *moves = new uint16_t[MAX_MOVES];
	uint16_t *end = generateAllMoves(*board, moves);
	uint16_t *orderedMoves = moveOrdering(board, moves);
	uint16_t move;
	bool side = board->turn;
	int value;
	int bestValue;
	Undo undo;

	for (int i = 0; i < end-moves; i++) {
		if (i == 0) {
			if (side == WHITE) {
				bestValue = -10000000;
			} else {
				bestValue = 10000000;
			}
			bestMove = NULL_MOVE;
		}
		move = *(orderedMoves+i);
		undo = generateUndo(board, move);
		makeMove(board, move);
		if (isLegalPosition(board)) {
			value = alphaBeta(board, depth-1, INT_MIN, INT_MAX);
			if (bestMove == NULL_MOVE) {
				bestMove = move;
			}
			if (i == 0) {
				bestValue = value;
				bestMove = move;
			}
			if (side == WHITE && value > bestValue) {
				bestValue = value;
				bestMove = move;
			} else if (side == BLACK && value < bestValue) {
				bestValue = value;
				bestMove = move;
			}
		}
		unmakeMove(board, move, &undo);
	}
	return bestMove;
}
