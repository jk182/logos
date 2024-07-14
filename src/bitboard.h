#pragma once

#include <stdint.h>

bool testBit(uint64_t bb, int i);
void printBitboard(uint64_t bb);
int getSquare(char file, int rank);
char* getSquareName(int square);
int getLSB(uint64_t *bb);
