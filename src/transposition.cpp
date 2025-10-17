#include "move.h"
#include "transposition.h"

#include <iostream>


void initTT(TranspositionTable *tt) {
	TTEntry entry;
	entry.zobristHash = 0;
	entry.depth = -1;
	entry.evaluation = UNKNOWN_EVAL;
	entry.bestMove = NULL_MOVE;
	entry.flag = EXACT_FLAG;
	for (int i = 0; i < TTSIZE; i++) {
		tt->ttTable[i] = entry;
	}
}


TTEntry probeTranspositionTable(TranspositionTable *tt, Board *board) {
	return tt->ttTable[board->hash % TTSIZE];
}


void updateTranspositionTable(TranspositionTable *tt, Board *board, int depth, int evaluation, uint16_t bestMove, int flag) {
	if (tt->ttTable[board->hash % TTSIZE].depth > depth) { // TODO: maybe also overwrite non-exact values?
		return;
	}
	TTEntry entry;
	entry.zobristHash = board->hash;
	entry.depth = depth;
	entry.evaluation = evaluation;
	entry.bestMove = bestMove;
	entry.flag = flag;
	tt->ttTable[board->hash % TTSIZE] = entry;
}


void printTTInfo(TranspositionTable *tt) {
	int entries = 0;
	for (int i = 0; i < TTSIZE; i++) {
		if (tt->ttTable[i].evaluation != UNKNOWN_EVAL) {
			entries++;
		}
	}
	std::cout << "Transpotion table entries/total: " << entries << "/" << TTSIZE << "\n";
}
