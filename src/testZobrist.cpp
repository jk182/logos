#include "board.h"
#include "move.h"

#include <cassert>
#include <iostream>
#include <stdint.h>
#include <string.h>

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
	// Undo undo = generateUndo(&startPos, e2e4);
	Undo undo;
	makeMove(&startPos, e2e4, &undo);
	assert(startPos.hash == e4.hash);
	unmakeMove(&startPos, e2e4, &undo);
	assert(startPos.hash == startHash);

	testCastlingRights();

	// Captures
	Board beforeCapture;
	Board afterCapture;
	boardFromFEN(&beforeCapture, "rnbqkbnr/pp1ppppp/8/2p5/3PP3/8/PPP2PPP/RNBQKBNR b KQkq - 0 2");
	boardFromFEN(&afterCapture, "rnbqkbnr/pp1ppppp/8/8/3pP3/8/PPP2PPP/RNBQKBNR w KQkq - 0 3");
	uint64_t bCapHash = beforeCapture.hash;
	assert(beforeCapture.hash != afterCapture.hash);
	uint16_t cap = encodeUCIMove(beforeCapture, strdup("c5d4"));
	// Undo undoCap = generateUndo(&beforeCapture, cap);
	Undo undoCap;
	makeMove(&beforeCapture, cap, &undo);
	assert(beforeCapture.hash == afterCapture.hash);
	unmakeMove(&beforeCapture, cap, &undoCap);
	assert(beforeCapture.hash != afterCapture.hash);
	assert(beforeCapture.hash == bCapHash);

	// Promotion
	Board bProm;
	Board aProm;
	boardFromFEN(&bProm, "6r1/5P2/8/2k5/8/3K4/8/8 w - - 0 1");
	boardFromFEN(&aProm, "6Q1/8/8/2k5/8/3K4/8/8 b - - 0 1");
	uint64_t bPromHash = bProm.hash;
	assert(bProm.hash != aProm.hash);
	uint16_t prom = encodeUCIMove(bProm, strdup("f7g8q"));
	// Undo undoProm = generateUndo(&bProm, prom);
	Undo undoProm;
	makeMove(&bProm, prom, &undo);
	assert(bProm.hash == aProm.hash);
	unmakeMove(&bProm, prom, &undoProm);
	assert(bProm.hash != aProm.hash);
	assert(bProm.hash == bPromHash);

	std::cout << "Zobrist hash tests completed!\n";
}
