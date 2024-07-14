#pragma once

// Move format: 
// 	bits 0-5:	start square
// 	bits 6-11:	end square
// 	bits 12-14:	promotion piece

enum {
	NULL_MOVE = 0
};


void printMove(uint16_t move);
