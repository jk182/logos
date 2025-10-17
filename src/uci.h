#pragma once

#include "thread.h"

#include <iostream>

void changePosition(std::string command, Thread *thread);
uint16_t searchPosition(std::string command, Thread *thread);
uint16_t timedSearch(Thread *thread, int thinkingTime);
