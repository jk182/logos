#include <iostream>

#include "move.h"
#include "bitboard.h"


void printMove(uint16_t move) {
	// TODO: add promotion piece
	char* startSquare = getSquareName(move & 0x3F);
	char* endSquare = getSquareName((move & 0xFC0) >> 6);
	std::cout << startSquare << "->" << endSquare << "\n";
}


uint16_t makeMove(int startSqIndex, int endSqIndex) {
	uint16_t move = startSqIndex | (endSqIndex << 6);
	return move;
}


uint16_t makeMove(int startSqIndex, int endSqIndex, int promotionPiece) {
	uint16_t move = makeMove(startSqIndex, endSqIndex);
	move |= promotionPiece << 12;
	return move;
}
