#include <stdint.h>
#include <iostream>

#include "board.h"
#include "bitboard.h"


void clearBoard(Board *board) {
	for (int i = 0; i < 12; i++) {
		board->pieces[i] = 0;
	}
	board->turn = true;
	board->halfMoveCounter = 0;
	board->fullMoveCounter = 0;
	board->epSquare = -1;
	board->castling = 0;
}

void boardFromFEN(Board *board, const char *fen) {
	clearBoard(board);
	int piece = 0;
	int index = 0;
	int square = 63;
	char c = fen[0];
	// Getting the piece placement
	while (c != ' ') {
		if (isdigit(c)) {
			square -= c-'0';
		} else if (c != '/') {
			switch (c) {
				case 'P':
					piece = 0;
					break;
				case 'N':
					piece = 1;
					break;
				case 'B':
					piece = 2;
					break;
				case 'R':
					piece = 3;
					break;
				case 'Q':
					piece = 4;
					break;
				case 'K':
					piece = 5;
					break;
				case 'p':
					piece = 6;
					break;
				case 'n':
					piece = 7;
					break;
				case 'b':
					piece = 8;
					break;
				case 'r':
					piece = 9;
					break;
				case 'q':
					piece = 10;
					break;
				case 'k':
					piece = 11;
					break;
			}
			board->pieces[piece] |= 1ull << square;
			square--;
		}
		index++;
		c = fen[index];
	}
	// Side to move
	index++;
	if (fen[index] == 'w') {
		board->turn = true;
	} else {
		board->turn = false;
	}
	index += 2;
	// Castling rights
	c = fen[index];
	while (c != ' ') {
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
		index++;
		c = fen[index];
	}
	index++;
	// En passant square
	if (fen[index] == '-') {
		index++;
	} else {
		// TODO: handle en passant square
		// Write square to index method to do this
	}
	// Half move counter
	int counter = 0;
	index++;
	c = fen[index];
	while (c != ' ') {
		counter *= 10;
		counter += c-'0';
		index++;
		c = fen[index];
	}
	board->halfMoveCounter = counter;
	// Full move counter
	counter = 0;
	index++;
	c = fen[index];
	while (isdigit(c)) {
		std::cout << c;
		counter *= 10;
		counter += c-'0';
		index++;
		c = fen[index];
	}
	board->fullMoveCounter = counter;
}

void printBoard(Board *board) {
	// TODO
	for (uint64_t bb : board->pieces) {
		printBitboard(bb);
		std::cout << "----------" << "\n";
	}
	std::cout << board->turn << ", ";
	std::cout << board->halfMoveCounter << ", ";
	std::cout << board->fullMoveCounter << ", ";
	std::cout << board->epSquare << ", ";
	std::cout << board->castling << "\n";
}
