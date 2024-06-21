#pragma once

#include <ncurses.h>
#include <vector>
#include "variable.h"
#include "struct.h"

void init_colors();
void screen_init(int height, int width);
void update_window(WINDOW* win, int y, int x, int color_pair);
void create_wall_windows(int map[HEIGHT][WIDTH]);
void create_snake_windows(const Snake& snake);
void create_item_windows(int map[HEIGHT][WIDTH]);
void create_gate_windows(const Gate& gate);
void delete_windows(std::vector<WINDOW*>& windows);
void delete_snake_windows(const std::vector<std::pair<int, int>>& positions);
void drawScore(const Snake& snake, int elapsed_time);
void showMessage(const char* message);