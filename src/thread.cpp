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
	return thread;
}


void updateNodeStack(Thread *thread, Node *nodes) {
	Node currentNode;
	Node node;
	for (int i = 0; i < thread->nodeStackHeight; i++) {
		currentNode = *(nodes+i);
		for (int j = i+1; j < thread->nodeStackHeight; j++) {
			node = *(nodes+j);
			if (node.eval > currentNode.eval) {
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
