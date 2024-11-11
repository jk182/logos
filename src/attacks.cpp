#include "attacks.h"
#include "bitboard.h"
#include "board.h"
#include "magic.h"
#include "types.h"

#include <stdint.h>

Magic BISHOP_TABLES[64];
Magic ROOK_TABLES[64];

uint64_t slidingAttacks(Board *board, bool turn, int square, uint64_t mask) {
	uint64_t pieceBB = 1ull << square;
	uint64_t attacks;
	uint64_t negative;

	uint64_t occupied = occupiedSquares(board);
	uint64_t occMask = occupied & mask;

	attacks = occMask^(occMask-(pieceBB<<1));

	negative = occMask^reverse((reverse(occMask)-(reverse(pieceBB)<<1))&reverse(mask));
	negative &= (1ull<<square)-1;
	attacks |= negative;
	attacks &= (~pieceBB);
	attacks &= mask;

	return attacks;
}


uint64_t slidingAttacks(uint64_t occupied, int square, uint64_t mask) {
	uint64_t pieceBB = 1ull << square;
	uint64_t attacks;
	uint64_t negative;
	uint64_t occMask = occupied & mask;

	attacks = occMask^(occMask-(pieceBB<<1));

	negative = occMask^reverse((reverse(occMask)-(reverse(pieceBB)<<1))&reverse(mask));
	negative &= (1ull<<square)-1;
	attacks |= negative;
	attacks &= (~pieceBB);
	attacks &= mask;

	return attacks;
}


uint64_t pawnAttacks(Board *board, bool turn) {
	uint64_t pawns;
	uint64_t attacks = 0ull;
	if (turn) {
		pawns = board->pieces[W_PAWN];
		attacks |= (pawns & (~A_FILE)) << 9;
		attacks |= (pawns & (~H_FILE)) << 7;
	} else {
		pawns = board->pieces[B_PAWN];
		attacks |= (pawns & (~A_FILE)) >> 7;
		attacks |= (pawns & (~H_FILE)) >> 9;
	}
	return attacks;
}


uint64_t knightAttacks(Board *board, bool turn) {
	uint64_t attacks = 0ull;
	uint64_t knights = board->pieces[turn ? W_KNIGHT : B_KNIGHT];

	while (knights) {
		attacks |= KNIGHT_ATTACKS[popLSB(&knights)];
	}
	return attacks;
}


uint64_t bishopAttacks(uint64_t occupied, int square) {
	Magic table = BISHOP_TABLES[square];
	return table.table[(((occupied | EDGES) & table.mask) * table.magic) >> (64-table.index)];
}


uint64_t bishopAttacks(Board *board, bool turn) {
	int piece = turn ? W_BISHOP : B_BISHOP;
	uint64_t bishopBB = board->pieces[piece];
	uint64_t attacks = 0ull;

	uint64_t occupied = occupiedSquares(board);
	while (bishopBB) {
		attacks |= bishopAttacks(occupied, popLSB(&bishopBB));
	}
	return attacks;
}


uint64_t rookAttacks(uint64_t occupied, int square) {
	Magic table = ROOK_TABLES[square];
	uint64_t edges = ((A_FILE | H_FILE) & ~getFile(square)) | ((RANK_1 | RANK_8) & ~getRank(square));
	return table.table[(((occupied | edges) & table.mask) * table.magic) >> (64-table.index)];
}


uint64_t rookAttacks(Board *board, bool turn) {
	int piece = turn ? W_ROOK : B_ROOK;
	uint64_t rookBB = board->pieces[piece];
	uint64_t attacks = 0ull;
	uint64_t occupied = occupiedSquares(board);

	while (rookBB) {
		attacks |= rookAttacks(occupied, popLSB(&rookBB));
	}
	return attacks;
}


uint64_t queenAttacks(Board *board, bool turn) {
	int piece = turn ? W_QUEEN : B_QUEEN;
	uint64_t queenBB = board->pieces[piece];
	uint64_t attacks = 0ull;
	int square;
	uint64_t occupied = occupiedSquares(board);

	while (queenBB) {
		square = popLSB(&queenBB);
		attacks |= bishopAttacks(occupied, square);
		attacks |= rookAttacks(occupied, square);
	}

	return attacks;
}


uint64_t kingAttacks(Board *board, bool turn) {
	int piece = turn ? W_KING : B_KING;
	uint64_t king = board->pieces[piece];

	return KING_ATTACKS[popLSB(&king)];
}


uint64_t getAttacks(Board board, bool turn) {
	uint64_t attacks;

	attacks = pawnAttacks(&board, turn);
	attacks |= knightAttacks(&board, turn);
	attacks |= bishopAttacks(&board, turn);
	attacks |= rookAttacks(&board, turn);
	attacks |= queenAttacks(&board, turn);
	attacks |= kingAttacks(&board, turn);

	return attacks;
}


void initSlidingAttacks() {
	uint64_t file;
	uint64_t rank;
	uint64_t diag;
	uint64_t antidiag;
	for (int square = 0; square < 64; square++) {
		BISHOP_TABLES[square].magic = BISHOP_MAGICS[square];
		diag = getDiagonal(square);
		antidiag = getAntidiagonal(square);
		BISHOP_TABLES[square].index = 12;
		BISHOP_TABLES[square].mask = diag ^ antidiag;
		BISHOP_TABLES[square].table = makeLookupTable(BISHOP_MAGICS[square], diag, antidiag, BISHOP_TABLES[square].index, square);

		ROOK_TABLES[square].magic = ROOK_MAGICS[square];
		file = getFile(square);
		rank = getRank(square);
		ROOK_TABLES[square].index = 12;
		ROOK_TABLES[square].mask = file ^ rank;
		ROOK_TABLES[square].table = makeLookupTable(ROOK_MAGICS[square], file, rank, ROOK_TABLES[square].index, square);
	}
}


bool isSquareAttacked(Board board, bool turn, int square) {
	return (getAttacks(board, turn) & (1ull<<square)) != 0;
}
