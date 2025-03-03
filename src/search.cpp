#include "bitboard.h"
#include "board.h"
#include "evaluation.h"
#include "move.h"
#include "movegen.h"
#include "search.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <limits.h>


int qsearch(Board *board, int depth, int alpha, int beta) {
	if (depth <= 0 || isGameOver(board)) {
		return evaluate(board);
	}
	if (isCheck(board)) {
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
	return standingPat;
	// return board->turn!=WHITE ? alpha : beta;
}


int alphaBeta(Board *board, int depth, int alpha, int beta) {
	if (depth <= 0) {
		return qsearch(board, 1, alpha, beta);
	}
	if (isCheckmate(board)) {
		int factor = board->turn ? 1 : -1;
		return evaluate(board)-depth*factor;
	}
	if (isDraw(board)) {
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
			// undo = generateUndo(board, move);
			makeMove(board, move, &undo);
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
			// undo = generateUndo(board, move);
			makeMove(board, move, &undo);
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
			value = alphaBeta(board, depth-1, alpha, beta);
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
	return bestMove;
}


uint16_t findGameMove(Board *board, int depth) {
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
			value = alphaBeta(board, depth-1, -MATE_SCORE, MATE_SCORE);
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
