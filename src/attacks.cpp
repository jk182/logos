#include "bitboard.h"
#include "board.h"
#include "types.h"


bool isSquareAttacked(int square, Board board) {
	// return (getAttacks(board) & (1ull<<square)) != 0;
	return false;
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
		currKnight = 1ull << square;

		attacks ^= (currKnight & (~(RANK_1|RANK_2|H_FILE))) >> 17;
		attacks ^= (currKnight & (~(RANK_1|RANK_2|A_FILE))) >> 15;

		attacks ^= (currKnight & (~(RANK_1|G_FILE|H_FILE))) >> 10;
		attacks ^= (currKnight & (~(RANK_1|A_FILE|B_FILE))) >> 6;

		attacks ^= (currKnight & (~(RANK_8|G_FILE|H_FILE))) << 6;
		attacks ^= (currKnight & (~(RANK_8|A_FILE|B_FILE))) << 10;

		attacks ^= (currKnight & (~(RANK_7|RANK_8|H_FILE))) << 15;
		attacks ^= (currKnight & (~(RANK_7|RANK_8|A_FILE))) << 17;
	}
	return attacks;
}


uint64_t kingAttacks(Board board, bool turn) {
	uint64_t attacks = 0ull;
	int piece = turn ? W_KING : B_KING;
	uint64_t king = board.pieces[piece];

	attacks ^= (king & (~A_FILE)) << 1;
	attacks ^= (king & (~H_FILE)) >> 1;
	attacks ^= (king & (~RANK_1)) >> 8;
	attacks ^= (king & (~RANK_8)) << 8;

	attacks ^= (king & (~(A_FILE|RANK_1))) >> 7;
	attacks ^= (king & (~(A_FILE|RANK_8))) << 9;
	attacks ^= (king & (~(H_FILE|RANK_1))) >> 9;
	attacks ^= (king & (~(H_FILE|RANK_8))) << 7;

	return attacks;
}
