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


int qsearch(Thread *thread, int depth, int alpha, int beta, int color) {
	Board *board = &(thread->board);
	thread->nodes ++;
	if (depth <= 0 || isGameOver(board)) {
		return evaluate(board) * color;
	}

	int standingPat = evaluate(board) * color;
	if (standingPat >= beta) {
		return standingPat;
	}
	if (standingPat > alpha) {
		alpha = standingPat;
	}

	uint16_t *moves = new uint16_t[MAX_MOVES];
	// uint16_t *endMove = generateNoisyMoves(board, moves);
	uint16_t *endMove = generateAllMoves(board, moves);
	int limit = endMove-moves;
	uint16_t *orderedMoves = moveOrdering(board, moves, limit);
	uint16_t move;
	Undo undo;
	int score;

	for (int i = 0; i < limit; i++) {
		move = *(orderedMoves+i);
		if (isCapture(board, move)) {
			makeMove(board, move, &undo);
			if (isLegalPosition(board)) {
				score = -qsearch(thread, depth-1, -beta, -alpha, -color);
				unmakeMove(board, move, &undo);
				if (score >= beta) {
					delete[] moves;
					return score;
				}
				/*
				if (score > standingPat) {
					standingPat = score;
				}
				*/
				if (score > alpha) {
					alpha = score;
				}
			} else {
				unmakeMove(board, move, &undo);
			}
		}
	}
	delete[] moves;
	return standingPat;
}


int search(Thread *thread, int depth, int alpha, int beta, int color) {
	if (depth <= 0) {
		/* TODO: search extra depth if the position is a check, but this doesn't pass the search tests
		if (isCheck(board)) {
			return search(thread, 1, alpha, beta, color);
		} else {
			return qsearch(thread, 1, alpha, beta, color);
		}
		*/
		return qsearch(thread, 1, alpha, beta, color);
	}
	Board *board = &(thread->board);
	if (isCheckmate(board)) {
		thread->nodes ++;
		return (evaluate(board)+depth)*color;
	}
	if (isDraw(board)) {
		thread->nodes++;
		return 0;
	}

	TTEntry entry = probeTranspositionTable(&(thread->tt), board);
	if (entry.zobristHash == board->hash && entry.depth >= depth) {
		if (entry.flag == EXACT_FLAG) {
			return entry.evaluation;
		}
		if (entry.flag == BETA_FLAG && entry.evaluation >= beta) {
			return entry.evaluation;
		}
		if (entry.flag == ALPHA_FLAG && entry.evaluation <= alpha) {
			return entry.evaluation;
		}
	}

	int value;
	uint16_t *moves = new uint16_t[MAX_MOVES];
	uint16_t *end = generateAllMoves(board, moves);
	int limit = end-moves;
	uint16_t *orderedMoves = moveOrdering(board, moves, limit);
	uint16_t move;
	Undo undo;

	value = -MATE_SCORE;
	for (int i = 0; i < limit; i++) {
		move = *(orderedMoves+i);
		makeMove(board, move, &undo);
		if (isLegalPosition(board)) {
			value = std::max(value, -search(thread, depth-1, -beta, -alpha, -color));
			alpha = std::max(alpha, value);
			unmakeMove(board, move, &undo);
			if (alpha >= beta) {
				break;
			}
		} else {
			unmakeMove(board, move, &undo);
		}
	}
	delete[] moves;

	int ttFlag;
	if (value <= alpha) {
		ttFlag = ALPHA_FLAG;
	} else if (value <= beta) {
		ttFlag = BETA_FLAG;
	} else {
		ttFlag = EXACT_FLAG;
	}
	updateTranspositionTable(&(thread->tt), board, depth, value, NULL_MOVE, ttFlag);

	return value;
}


int iterativeDeepening(Thread *thread, int depth) {
	Board *board = &(thread->board);
	if (depth == 0 || isGameOver(board)) {
		return evaluate(board);
	}
	Undo undo;
	uint16_t move;
	Node *nodes = new Node[thread->nodeStackHeight];
	int colorFactor = board->turn ? -1 : 1;
	Node node;
	for (int d = 1; d <= depth; d++) {
		for (int i = 0; i < thread->nodeStackHeight; i++) {
			node = *(thread->nodeStack+i);
			move = node.move;
			makeMove(board, move, &undo);
			node.eval = search(thread, d-1, -MATE_SCORE, MATE_SCORE, colorFactor) * colorFactor;
			updateTranspositionTable(&(thread->tt), board, d-1, node.eval, node.move, EXACT_FLAG);
			unmakeMove(board, move, &undo);
			node.move = move;
			node.depth = d;
			*(nodes+i) = node;
		}
		updateNodeStack(thread, nodes);
	}
	// printMove((thread->nodeStack+0)->move);
	delete[] nodes;
	return (thread->nodeStack+0)->eval;
}


uint16_t findBestMove(Thread *thread, int depth) {
	Board *board = &(thread->board);
	uint16_t bestMove = NULL_MOVE;

	uint16_t *moves = new uint16_t[MAX_MOVES];
	uint16_t *end = generateAllMoves(board, moves);
	int limit = end-moves;
	uint16_t move;
	int factor = board->turn ? 1 : -1;
	int value;
	int bestValue;
	Undo undo;
	int alpha = -MATE_SCORE-10;
	int beta = MATE_SCORE+10;

	for (int i = 0; i < limit; i++) {
		move = *(moves+i);
		makeMove(board, move, &undo);
		if (isLegalPosition(board)) {
			value = search(thread, depth-1, alpha, beta, -factor) * -factor;
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

	for (int i = 0; i < limit; i++) {
		move = *(moves+i);
		makeMove(board, move, &undo);
		if (isLegalPosition(board)) {
			value = search(thread, depth-1, -MATE_SCORE, MATE_SCORE, -factor) * -factor;
			if (bestMove == NULL_MOVE) {
				bestMove = move;
				bestValue = value;
			}
			if (value * factor > bestValue * factor) {
				bestValue = value;
				bestMove = move;
			} else if (value == bestValue) {
				if (board->halfMoveCounter == 0 && value*factor > 300) {
					bestMove = move;
					bestValue = value;
				} else {
					int random = std::rand() % 2;
					if (random == 0) {
						bestMove = move;
						bestValue = value;
					}
				}
			}
		}
		unmakeMove(board, move, &undo);
	}
	delete[] moves;
	std::cout << bestValue << "\n";
	return bestMove;
}
