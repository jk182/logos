#include "board.h"
#include "evaluation.h"
#include "move.h"
#include "movegen.h"
#include "thread.h"

#include <iostream>

Thread* createThread() {
	Thread *thread = new Thread;
	Board board;
	clearBoard(&board);
	thread->board = board;
	thread->eval = 0;
	thread->nodes = 0ull;
	thread->depth = 0;
	thread->seldepth = 0;
	initTT(&(thread->tt));
	return thread;
}


Thread* createThread(Board *board) {
	Thread *thread = new Thread;
	thread->board = *board;
	thread->eval = 0;
	thread->nodes = 0ull;
	thread->depth = 0;
	thread->seldepth = 0;
	initTT(&(thread->tt));
	initNodeStack(thread);
	return thread;
}


void initNodeStack(Thread *thread) {
	Board *board = &(thread->board);
	thread->nodeStackHeight = 0;
	uint16_t move;
	uint16_t *moves = new uint16_t[MAX_MOVES];
	uint16_t *endMove = generateAllMoves(board, moves);
	int limit = endMove-moves;
	Undo undo;
	Node node;
	Node node2;
	for (int i = 0; i < limit; i++) {
		move = *(moves+i);
		makeMove(board, move, &undo);
		if (isLegalPosition(board)) {
			node.move = move;
			node.eval = evaluate(board);
			node.depth = 0;
			for (int j = 0; j < thread->nodeStackHeight; j++) {
				node2 = *(thread->nodeStack+j);
				if (node.eval > node2.eval) {
					*(thread->nodeStack+j) = node;
					node = node2;
				}
			}
			*(thread->nodeStack+thread->nodeStackHeight) = node;
			thread->nodeStackHeight++;
		}
		unmakeMove(board, move, &undo);
	}
	delete[] moves;
}


void updateNodeStack(Thread *thread, Node *nodes) {
	Node currentNode;
	Node node;
	for (int i = 0; i < thread->nodeStackHeight; i++) {
		currentNode = *(nodes+i);
		for (int j = i+1; j < thread->nodeStackHeight; j++) {
			node = *(nodes+j);
			if (node.eval > currentNode.eval && node.depth >= currentNode.depth) {
				*(nodes+i) = node;
				*(nodes+j) = currentNode;
				currentNode = node;
			}
		}
	}
	for (int i = 0; i < thread->nodeStackHeight; i++) {
		*(thread->nodeStack+i) = *(nodes+i);
		node = *(thread->nodeStack+i);
	}
}


uint16_t movePicker(Thread *thread) {
	int bestMoves = 1;
	int bestEval = (thread->nodeStack+0)->eval;
	if (bestEval >= MATE_SCORE) {
		return (thread->nodeStack+0)->move;
	}
	for (int i = 1; i < thread->nodeStackHeight; i++) {
		if ((thread->nodeStack+i)->eval == bestEval) {
			bestMoves++;
		} else {
			break;
		}
	}
	if (bestMoves == 1) {
		return (thread->nodeStack+0)->move;
	} 
	int randomIndex = std::rand() % bestMoves;
	return (thread->nodeStack+randomIndex)->move;
}
