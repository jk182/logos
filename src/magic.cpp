#include <stdint.h>

uint64_t* enumerateSubsets(uint64_t set) {
	uint64_t subsets[16384];
	int index = 0;
	uint64_t subset = set;
	while (subset != 0) {
		subsets[index] = subset;
		index++;
		subset = (subset-set) && set;
	}
	return subsets;
}


uint64_t generateRookMagic(int square) {
	uint64_t table[4096];
	return 0;
}
