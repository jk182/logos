#include "board.h"
#include "move.h"

#include <cassert>
#include <iostream>
#include <stdint.h>

void testCastlingRights() {
	std::cout << "Testing castling right hashes...\n";
	Board board;
	uint64_t* hashes = new uint64_t[16];
	const char* castling[16] = {"-", "K", "Q", "k", "q", "KQ", "Kk", "Kq", "Qk", "Qq", "kq", "KQk", "KQq", "Kkq", "Qkq", "KQkq"};
	for (int i = 0; i < 16; i++) {
		boardFromFEN(&board, strcat(strcat(strdup("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w "), castling[i]), strdup(" - 0 1")));
		hashes[i] = board.hash;
	}

	for (int i = 0; i < 15; i++) {
		for (int j = i+1; j < 16; j++) {
			assert(hashes[i] != hashes[j]);
		}
	}
}

void testZobrist() {
	Board startPos;
	Board e4;
	Board e4nEP;
	Board e4W;
	boardFromFEN(&startPos, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	boardFromFEN(&e4, "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
	boardFromFEN(&e4nEP, "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1");
	boardFromFEN(&e4W, "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e3 0 1");
	uint64_t startHash = startPos.hash;
	assert(startHash != e4.hash);

	assert(e4.hash != e4nEP.hash);
	assert(e4.hash != e4W.hash);

	// Making and unmaking moves
	uint16_t e2e4 = encodeUCIMove(startPos, strdup("e2e4"));
	Undo undo = generateUndo(&startPos, e2e4);
	makeMove(&startPos, e2e4);
	assert(startPos.hash == e4.hash);
	unmakeMove(&startPos, e2e4, &undo);
	assert(startPos.hash == startHash);

	testCastlingRights();

	// Captures
	Board beforeCapture;
	Board afterCapture;
	boardFromFEN(&beforeCapture, "rnbqkbnr/pp1ppppp/8/2p5/3PP3/8/PPP2PPP/RNBQKBNR b KQkq - 0 2");
	boardFromFEN(&afterCapture, "rnbqkbnr/pp1ppppp/8/8/3pP3/8/PPP2PPP/RNBQKBNR w KQkq - 0 3");
	assert(beforeCapture.hash != afterCapture.hash);
	uint16_t cap = encodeUCIMove(beforeCapture, strdup("c5d4"));
	Undo undoCap = generateUndo(&beforeCapture, cap);
	makeMove(&beforeCapture, cap);
	assert(beforeCapture.hash == afterCapture.hash);
	unmakeMove(&beforeCapture, cap, &undoCap);
	assert(beforeCapture.hash != afterCapture.hash);

	std::cout << "Zobrist hash tests completed!\n";
}
