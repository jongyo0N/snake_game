#pragma once

#include <ncurses.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "variable.h"
#include "struct.h"

void generateGates(int map[HEIGHT][WIDTH]);
void adjustGateExit(int map[HEIGHT][WIDTH], int& x, int& y, Direction& dir);
void generatePortal(int map[HEIGHT][WIDTH]);