#include "attacks.h"
#include "bitboard.h"

#include <iostream>
#include <random>
#include <stdint.h>


uint64_t* enumerateSubsets(uint64_t set, int indexBits) {
	uint64_t* subsets = new uint64_t[1ull << indexBits];
	int index = 0;
	uint64_t subset = 0;
	do {
		subset = (subset-set) & set;
		subsets[index] = subset;
		index++;
	} while (subset != 0);
	return subsets;
}


uint64_t* makeLookupTable(uint64_t magic, uint64_t direction1, uint64_t direction2, int indexBits, int square) {
	// Directions are either the file and rank of a rook or the two diagonals of a bishop
	uint64_t *table = new uint64_t[1ull << indexBits];
	uint64_t edges = ((A_FILE | H_FILE) & ~direction1) | ((RANK_1 | RANK_8) & ~direction2);
	uint64_t *subsets = enumerateSubsets((direction1 ^ direction2) & ~edges, indexBits);
	uint64_t index;
	uint64_t attacks;
	uint64_t blockers;

	do {
		blockers = (blockers | edges) & (direction1 ^ direction2);
		index = (blockers*magic) >> (64-indexBits);
		attacks = slidingAttacks(blockers, square, direction1);
		attacks |= slidingAttacks(blockers, square, direction2);
		if (table[index] == 0) {
			table[index] = attacks;
		} else if (table[index] != attacks) {
			return nullptr;
		}
	} while ((blockers = *(subsets++)));
	return table;
}


uint64_t generateBishopMagic(int square) {
	uint64_t *table;
	uint64_t diagonal = getDiagonal(square);
	uint64_t antidiagonal = getAntidiagonal(square);
	uint64_t magic;

	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<uint64_t> dis;

	do {
		magic = dis(gen) & dis(gen) & dis(gen);
		table = makeLookupTable(magic, diagonal, antidiagonal, 12, square);
	} while (table == nullptr);
	return magic;
}


uint64_t generateRookMagic(int square) {
	uint64_t *table;
	uint64_t file = getFile(square);
	uint64_t rank = getRank(square);
	uint64_t magic;
	
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<uint64_t> dis;

	do {
		magic = dis(gen) & dis(gen) & dis(gen);
		table = makeLookupTable(magic, file, rank, 12, square);
	} while (table == nullptr);
	return magic;
}


void generateAllMagics() {
	std::cout << "BISHOP MAGICS:" << "\n";
	for (int square = 0; square < 64; square++) {
		std::cout << "0x" << std::hex << generateBishopMagic(square) << "ull, \n";
	}
	std::cout << "ROOK MAGICS:" << "\n";
	for (int square = 0; square < 64; square++) {
		std::cout << "0x" << std::hex << generateRookMagic(square) << "ull, \n";
	}
}


void testMagics() {
	uint64_t bishopMask;
	uint64_t rookMask;
	uint64_t *subsets;
	uint64_t *table;
	int bishopMistakes = 0;
	int rookMistakes = 0;
	int indexBits = 12;
	uint64_t edges;
	uint64_t bishopMagic;
	uint64_t rookMagic;

	for (int square = 0; square < 64; square++) {
		bishopMask = getDiagonal(square) ^ getAntidiagonal(square);
		subsets = enumerateSubsets(bishopMask & ~EDGES, indexBits);
		bishopMagic = generateBishopMagic(square);
		table = makeLookupTable(bishopMagic, getDiagonal(square), getAntidiagonal(square), indexBits, square);
		while (uint64_t blockers = *(subsets++)) {
			if ((slidingAttacks(blockers, square, getDiagonal(square)) | slidingAttacks(blockers, square, getAntidiagonal(square))) != table[(((blockers | EDGES) & bishopMask) * bishopMagic) >> (64-indexBits)]) {
				bishopMistakes++;
			}
		}
		edges = ((A_FILE | H_FILE) & ~getFile(square)) | ((RANK_1 | RANK_8) & ~getRank(square));
		rookMask = getFile(square) ^ getRank(square);
		subsets = enumerateSubsets(rookMask & ~edges, indexBits);
		rookMagic = generateRookMagic(square);
		table = makeLookupTable(rookMagic, getFile(square), getRank(square), indexBits, square);
		while (uint64_t blockers = *(subsets++)) {
			blockers = (blockers | edges) & rookMask;
			if ((slidingAttacks(blockers, square, getFile(square)) | slidingAttacks(blockers, square, getRank(square))) != table[(blockers * rookMagic) >> (64-indexBits)]) {
				rookMistakes++;
			}
		}
	}
	std::cout << "Number of Bishop mistakes: " << bishopMistakes << "\n";
	std::cout << "Number of Rook mistakes:   " << rookMistakes << "\n";
}
