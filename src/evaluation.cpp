#include "bitboard.h"
#include "board.h"
#include "types.h"


int evaluate(Board *board) {
	return 1;
}


int* countMaterial(Board *board) {
	int material[PIECES] = {};
	uint64_t bb;
	
	for (int i = 0; i < PIECES; i++) {
		bb = board->pieces[i];
		while (popLSB(&bb) >= 0) {
			material[i] ++;
		}
	}
	return material;
}
