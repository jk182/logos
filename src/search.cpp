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
	if (depth <= 0 || isGameOver(board)) {
		return evaluate(board);
	}
	if (isCheck(board)) {
		// printBoard(board);
		return alphaBeta(board, 1, alpha, beta);
	}
	/*
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
	*/
	uint16_t *moves = new uint16_t[MAX_MOVES];
	uint16_t *endMove = generateAllMoves(*board, moves); // TODO: generate only captures
	int limit = endMove-moves;
	uint16_t *orderedMoves = moveOrdering(board, moves, limit);
	uint16_t move;
	Undo undo;
	int score;

	for (int i = 0; i < limit; i++) {
		move = *(orderedMoves+i);
		// move = *(moves+i);
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
		return qsearch(board, 1, alpha, beta);
	}
	if (isGameOver(board)) {
		return evaluate(board);
	}
	int value;
	uint16_t *moves = new uint16_t[MAX_MOVES];
	uint16_t *end = generateAllMoves(*board, moves);
	int limit = end-moves;
	uint16_t *orderedMoves = moveOrdering(board, moves, limit);
	uint16_t move;
	Undo undo;

	if (board->turn == WHITE) {
		value = -MATE_SCORE;
		for (int i = 0; i < limit; i++) {
			move = *(orderedMoves+i);
			// move = *(moves+i);
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
		value = MATE_SCORE;
		for (int i = 0; i < limit; i++) {
			move = *(orderedMoves+i);
			// move = *(moves+i);
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
		score = alphaBeta(board, d, -MATE_SCORE, MATE_SCORE);
	}
	return score;
}


uint16_t findBestMove(Board *board, int depth) {
	uint16_t bestMove = NULL_MOVE;

	uint16_t *moves = new uint16_t[MAX_MOVES];
	uint16_t *end = generateAllMoves(*board, moves);
	int limit = end-moves;
	// uint16_t *orderedMoves = moveOrdering(board, moves, limit);
	uint16_t move;
	bool side = board->turn;
	int value;
	int bestValue;
	Undo undo;

	for (int i = 0; i < limit; i++) {
		/*
		if (i == 0) {
			if (side == WHITE) {
				bestValue = -10000000;
			} else {
				bestValue = 10000000;
			}
		}
		*/
		move = *(moves+i);
		undo = generateUndo(board, move);
		// printMove(move);
		makeMove(board, move);
		if (isLegalPosition(board)) {
			value = alphaBeta(board, depth-1, -MATE_SCORE, MATE_SCORE);
			// std::cout << value << " " << bestValue << "\n";
			if (bestMove == NULL_MOVE) {
				bestMove = move;
				bestValue = value;
			}
			/*
			if (i == 0) {
				bestValue = value;
				bestMove = move;
			}
			*/
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
	std::cout << bestValue << "\n";
	return bestMove;
}
