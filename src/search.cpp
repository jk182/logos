#include <limits.h>
#include <algorithm>

#include "board.h"
#include "evaluation.h"
#include "move.h"
#include "movegen.h"


int alphaBeta(Board *board, int depth, int alpha, int beta) {

	if (depth <= 0 || isGameOver(board)) {
		return evaluate(board); // TODO : quiescence search
	}

	int value;
	int bestScore = INT_MIN;
	uint16_t moveArr[128];
	uint16_t *moves = moveArr;
	uint16_t *end = generateAllMoves(*board, moves);
	uint16_t move;
	Undo undo;
	undo.halfMoveCounter = board->halfMoveCounter;
	undo.castling = board->castling;
	
	// Move loop
	for (int i = 0; i < end-moves; i++) {
		move = *(moves+i);
		undo.capturedPiece = getPieceAtSquare(board, getEndSquare(move));
		makeMove(board, move);
		value = alphaBeta(board, depth-1, -beta, -alpha);
		unmakeMove(board, move, &undo);
		alpha = std::max(alpha, value);
		if (value > bestScore) {
            bestScore = value;
            if (value > alpha) {
                alpha = value;
			}
        }
        if( value >= beta )
            break;
   }
   return bestScore;
}


int iterativeDeepening(Board *board, int depth) {
	if (depth == 0 || isGameOver(board)) {
		return evaluate(board);
	}
	int score;
	for (int d = 0; d <= depth; d++) {
		score = alphaBeta(board, d, INT_MIN, INT_MAX);
	}
	return score;
}
