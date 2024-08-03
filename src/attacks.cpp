#include "attacks.h"
#include "bitboard.h"
#include "board.h"
#include "types.h"

#include <stdint.h>


uint64_t slidingAttacks(Board board, bool turn, int square, uint64_t mask) {
	uint64_t pieceBB = 1ull << square;
	uint64_t attacks = 0ull;
	uint64_t negative = 0ull;

	uint64_t occupied = occupiedSquares(&board);
	uint64_t occMask = occupied & mask;

	attacks = occMask^(occMask-(pieceBB<<1));
	attacks &= mask;

	negative = occMask^reverse(reverse(occMask)-(reverse(pieceBB)<<1));
	negative &= mask;
	negative &= (1ull<<square)-1;
	attacks |= negative;
	attacks &= (~pieceBB);

	return attacks;
}


uint64_t pawnAttacks(Board board, bool turn) {
	uint64_t attacks = 0ull;
	int piece = turn ? W_PAWN : B_PAWN;
	uint64_t pawns = board.pieces[piece];

	attacks |= (pawns & (~A_FILE)) << 9;
	attacks |= (pawns & (~H_FILE)) << 7;

	return attacks;
}


uint64_t knightAttacks(Board board, bool turn) {
	uint64_t attacks = 0ull;
	int piece = turn ? W_KNIGHT : B_KNIGHT;
	uint64_t knights = board.pieces[piece];
	int square;
	uint64_t currKnight;

	while (knights) {
		square = popLSB(&knights);
		attacks ^= KNIGHT_ATTACKS[square];
	}
	return attacks;
}


uint64_t bishopAttacks(Board board, bool turn) {
	int piece = turn ? W_BISHOP : B_BISHOP;
	uint64_t bishopBB = board.pieces[piece];
	uint64_t attacks = 0ull;
	uint64_t diagonal;
	uint64_t antidiagonal;
	int square;

	while (bishopBB) {
		square = popLSB(&bishopBB);
		diagonal = getDiagonal(square);
		antidiagonal = getAntidiagonal(square);

		attacks |= slidingAttacks(board, turn, square, diagonal);
		attacks |= slidingAttacks(board, turn, square, antidiagonal);
	}

	return attacks;
}


uint64_t rookAttacks(Board board, bool turn) {
	int piece = turn ? W_ROOK : B_ROOK;
	uint64_t rookBB = board.pieces[piece];
	uint64_t attacks = 0ull;
	uint64_t file;
	uint64_t rank;
	int square;

	while (rookBB) {
		square = popLSB(&rookBB);
		file = getFile(square);
		rank = getRank(square);

		attacks |= slidingAttacks(board, turn, square, file);
		attacks |= slidingAttacks(board, turn, square, rank);
	}

	return attacks;
}


uint64_t queenAttacks(Board board, bool turn) {
	int piece = turn ? W_QUEEN : B_QUEEN;
	uint64_t queenBB = board.pieces[piece];
	uint64_t attacks = 0ull;
	uint64_t file;
	uint64_t rank;
	uint64_t diagonal;
	uint64_t antidiagonal;
	int square;

	while (queenBB) {
		square = popLSB(&queenBB);
		file = getFile(square);
		rank = getRank(square);
		diagonal = getDiagonal(square);
		antidiagonal = getAntidiagonal(square);

		attacks |= slidingAttacks(board, turn, square, file);
		attacks |= slidingAttacks(board, turn, square, rank);
		attacks |= slidingAttacks(board, turn, square, diagonal);
		attacks |= slidingAttacks(board, turn, square, antidiagonal);
	}

	return attacks;
}



uint64_t kingAttacks(Board board, bool turn) {
	uint64_t attacks = 0ull;
	int piece = turn ? W_KING : B_KING;
	uint64_t king = board.pieces[piece];

	return KING_ATTACKS[popLSB(&king)];
}


uint64_t getAttacks(Board board, bool turn) {
	uint64_t attacks = 0ull;

	attacks |= pawnAttacks(board, turn);
	attacks |= knightAttacks(board, turn);
	attacks |= bishopAttacks(board, turn);
	attacks |= rookAttacks(board, turn);
	attacks |= queenAttacks(board, turn);
	attacks |= kingAttacks(board, turn);

	return attacks;
}


bool isSquareAttacked(Board board, bool turn, int square) {
	return (getAttacks(board, turn) & (1ull<<square)) != 0;
}
