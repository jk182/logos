#include "attacks.h"
#include "bitboard.h"

#include <iostream>
#include <random>
#include <stdint.h>


uint64_t* enumerateSubsets(uint64_t set, int indexBits) {
	uint64_t* subsets = new uint64_t[2<<indexBits];
	int index = 0;
	uint64_t subset = 0;
	do {
		subset = (subset-set) & set;
		subsets[index] = subset;
		index++;
	} while (subset != 0);
	return subsets;
}


uint64_t* makeLookupTable(uint64_t magic, uint64_t mask, int indexBits, int square) {
	uint64_t *table = new uint64_t[2<<indexBits];
	uint64_t *subsets = enumerateSubsets(mask, indexBits);
	uint64_t index;
	uint64_t attacks;
	int counter = 0;

	while (uint64_t blockers = *(subsets++)) {
		index = (blockers*magic) >> (64-indexBits);
		attacks = slidingAttacks((blockers | (2<<square)), square, mask);
		if (table[index] == 0) {
			table[index] = attacks;
			counter++;
		} else if (table[index] != attacks) {
			std::cout << "Test" << "\n";
			return nullptr;
		}
	}
	std::cout << counter << "\n";
	return table;
}


uint64_t generateRookMagic(int square) {
	uint64_t *table;
	uint64_t file = getFile(square);
	uint64_t rank = getRank(square);
	uint64_t mask = file ^ rank;
	uint64_t magic;
	
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<uint64_t> dis;

	do {
		magic = dis(gen) * dis(gen) * dis(gen);
		std::cout << magic << "\n";
		table = makeLookupTable(magic, mask, 14, square);
	} while (table == nullptr);
	return 0;
}
