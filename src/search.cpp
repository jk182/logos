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
	thread->nodes ++;
	if (depth <= 0 || isGameOver(board)) {
		return evaluate(board);
	}
	/*
	if (isCheck(board)) {
		return alphaBeta(thread, 1, alpha, beta);
	}
	*/

	int bestScore = evaluate(board);
	if (bestScore >= beta) {
		return bestScore;
	}
	if (bestScore > alpha) {
		alpha = bestScore;
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
				score = -qsearch(thread, depth-1, -beta, -alpha);
				unmakeMove(board, move, &undo);
				if (score >= beta) {
					return score;
				}
				/*
				if (score > bestScore) {
					bestScore = score;
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
	return alpha;
	// return board->turn!=WHITE ? alpha : beta;
}


int alphaBeta(Thread *thread, int depth, int alpha, int beta) {
	Board *board = &(thread->board);
	if (depth <= 0) {
		thread->nodes ++;
		return evaluate(board);
		// return qsearch(thread, 1, alpha, beta);
	}
	if (isCheckmate(board)) {
		int factor = board->turn ? 1 : -1;
		thread->nodes ++;
		return evaluate(board)-depth*factor;
	}
	if (isDraw(board)) {
		thread->nodes++;
		return 0;
	}
	/*
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
	*/
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
				if (value >= beta) {
					// updateTranspositionTable(&(thread->tt), board,  depth-1, value, move, BETA_FLAG);
					unmakeMove(board, move, &undo);
					break;
				} else {
					// updateTranspositionTable(&(thread->tt), board,  depth-1, value, move, EXACT_FLAG);
					unmakeMove(board, move, &undo);
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
				if (value <= alpha) {
					// updateTranspositionTable(&(thread->tt), board,  depth-1, value, move, ALPHA_FLAG);
					unmakeMove(board, move, &undo);
					break;
				} else {
					// updateTranspositionTable(&(thread->tt), board,  depth-1, value, move, EXACT_FLAG);
					unmakeMove(board, move, &undo);
				}
			} else {
				unmakeMove(board, move, &undo);
			}
		}
	}
	delete[] moves;
	return value;
}


int search(Thread *thread, int depth, int alpha, int beta, int color) {
	Board *board = &(thread->board);
	if (depth <= 0) {
		// thread->nodes ++;
		// return evaluate(board) * color;
		
		if (color == 1) {
			return qsearch(thread, 1, alpha, beta);
		} else {
			return qsearch(thread, 1, -beta, -alpha);
		}
		
		// return qsearch(thread, 1, alpha, beta, color);
	}
	if (isCheckmate(board)) {
		thread->nodes ++;
		return (evaluate(board)-depth)*color;
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
		// move = *(moves+i);
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
	int ttFlag;
	if (value <= alpha) {
		ttFlag = ALPHA_FLAG;
	} else if (value <= beta) {
		ttFlag = BETA_FLAG;
	} else {
		ttFlag = EXACT_FLAG;
	}
	updateTranspositionTable(&(thread->tt), board, depth, value, NULL_MOVE, ttFlag);
	delete[] moves;
	return value;
}


int iterativeDeepening(Thread *thread, int depth) {
	Board *board = &(thread->board);
	if (depth == 0 || isGameOver(board)) {
		return evaluate(board);
	}
	int score = 0;
	Undo undo;
	uint16_t move;
	Node *nodes = new Node[thread->nodeStackHeight];
	Node node;
	for (int d = 1; d <= depth; d++) {
		for (int i = 0; i < thread->nodeStackHeight; i++) {
			node = *(thread->nodeStack+i);
			move = node.move;
			makeMove(board, move, &undo);
			node.eval = alphaBeta(thread, d-1, -MATE_SCORE, MATE_SCORE);
			updateTranspositionTable(&(thread->tt), board, d-1, node.eval, node.move, EXACT_FLAG);
			unmakeMove(board, move, &undo);
			node.move = move;
			node.depth = d;
			*(nodes+i) = node;
		}
		updateNodeStack(thread, nodes);
	}
	printMove((thread->nodeStack+0)->move);
	return score;
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
			// value = alphaBeta(thread, depth-1, alpha, beta);
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
	std::cout << bestValue << "\n";
	return bestMove;
}
