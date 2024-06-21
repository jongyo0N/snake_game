#include <unistd.h>
#include <algorithm>
#include <cstring>
#include <vector>
#include <ctime>
#include <ncurses.h>

#include "variable.h"
#include "struct.h"
#include "snake.h"
#include "item.h"
#include "gate.h"
#include "window.h"

// �� �ʱ�ȭ �Լ�
void initMap(int map[HEIGHT][WIDTH], int stageIndex) {
    // �־��� �������� �迭�� ���� �� �迭�� ����
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            map[i][j] = stage[stageIndex][i][j];
        }
    }
}

// �� �ʱ�ȭ �Լ�
void initSnake(Snake& snake) {
    //�ʱ� ��ġ�� ���� ���
    snake.x = HEIGHT / 2;
    snake.y = WIDTH / 2;
    snake.dir = RIGHT;
    snake.body = { {snake.x, snake.y}, {snake.x, snake.y - 1}, {snake.x, snake.y - 2} };
}

// �� �̵� ó�� �Լ�
void moveSnake(int map[HEIGHT][WIDTH], Snake& snake) {
    passingGate = false;

    //���� ���⿡ ���� �̵� ��ǥ ����
    int dx = 0, dy = 0;
    switch (snake.dir) {
    case UP:    dx = -1; break;
    case DOWN:  dx = 1; break;
    case LEFT:  dy = -1; break;
    case RIGHT: dy = 1; break;
    }
    //���ο� ��ǥ�� �̵�
    int newX = snake.body.front().first + dx;
    int newY = snake.body.front().second + dy;

    // ���̳� �ڱ� �ڽŰ��� �浹 ó��
    if (map[newX][newY] == WALL || map[newX][newY] == IMMUNE_WALL ||
        std::find(snake.body.begin(), snake.body.end(), std::make_pair(newX, newY)) != snake.body.end()) {
        // �浹 �� ���� ���� ó��
        showMessage("Game Over! You collided with an obstacle or yourself.");
        endwin();
        exit(0);
    }

    // ����Ʈ �̵� ó��
    if (map[newX][newY] == GATE) {
        passingGate = true;
        //�ⱸ ����Ʈ�� �̵� ��ǥ ����
        if (newX == gate.x1 && newY == gate.y1) {
            newX = gate.x2;
            newY = gate.y2;
        }
        else {
            newX = gate.x1;
            newY = gate.y1;
        }
        //������� ����
        adjustGateExit(map, newX, newY, snake.dir);
        gateUses++;
    }
    else {
        passingGate = false;
    }

    // ��Ż �̵� ó��(������������ �Ѿ��)
    if (map[newX][newY] == STAGE_PORTAL) {
        time_t endTime = time(NULL);
        clearTimes[currentStage] = difftime(endTime, stageStartTime);

        currentStage++;
        //������ �������� Ŭ���� ��
        if (currentStage >= 4) {
            // �� Ŭ���� �ð� ���
            int totalClearTime = 0;
            for (int i = 0; i < 4; ++i) {
                totalClearTime += clearTimes[i];
            }
            // �� Ŭ���� �ð� ǥ��
            char totalTimeMessage[100];
            snprintf(totalTimeMessage, sizeof(totalTimeMessage), "Congratulations! You completed all stages.\nTotal clear time: %d sec", totalClearTime);
            showMessage(totalTimeMessage);

            endwin();
            exit(0);
        }

        // �ȳ��� ǥ��
        showMessage("Proceeding to the next stage...");

        //â �ʱ�ȭ �۾�
        items.clear();
        delete_windows(walls);
        delete_windows(item_windows);
        snake_positions.clear();
        delwin(game_screen);
        delwin(score_win);
        delwin(item_win);
        delwin(mission_win);
        delwin(clearTime_win);

        // ���� ����Ʈ ����
        if (gate.x1 != -1 && gate.y1 != -1 && gate.x2 != -1 && gate.y2 != -1) {
            map[gate.x1][gate.y1] = WALL;
            map[gate.x2][gate.y2] = WALL;
            gate = { -1, -1, -1, -1, 0 }; // ����Ʈ �ʱ�ȭ
        }

        initMap(map, currentStage);
        initSnake(snake);

        // �̼� ���� ���� �ʱ�ȭ
        growthItems = 0;
        poisonItems = 0;
        speeddownItems = 0;
        speedupItems = 0;
        gateUses = 0;
        portalGenerated = false; // ��Ż ���� ���� �ʱ�ȭ

        // �̵� �ӵ� �ʱ�ȭ
        tick_duration = 300000; // �⺻ �̵� �ӵ� �ʱ�ȭ

        // ������ �� ����Ʈ �����
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

        // ȭ�� �ʱ�ȭ
        screen_init(HEIGHT, WIDTH);
        nodelay(game_screen, TRUE);
        keypad(game_screen, TRUE);

        // �̼� ������ �ʱ�ȭ
        werase(mission_win);
        box(mission_win, 0, 0);
        mvwprintw(mission_win, 1, 1, "Mission");
        mvwprintw(mission_win, 2, 1, "B: 5 ( )");
        mvwprintw(mission_win, 3, 1, "+: 2 ( )");
        mvwprintw(mission_win, 4, 1, "-: 2 ( )");
        mvwprintw(mission_win, 5, 1, "G: 1 ( )");
        wrefresh(mission_win);

        // Ŭ���� �ð� â �ʱ�ȭ �� ǥ��
        werase(clearTime_win);
        box(clearTime_win, 0, 0);
        mvwprintw(clearTime_win, 1, 1, "Clear Times:");
        for (int i = 0; i < currentStage; ++i) {  // ���� �������������� ǥ��
            mvwprintw(clearTime_win, 2 + i, 1, "Stage %d: %d sec", i + 1, clearTimes[i]);
        }
        wrefresh(clearTime_win);

        // �� �������� ��� ���� �� ȭ�� ����
        create_wall_windows(map);
        create_item_windows(map);
        create_gate_windows(gate);
        create_snake_windows(snake);
        drawScore(snake, 0);

        stageStartTime = time(NULL); // ���ο� �������� ���� �ð� ���

        return;
    }

    // ������ ó��
    //�� ���� ���� ������
    if (map[newX][newY] == GROWTH_ITEM) {
        score++;    //���� ����
        growthItems++;  //������ ���� ���� ����
        //������ ������ ���� �ӽ� ���� ���� �߰�
        snake.body.push_back({ -1, -1 });
        map[newX][newY] = 0;    //�Ϲ� �ʵ�� ����
        //������ ���Ϳ��� ����
        items.erase(std::remove_if(items.begin(), items.end(), [&](const Item& item) {
            return item.x == newX && item.y == newY && item.type == GROWTH_ITEM;
            }), items.end());
    }
    //�� ���� ���� ������
    else if (map[newX][newY] == POISON_ITEM) {
        score--;    //���� ����
        poisonItems++;  //������ ���� ���� ����
        map[newX][newY] = 0;    //�Ϲ� �ʵ�� ����
        //������ ���Ϳ��� ����
        items.erase(std::remove_if(items.begin(), items.end(), [&](const Item& item) {
            return item.x == newX && item.y == newY && item.type == POISON_ITEM;
            }), items.end());
        snake.body.pop_back();
        //���̰� 3���� �پ��� ���ӿ���
        if (snake.body.size() < 3) {
            showMessage("Game Over! The snake has no body left.");
            endwin();
            exit(0);
        }
    }
    else if (map[newX][newY] == SPEEDDOWN_ITEM) {
        score--;    //���� ����
        speeddownItems++; // ������ ���� ���� ����
        map[newX][newY] = 0;    //�Ϲ� �ʵ�� ����
        //������ ���Ϳ��� ����
        items.erase(std::remove_if(items.begin(), items.end(), [&](const Item& item) {
            return item.x == newX && item.y == newY && item.type == SPEEDDOWN_ITEM;
            }), items.end());
        tick_duration *= 2; // �̵� �ӵ� �� �� ����
    }
    else if (map[newX][newY] == SPEEDUP_ITEM) {
        score++;    //���� ����
        speedupItems++; // ������ ���� ���� ����
        map[newX][newY] = 0;    //�Ϲ� �ʵ�� ����
        //������ ���Ϳ��� ����
        items.erase(std::remove_if(items.begin(), items.end(), [&](const Item& item) {
            return item.x == newX && item.y == newY && item.type == SPEEDUP_ITEM;
            }), items.end());
        tick_duration /= 2; // �̵� �ӵ� �� �� ����
    }

    // �� �̵� ������Ʈ
    std::pair<int, int> prev = snake.body.front();
    //���ο� ��ǥ�� ������ 
    snake.body.front() = { newX, newY };
    //���� �� ĭ�� ������ �̵�
    for (size_t i = 1; i < snake.body.size(); ++i) {
        std::pair<int, int> temp = snake.body[i];
        snake.body[i] = prev;
        prev = temp;
    }

    // ���������� ���� ����Ʈ ���� ���� üũ �� ����
    if (!passingGate &&
        ((currentStage == 0 && snake.body.size() >= 5) ||
            (currentStage == 1 && snake.body.size() >= 6) ||
            (currentStage == 2 && snake.body.size() >= 7) ||
            (currentStage == 3 && snake.body.size() >= 8))) {
        if (difftime(time(NULL), gate.spawnTime) >= GATE_DURATION) {
            generateGates(map);
        }
    }

    // ȭ�� ������Ʈ
    delete_snake_windows(snake_positions);
    snake_positions.clear();
    create_snake_windows(snake);
}

// �̼� �Ϸ� üũ �Լ�
bool checkMissionComplete(const Snake& snake) {
    const Mission& mission = missions[currentStage];
    return (snake.body.size() >= mission.requiredBodyLength &&
        growthItems >= mission.requiredGrowthItems &&
        poisonItems >= mission.requiredPoisonItems &&
        gateUses >= mission.requiredGateUses);
}