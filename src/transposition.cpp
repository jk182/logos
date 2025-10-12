#include "move.h"
#include "transposition.h"


void initTT(TranspositionTable *tt) {
	TTEntry entry;
	entry.zobristHash = 0;
	entry.depth = -1;
	entry.evaluation = UNKNOWN_EVAL;
	entry.bestMove = NULL_MOVE;
	for (int i = 0; i < TTSIZE; i++) {
		tt->ttTable[i] = entry;
	}
}


int probeTranspositionTable(TranspositionTable *tt, Board *board, int depth) {
	TTEntry entry = tt->ttTable[board->hash % TTSIZE];
	if (entry.zobristHash == board->hash && entry.depth >= depth) {
		return entry.evaluation;
	}
	return UNKNOWN_EVAL;
}


void updateTranspositionTable(TranspositionTable *tt, Board *board, int depth, int evaluation, uint16_t bestMove) {
	if (tt->ttTable[board->hash % TTSIZE].depth > depth) {
		return;
	}
	TTEntry entry;
	entry.zobristHash = board->hash;
	entry.depth = depth;
	entry.evaluation = evaluation;
	entry.bestMove = bestMove;
	tt->ttTable[board->hash % TTSIZE] = entry;
}
