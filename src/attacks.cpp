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
	int piece = turn ? W_KNIGHT : B_KNIGHT;
	uint64_t knights = board->pieces[piece];
	int square;

	while (knights) {
		square = popLSB(&knights);
		attacks |= KNIGHT_ATTACKS[square];
	}
	return attacks;
}


uint64_t bishopAttacks(Board *board, bool turn) {
	int piece = turn ? W_BISHOP : B_BISHOP;
	uint64_t bishopBB = board->pieces[piece];
	uint64_t attacks = 0ull;
	int square;

	/*
	uint64_t diagonal;
	uint64_t antidiagonal;
	while (bishopBB) {
		square = popLSB(&bishopBB);
		diagonal = getDiagonal(square);
		antidiagonal = getAntidiagonal(square);

		attacks |= slidingAttacks(board, turn, square, diagonal);
		attacks |= slidingAttacks(board, turn, square, antidiagonal);
	}
	*/
	Magic table;
	uint64_t occupied = occupiedSquares(board);
	while (bishopBB) {
		square = popLSB(&bishopBB);
		table = BISHOP_TABLES[square];
		attacks |= table.table[(((occupied | EDGES) & table.mask) * table.magic) >> (64-table.index)];
	}
	return attacks;
}


uint64_t rookAttacks(Board *board, bool turn) {
	int piece = turn ? W_ROOK : B_ROOK;
	uint64_t rookBB = board->pieces[piece];
	uint64_t attacks = 0ull;
	int square;
	/*
	uint64_t file;
	uint64_t rank;
	while (rookBB) {
		square = popLSB(&rookBB);
		file = getFile(square);
		rank = getRank(square);

		attacks |= slidingAttacks(board, turn, square, file);
		attacks |= slidingAttacks(board, turn, square, rank);
	}
	*/
	Magic table;
	uint64_t edges;
	uint64_t occupied = occupiedSquares(board);
	while (rookBB) {
		square = popLSB(&rookBB);
		edges = ((A_FILE | H_FILE) & ~getFile(square)) | ((RANK_1 | RANK_8) & ~getRank(square));
		table = ROOK_TABLES[square];
		attacks |= table.table[(((occupied | edges) & table.mask) * table.magic) >> (64-table.index)];
	}
	return attacks;
}


uint64_t queenAttacks(Board *board, bool turn) {
	int piece = turn ? W_QUEEN : B_QUEEN;
	uint64_t queenBB = board->pieces[piece];
	uint64_t attacks = 0ull;
	uint64_t file;
	uint64_t rank;
	uint64_t diagonal;
	uint64_t antidiagonal;
	int square;
	uint64_t occupied = occupiedSquares(board);
	Magic table;
	uint64_t edges;

	while (queenBB) {
		square = popLSB(&queenBB);
		file = getFile(square);
		rank = getRank(square);
		edges = ((A_FILE | H_FILE) & ~getFile(square)) | ((RANK_1 | RANK_8) & ~getRank(square));
		table = ROOK_TABLES[square];
		attacks |= table.table[(((occupied | edges) & table.mask) * table.magic) >> (64-table.index)];

		diagonal = getDiagonal(square);
		antidiagonal = getAntidiagonal(square);
		table = BISHOP_TABLES[square];
		attacks |= table.table[(((occupied | EDGES) & table.mask) * table.magic) >> (64-table.index)];

		/*
		attacks |= slidingAttacks(board, turn, square, file);
		attacks |= slidingAttacks(board, turn, square, rank);
		attacks |= slidingAttacks(board, turn, square, diagonal);
		attacks |= slidingAttacks(board, turn, square, antidiagonal);
		*/
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
		BISHOP_TABLES[square].index = 15;
		BISHOP_TABLES[square].mask = diag ^ antidiag;
		BISHOP_TABLES[square].table = makeLookupTable(BISHOP_MAGICS[square], diag, antidiag, BISHOP_TABLES[square].index, square);

		ROOK_TABLES[square].magic = ROOK_MAGICS[square];
		file = getFile(square);
		rank = getRank(square);
		ROOK_TABLES[square].index = 15;
		ROOK_TABLES[square].mask = file ^ rank;
		ROOK_TABLES[square].table = makeLookupTable(ROOK_MAGICS[square], file, rank, ROOK_TABLES[square].index, square);
	}
}


bool isSquareAttacked(Board board, bool turn, int square) {
	return (getAttacks(board, turn) & (1ull<<square)) != 0;
}
