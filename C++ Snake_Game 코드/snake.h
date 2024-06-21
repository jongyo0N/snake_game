#pragma once

#include <ncurses.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "variable.h"
#include "struct.h"


void initMap(int map[HEIGHT][WIDTH], int stageIndex);
void initSnake(Snake& snake);
void moveSnake(int map[HEIGHT][WIDTH], Snake& snake);
bool checkMissionComplete(const Snake& snake);