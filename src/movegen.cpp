#include <stdint.h>
#include <iostream>

#include "move.h"
#include "movegen.h"
#include "board.h"
#include "types.h"
#include "bitboard.h"


uint16_t* generateKingMoves(Board *board, uint16_t *moves) {
	int piece = board->turn ? W_KING : B_KING;
	int square = getLSB(&board->pieces[piece]);
	return moves;
}
