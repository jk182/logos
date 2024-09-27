#include <stdint.h>
#include <iostream>


uint64_t* enumerateSubsets(uint64_t set) {
	uint64_t subsets[16384];
	int index = 0;
	uint64_t subset = 0;
	do {
		subsets[index] = subset;
		index++;
		std::cout << subset << "\n";
		subset = (subset-set) & set;
	} while (subset != 0);
	return subsets;
}


uint64_t generateRookMagic(int square) {
	uint64_t table[4096];
	return 0;
}
