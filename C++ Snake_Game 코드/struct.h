#pragma once

#include <ctime>
#include <vector>

enum Direction { UP, DOWN, LEFT, RIGHT };

struct Mission {
    int requiredBodyLength;
    int requiredGrowthItems;
    int requiredPoisonItems;
    int requiredGateUses;
};

extern const Mission missions[];

struct Snake {
    int x, y;
    Direction dir;
    std::vector<std::pair<int, int>> body;
};

struct Item {
    int x, y, type;
    time_t spawnTime;
};

struct Gate {
    int x1, y1, x2, y2;
    time_t spawnTime;
};