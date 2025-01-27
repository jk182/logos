#pragma once

#include "types.h"

#include <stdint.h>

extern uint64_t zobristKeys[PIECES][64];
extern uint64_t zobristEpKeys[8];
extern uint64_t zobristCastlingKeys[16];
extern uint64_t zobristTurnKey;


uint64_t randXOR();
void initZobrist();
