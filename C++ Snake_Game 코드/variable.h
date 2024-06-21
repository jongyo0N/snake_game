#pragma once

#include <vector>
#include <ncurses.h>
#include <ctime>
#include "struct.h"

#define WALL 1
#define IMMUNE_WALL 2
#define SNAKE_HEAD 3
#define SNAKE_BODY 4
#define GROWTH_ITEM 5
#define POISON_ITEM 6
#define GATE 7
#define SPEEDDOWN_ITEM 8
#define SPEEDUP_ITEM 9
#define STAGE_PORTAL 10
#define COLOR_DARKGREEN 11
#define COLOR_DARKYELLOW 12
#define COLOR_DARKCYAN 13
#define WIDTH 21
#define HEIGHT 21


extern const int stage[4][21][21];
extern time_t stageStartTime;
extern int clearTimes[4];
extern int score;
extern int growthItems;
extern int poisonItems;
extern int speeddownItems;
extern int speedupItems;
extern int gateUses;
extern const int TARGET_SCORE;
extern const int MAX_ITEMS;
extern const int ITEM_DURATION;
extern const int GATE_DURATION;
extern int tick_duration;
extern int currentStage;
extern bool portalGenerated;
extern bool passingGate;

extern std::vector<Item> items;
extern Gate gate;
extern std::vector<WINDOW*> walls;
extern std::vector<std::pair<int, int>> snake_positions;
extern std::vector<WINDOW*> item_windows;
extern WINDOW* game_screen;
extern WINDOW* score_win;
extern WINDOW* item_win;
extern WINDOW* mission_win;
extern WINDOW* clearTime_win;

