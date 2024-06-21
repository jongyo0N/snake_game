#include <ncurses.h>
#include <unistd.h>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <vector>

#include "snake.h"
#include "item.h"
#include "gate.h"
#include "window.h"
#include "variable.h"

// Global Variables Initialization
const int stage[4][21][21] = {
    {
        {2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1},
        {1,0,0,1,1,1,1,1,0,0,0,0,0,1,0,0,0,0,0,0,1},
        {1,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
        {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2},
    },
    {
        {2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
        {1,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1},
        {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
        {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
        {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
        {1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
        {1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2}
    },
    {
        {2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1},
        {1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,1,0,0,0,1},
        {1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
        {1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
        {1,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2}
    },
    {
        {2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1},
        {1,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1},
        {1,0,0,1,0,0,1,0,0,1,0,0,0,1,0,0,0,1,0,0,1},
        {1,0,0,1,0,0,1,0,0,1,0,0,0,1,0,0,0,1,0,0,1},
        {1,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,1,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,1,0,0,1,0,0,0,0,0,0,1,0,0,0,1,0,0,1},
        {1,0,0,1,0,0,1,0,0,1,0,0,0,1,0,0,0,1,0,0,1},
        {1,0,0,1,0,0,1,0,0,1,0,0,0,1,0,0,0,1,0,0,1},
        {1,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1},
        {1,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1},
        {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2},
    }
};

time_t stageStartTime;
int clearTimes[4] = { 0, 0, 0, 0 };
int score = 0;
int growthItems = 0;
int poisonItems = 0;
int speeddownItems = 0;
int speedupItems = 0;
int gateUses = 0;
const int TARGET_SCORE = 100;
const int MAX_ITEMS = 3;
const int ITEM_DURATION = 5;
const int GATE_DURATION = 10;
int tick_duration = 300000;
int currentStage = 0;
bool portalGenerated = false;
bool passingGate = false;

std::vector<Item> items;
Gate gate = { -1, -1, -1, -1, 0 };

std::vector<WINDOW*> walls;
std::vector<std::pair<int, int>> snake_positions;
std::vector<WINDOW*> item_windows;
WINDOW* game_screen;
WINDOW* score_win;
WINDOW* item_win;
WINDOW* mission_win;
WINDOW* clearTime_win;

const Mission missions[] = {
    {4, 1, 1, 1},
    {4, 1, 1, 1},
    {4, 1, 1, 1},
    {4, 1, 1, 1}
};

int main() {
    srand(time(NULL));

    int map[HEIGHT][WIDTH];
    Snake snake;

    initMap(map, currentStage);
    initSnake(snake);

    stageStartTime = time(NULL);

    screen_init(HEIGHT, WIDTH);
    nodelay(game_screen, TRUE);
    keypad(game_screen, TRUE);

    for (int i = 0; i < MAX_ITEMS; ++i) {
        int randnum = rand() % 4;
        int type;
        if (randnum == 0) {
            type = GROWTH_ITEM;
        }
        else if (randnum == 1) {
            type = POISON_ITEM;
        }
        else if (randnum == 2) {
            type = SPEEDDOWN_ITEM;
        }
        else if (randnum == 3) {
            type = SPEEDUP_ITEM;
        }
        generateItem(map, snake, type);
    }

    while (true) {
        int ch = wgetch(game_screen);
        switch (ch) {
        case KEY_UP:
            if (snake.dir == DOWN) {
                showMessage("Game Over! You tried to move in the opposite direction.");
                endwin();
                return 0;
            }
            snake.dir = UP;
            break;
        case KEY_DOWN:
            if (snake.dir == UP) {
                showMessage("Game Over! You tried to move in the opposite direction.");
                endwin();
                return 0;
            }
            snake.dir = DOWN;
            break;
        case KEY_LEFT:
            if (snake.dir == RIGHT) {
                showMessage("Game Over! You tried to move in the opposite direction.");
                endwin();
                return 0;
            }
            snake.dir = LEFT;
            break;
        case KEY_RIGHT:
            if (snake.dir == LEFT) {
                showMessage("Game Over! You tried to move in the opposite direction.");
                endwin();
                return 0;
            }
            snake.dir = RIGHT;
            break;
        case 'q':
            endwin();
            return 0;
        }

        moveSnake(map, snake);

        if (checkMissionComplete(snake) && !portalGenerated) {
            generatePortal(map);
        }

        int elapsed_time = difftime(time(NULL), stageStartTime);

        delete_windows(walls);
        delete_windows(item_windows);

        create_wall_windows(map);
        create_item_windows(map);
        create_gate_windows(gate);
        drawScore(snake, elapsed_time);

        time_t now = time(NULL);
        items.erase(std::remove_if(items.begin(), items.end(), [&](const Item& item) {
            if (difftime(now, item.spawnTime) >= ITEM_DURATION) {
                map[item.x][item.y] = 0;
                WINDOW* item_win = newwin(2, 3, item.x * 2, item.y * 3);
                wbkgd(item_win, COLOR_PAIR(1));
                wrefresh(item_win);
                delwin(item_win);
                return true;
            }
            return false;
            }), items.end());

        while (items.size() < MAX_ITEMS) {
            int randnum = rand() % 4;
            int type;
            if (randnum == 0) {
                type = GROWTH_ITEM;
            }
            else if (randnum == 1) {
                type = POISON_ITEM;
            }
            else if (randnum == 2) {
                type = SPEEDDOWN_ITEM;
            }
            else if (randnum == 3) {
                type = SPEEDUP_ITEM;
            }
            generateItem(map, snake, type);
        }

        usleep(tick_duration);
    }

    endwin();
    return 0;
}
