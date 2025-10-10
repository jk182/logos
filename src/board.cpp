#include "attacks.h"
#include "board.h"
#include "bitboard.h"
#include "move.h"
#include "movegen.h"
#include "types.h"
#include "zobrist.h"

#include <iostream>
#include <stdint.h>
#include <string.h>


void clearBoard(Board *board) {
	for (int i = 0; i < 12; i++) {
		board->pieces[i] = 0;
	}
	board->turn = true;
	board->halfMoveCounter = 0;
	board->fullMoveCounter = 0;
	board->numMoves = 0;
	board->epSquare = -1;
	board->castling = 0;
	board->hash = 0;
	for (int i = 0; i < HISTORY_LENGTH; i++) {
		board->history[i] = 0;
	}
	board->attacks = 0;
	// board->threats = 0;
}


int stringToSquare(char *sq) {
	return getSquare(sq[0], sq[1]-'1');
}


void boardFromFEN(Board *board, const char *fen) {
	clearBoard(board);
	int piece;
	int square = 63;
	char c;

	char *str = strdup(fen);
	char *strPos = NULL;
	char *token = strtok_r(str, " ", &strPos);

	// Getting the piece placement
	while ((c = *token++)) {
		if (isdigit(c)) {
			square -= c-'0';
		} else if (c != '/') {
			switch (c) {
				case 'P':
					piece = W_PAWN;
					break;
				case 'N':
					piece = W_KNIGHT;
					break;
				case 'B':
					piece = W_BISHOP;
					break;
				case 'R':
					piece = W_ROOK;
					break;
				case 'Q':
					piece = W_QUEEN;
					break;
				case 'K':
					piece = W_KING;
					break;
				case 'p':
					piece = B_PAWN;
					break;
				case 'n':
					piece = B_KNIGHT;
					break;
				case 'b':
					piece = B_BISHOP;
					break;
				case 'r':
					piece = B_ROOK;
					break;
				case 'q':
					piece = B_QUEEN;
					break;
				case 'k':
					piece = B_KING;
					break;
			}
			board->pieces[piece] |= 1ull << square;
			board->hash ^= zobristKeys[piece][square];
			square--;
		}
	}
	// Side to move
	token = strtok_r(NULL, " ", &strPos);
	board->turn = token[0] == 'w' ? true : false;
	if (! board->turn) {
		board->hash ^= zobristTurnKey;
	}

	// Castling rights
	token = strtok_r(NULL, " ", &strPos);
	while ((c = *token++)) {
		switch(c) {
			case 'K':
				board->castling |= 1ull;
				break;
			case 'Q':
				board->castling |= 1ull << 1;
				break;
			case 'k':
				board->castling |= 1ull << 2;
				break;
			case 'q':
				board->castling |= 1ull << 3;
				break;
		}
	}
	board->hash ^= zobristCastlingKeys[board->castling];

	// En passant square
	token = strtok_r(NULL, " ", &strPos);
	if (token[0] == '-') {
		board->epSquare = -1;
	} else {
		board->epSquare = stringToSquare(token);
		board->hash ^= zobristEpKeys[board->epSquare % 8];
	}

	// Half move counter
	token = strtok_r(NULL, " ", &strPos);
	int counter = 0;
	while ((c = *token++)) {
		counter *= 10;
		counter += c-'0';
	}
	board->halfMoveCounter = counter;

	// Full move counter
	token = strtok_r(NULL, " ", &strPos);
	counter = 0;
	while (isdigit(c)) {
		counter *= 10;
		counter += c-'0';
	}
	board->fullMoveCounter = counter;
	board->attacks = getAttacks(board, board->turn);
	// board->threats = getAttacks(board, !board->turn);
}


void printBoard(Board *board) {
	char symbols[] = {'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k'};
	char piece;
	for (int square = 63; square >= 0; square--) {
		piece = '.';
		for (int i = 0; i < 12; i++) {
			if ((board->pieces[i] & (1ull<<square)) != 0) {
				piece = symbols[i];
			} 
		}
		std::cout << piece << " ";
		if (square%8 == 0) {
			std::cout << "\n";
		}
	}
	char* ep = getSquareName(board->epSquare);
	std::cout << "Side to move: " << board->turn << ", ";
	std::cout << "Half move counter: " << board->halfMoveCounter << ", ";
	std::cout << "Move number: " << board->fullMoveCounter << ", ";
	std::cout << "En-passant square: " << ep << ", ";
	std::cout << "Castling rights: " << board->castling << "\n";
}


uint64_t whitePieces(Board *board) {
	uint64_t white = 0ull;
	for (int i = 0; i < PIECES/2; i++) {
		white |= board->pieces[i];
	}
	return white;
}


uint64_t blackPieces(Board *board) {
	uint64_t black = 0ull;
	for (int i = 6; i < PIECES; i++) {
		black |= board->pieces[i];
	}
	return black;
}


uint64_t occupiedSquares(Board *board) {
	uint64_t occupied = 0ull;
	for (int p = 0; p < PIECES; p++) {
		occupied |= board->pieces[p];
	}
	return occupied;
}


bool isLegalPosition(Board *board) {
	/*
	uint64_t bb = board->turn ? board->pieces[B_KING] : board->pieces[W_KING];
	int square = popLSB(&bb);
	return !isSquareAttacked(board, board->turn, square);
	*/
	uint64_t bb = board->turn ? board->pieces[B_KING] : board->pieces[W_KING];
	return (board->attacks & bb) == 0;
}


int getPieceAtSquare(Board *board, int square) {
	for (int p = 0; p < 12; p++) {
		if (board->pieces[p] & (1ull << square)) {
			return p;
		}
	}
	return -1;
}


Undo generateUndo(Board *board, uint16_t move) {
	// Obsolete
	Undo undo;
	undo.castling = board->castling;
	undo.halfMoveCounter = board->halfMoveCounter;
	undo.capturedPiece = getPieceAtSquare(board, getEndSquare(move));
	return undo;
}


bool isInsufficientMaterial(Board *board) {
	for (int piece : {W_PAWN, B_PAWN, W_ROOK, B_ROOK, W_QUEEN, B_QUEEN}) {
		if (board->pieces[piece] != 0) {
			return false;
		}
	}
	uint64_t minorPieces = board->pieces[W_KNIGHT] | board->pieces[W_BISHOP] | board->pieces[B_KNIGHT] | board->pieces[B_BISHOP];
	popLSB(&minorPieces);
	return (minorPieces == 0);
}


bool isStalemate(Board *board) {
	if (isCheck(board)) {
		return false;
	}

	uint16_t moves[MAX_MOVES];
	uint16_t *end = generateAllMoves(board, moves);
	uint16_t move;
	Undo undo;

	for (int i = 0; i < end-moves; i++) {
		move = *(moves+i);
		// undo = generateUndo(board, move);
		makeMove(board, move, &undo);
		if (isLegalPosition(board)) {
			unmakeMove(board, move, &undo);
			return false;
		}
		unmakeMove(board, move, &undo);
	}
	return true;
}


bool isThreefold(Board *board) {
	if (board->halfMoveCounter < 5) {
		return false;
	}
	int counter = 1;
	for (int i = 1; i <= board->halfMoveCounter; i++) {
		if (board->hash == board->history[board->numMoves - i]) counter++;
		if (counter >= 3) return true;	
	}
	return false;
}


bool isDraw(Board *board) {
	return (board->halfMoveCounter >= 50) || isThreefold(board) || isInsufficientMaterial(board) || isStalemate(board);
	// return (board->halfMoveCounter >= 50) || isThreefold(board) || isInsufficientMaterial(board) || isStalemate(board);
	// TODO: using the halfMoveCounter causes problems when decoding moves in UCI
}


bool isCheck(Board *board) {
	uint64_t bb = board->turn ? board->pieces[W_KING] : board->pieces[B_KING];
	return isSquareAttacked(board, !board->turn, getLSB(&bb));
	// return (bb & board->threats) != 0;
	/*
	if (board->turn) {
		return (board->attacks[1] & board->pieces[W_KING]) != 0;
	} else {
		return (board->attacks[0] & board->pieces[B_KING]) != 0;
	}
	*/
}


bool isCheckmate(Board *board) {
	if (!isCheck(board)) {
		return false;
	}

	uint16_t moves[MAX_MOVES];
	uint16_t *end = generateAllMoves(board, moves);
	uint16_t move;
	Undo undo;

	for (int i = 0; i < end-moves; i++) {
		move = *(moves+i);
		// undo = generateUndo(board, move);
		makeMove(board, move, &undo);
		if (isLegalPosition(board)) {
			unmakeMove(board, move, &undo);
			return false;
		}
		unmakeMove(board, move, &undo);
	}
	return true;
}


bool isGameOver(Board *board) {
	// return isDraw(board) || isCheckmate(board);
	if (isThreefold(board) || isInsufficientMaterial(board)) {
		return true;
	}
	uint16_t moves[MAX_MOVES];
	uint16_t *end = generateAllMoves(board, moves);
	uint16_t move;
	Undo undo;

	for (int i = 0; i < end-moves; i++) {
		move = *(moves+i);
		makeMove(board, move, &undo);
		if (isLegalPosition(board)) {
			unmakeMove(board, move, &undo);
			return false;
		}
		unmakeMove(board, move, &undo);
	}
	return true;
}


bool isLegalMove(Board board, uint16_t move) {
	uint16_t *moves = new uint16_t[MAX_MOVES];
	uint16_t *end = generateAllMoves(&board, moves);
	Undo undo;
	for (int i = 0; i < end-moves; i++) {
		if (move == *(moves+i)) {
			// undo = generateUndo(&board, move);
			makeMove(&board, move, &undo);
			if (isLegalPosition(&board)) {
				unmakeMove(&board, move, &undo);
				return true;
			} else {
				unmakeMove(&board, move, &undo);
				return false;
			}
		}
	}
	return false;
}


bool isSamePosition(Board *b1, Board *b2) {
	for (int i = 0; i < PIECES; i++) {
		if (b1->pieces[i] != b2->pieces[i]) {
			return false;
		}
	}
	return b1->castling == b2->castling && b1->turn == b2->turn;
}
