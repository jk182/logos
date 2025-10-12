#pragma once

#include "board.h"

#include <stdint.h>

enum {
	TTSIZE =  1024,
	UNKNOWN_EVAL = 123456789
};

struct TTEntry {
	uint64_t zobristHash;
	int depth;
	int evaluation;
	uint16_t bestMove;
};

struct TranspositionTable {
	TTEntry ttTable[TTSIZE];
};

void initTT(TranspositionTable *tt);
int probeTranspositionTable(TranspositionTable *tt, Board *board, int depth);
void updateTranspositionTable(TranspositionTable *tt, Board *board, int depth, int evaluation, uint16_t bestMove);
