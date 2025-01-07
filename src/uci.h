#pragma once

#include <iostream>

void changePosition(std::string command, Board* board);
Board* updateHistory(std::string command, Board board, Board *history);
uint16_t searchPosition(std::string command, Board board, Board *history, int length);
