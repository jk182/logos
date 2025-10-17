#pragma once

#include "board.h"

#include <stdint.h>

enum {
	TTSIZE =  8192,
	UNKNOWN_EVAL = 123456789,
	EXACT_FLAG = 0,
	ALPHA_FLAG = 1,
	BETA_FLAG = 2
};

struct TTEntry {
	uint64_t zobristHash;
	int depth;
	int evaluation;
	uint16_t bestMove;
	int flag;
};

struct TranspositionTable {
	TTEntry ttTable[TTSIZE];
};

void initTT(TranspositionTable *tt);
TTEntry probeTranspositionTable(TranspositionTable *tt, Board *board);
void updateTranspositionTable(TranspositionTable *tt, Board *board, int depth, int evaluation, uint16_t bestMove, int flag);
void printTTInfo(TranspositionTable *tt);
