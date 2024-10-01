#pragma once

#include <iostream>

uint64_t generateRookMagic(int square);
uint64_t generateBishopMagic(int square);
uint64_t* enumerateSubsets(uint64_t set, int indexBits);
void generateAllMagics();
uint64_t* makeLookupTable(uint64_t magic, uint64_t direction1, uint64_t direction2, int indexBits, int square);
