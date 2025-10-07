#include "bitboard.h"
#include "board.h"
#include "evaluation.h"
#include "move.h"
#include "movegen.h"
#include "search.h"
#include "thread.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <limits.h>


int qsearch(Thread *thread, int depth, int alpha, int beta) {
	Board *board = &(thread->board);
	if (depth <= 0 || isGameOver(board)) {
		thread->nodes ++;
		return evaluate(board);
	}
	if (isCheck(board)) {
		return alphaBeta(thread, 1, alpha, beta);
	}

	int standingPat = evaluate(board);
	if (board->turn == WHITE) {
		if (standingPat >= beta) {
			thread->nodes ++;
			return beta;
		}
		if (standingPat > alpha) {
			alpha = standingPat;
		}
	} else {
		if (standingPat <= alpha) {
			thread->nodes ++;
			return alpha;
		}
		if (standingPat < beta) {
			beta = standingPat;
		}
	}
	uint16_t *moves = new uint16_t[MAX_MOVES];
	uint16_t *endMove = generateAllMoves(board, moves); // TODO: generate only captures
	int limit = endMove-moves;
	uint16_t *orderedMoves = moveOrdering(board, moves, limit);
	uint16_t move;
	Undo undo;
	int score;

	for (int i = 0; i < limit; i++) {
		move = *(orderedMoves+i);
		// move = *(moves+i);
		if (isCapture(board, move)) {
			// undo = generateUndo(board, move);
			makeMove(board, move, &undo);
			if (isLegalPosition(board)) {
				score = qsearch(thread, depth-1, alpha, beta);
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
	delete[] moves;
	return standingPat;
	// return board->turn!=WHITE ? alpha : beta;
}


int alphaBeta(Thread *thread, int depth, int alpha, int beta) {
	if (depth <= 0) {
		return qsearch(thread, 1, alpha, beta);
	}
	Board *board = &(thread->board);
	if (isCheckmate(board)) {
		int factor = board->turn ? 1 : -1;
		thread->nodes ++;
		return evaluate(board)-depth*factor;
	}
	if (isDraw(board)) {
		thread->nodes ++;
		return 0;
	}
	int value;
	uint16_t *moves = new uint16_t[MAX_MOVES];
	uint16_t *end = generateAllMoves(board, moves);
	int limit = end-moves;
	uint16_t *orderedMoves = moveOrdering(board, moves, limit);
	uint16_t move;
	Undo undo;

	if (board->turn == WHITE) {
		value = -MATE_SCORE;
		for (int i = 0; i < limit; i++) {
			move = *(orderedMoves+i);
			// move = *(moves+i);
			makeMove(board, move, &undo);
			if (isLegalPosition(board)) {
				value = std::max(value, alphaBeta(thread, depth-1, alpha, beta));
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
			makeMove(board, move, &undo);
			if (isLegalPosition(board)) {
				value = std::min(value, alphaBeta(thread, depth-1, alpha, beta));
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
	delete[] moves;
	return value;
}


int iterativeDeepening(Thread *thread, int depth) {
	Board *board = &(thread->board);
	if (depth == 0 || isGameOver(board)) {
		return evaluate(board);
	}
	int score;
	for (int d = 0; d <= depth; d++) {
		score = alphaBeta(thread, d, -MATE_SCORE, MATE_SCORE);
	}
	return score;
}


uint16_t findBestMove(Thread *thread, int depth) {
	Board *board = &(thread->board);
	uint16_t bestMove = NULL_MOVE;

	uint16_t *moves = new uint16_t[MAX_MOVES];
	uint16_t *end = generateAllMoves(board, moves);
	int limit = end-moves;
	uint16_t move;
	bool side = board->turn;
	int factor = side==WHITE ? 1 : -1;
	int value;
	int bestValue;
	Undo undo;
	int alpha = -MATE_SCORE-10;
	int beta = MATE_SCORE+10;

	for (int i = 0; i < limit; i++) {
		move = *(moves+i);
		makeMove(board, move, &undo);
		if (isLegalPosition(board)) {
			value = alphaBeta(thread, depth-1, alpha, beta);
			/*
			if (!board->turn) {
				alpha = std::max(alpha, value);
			} else {
				beta = std::min(beta, value);
			}*/
			if (bestMove == NULL_MOVE) {
				bestMove = move;
				bestValue = value;
			}
			if (value * factor > bestValue * factor) {
				bestValue = value;
				bestMove = move;
			} else if (value == bestValue) {
				int random = std::rand() % 2;
				if (random == 0) {
					bestMove = move;
				}
			}
		}
		unmakeMove(board, move, &undo);
		if (alpha >= MATE_SCORE || beta <= -MATE_SCORE) {
			break;
		}
	}
	delete[] moves;
	return bestMove;
}


uint16_t findGameMove(Thread *thread, int depth) {
	Board *board = &(thread->board);
	uint16_t bestMove = NULL_MOVE;

	uint16_t *moves = new uint16_t[MAX_MOVES];
	uint16_t *end = generateAllMoves(board, moves);
	int limit = end-moves;
	uint16_t move;
	bool side = board->turn;
	int factor = side==WHITE ? 1 : -1;
	int value;
	int bestValue = board->turn ? -MATE_SCORE : MATE_SCORE;
	Undo undo;

	int standingPat = evaluate(board);
	for (int i = 0; i < limit; i++) {
		move = *(moves+i);
		// undo = generateUndo(board, move);
		makeMove(board, move, &undo);
		if (isLegalPosition(board)) {
			value = alphaBeta(thread, depth-1, -MATE_SCORE, MATE_SCORE);
			if (bestMove == NULL_MOVE) {
				bestMove = move;
				bestValue = value;
			}
			if (value * factor > bestValue * factor) {
				bestValue = value;
				bestMove = move;
			} else if (value == bestValue) {
				if (value*factor > 500) {
					if (board->halfMoveCounter == 0 || evaluate(board)*factor > standingPat*factor) {
						bestMove = move;
					}
				} else if (isCheck(board)) {
					int random = std::rand() % 5;
					if (random < 3) {
						bestMove = move;
					}
				} else {
					int random = std::rand() % 2;
					if (random == 0) {
						bestMove = move;
					}
				}
			} else if (value*factor > 1000 && bestValue*factor <= MATE_SCORE) {
				int random = std::rand() % 2;
				if (random == 0) {
					bestMove = move;
					bestValue = value;
				}
			} else if (value*factor > 500 && bestValue*factor < MATE_SCORE) {
				int random = std::rand() % 3;
				if (random == 0) {
					bestMove = move;
					bestValue = value;
				}
			}
		}
		unmakeMove(board, move, &undo);
	}
	std::cout << bestValue << "\n";
	return bestMove;
}
