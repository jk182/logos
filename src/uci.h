#pragma once

#include "thread.h"

#include <iostream>

void changePosition(std::string command, Board* board);
uint16_t searchPosition(std::string command, Board board);
uint16_t timedSearch(Thread *thread, int thinkingTime);
