#pragma once

#include "board.h"


bool isSquareAttacked(Board board, bool turn, int square);
uint64_t getAttacks(Board board, bool turn);
uint64_t pawnAttacks(Board board, bool turn);
uint64_t knightAttacks(Board board, bool turn);
uint64_t bishopAttacks(Board board, bool turn);
uint64_t rookAttacks(Board board, bool turn);
uint64_t queenAttacks(Board board, bool turn);
uint64_t kingAttacks(Board board, bool turn);
