#include <unistd.h>
#include <algorithm>
#include <cstring>
#include <vector>
#include <ctime>
#include <ncurses.h>

#include "variable.h"
#include "struct.h"
#include "gate.h"
#include "window.h"

// ����Ʈ ���� �Լ�
void generateGates(int map[HEIGHT][WIDTH]) {
    if (passingGate) {
        return; // ���� ����Ʈ�� ����ϴ� ���� ���ο� ����Ʈ�� �������� ����
    }
    // gate�� ��������
    // ���� ����Ʈ�� ������ ��ȯ
    if (gate.x1 != -1 && gate.y1 != -1 && gate.x2 != -1 && gate.y2 != -1) {
        map[gate.x1][gate.y1] = WALL;
        map[gate.x2][gate.y2] = WALL;
    }

    // �𼭸��� �ƴ� ���� ����Ʈ1 ����
    int x1, y1, x2, y2;
    do {
        x1 = rand() % HEIGHT;
        y1 = rand() % WIDTH;
    } while (map[x1][y1] != WALL || map[x1][y1] == IMMUNE_WALL);

    // ����Ʈ1�� �ٸ� ��ġ, �𼭸��� �ƴ� ���� ����Ʈ2 ����
    do {
        x2 = rand() % HEIGHT;
        y2 = rand() % WIDTH;
    } while ((x2 == x1 && y2 == y1) || map[x2][y2] != WALL || map[x2][y2] == IMMUNE_WALL);

    gate = { x1, y1, x2, y2, time(NULL) };
    map[x1][y1] = GATE;
    map[x2][y2] = GATE;
}

// �� ��� �� ��Ģ�� ���� ����Ʈ �ⱸ ���� ����
void adjustGateExit(int map[HEIGHT][WIDTH], int& x, int& y, Direction& dir) {
    if (x == 0) {
        x = 1; // ��� ��迡�� ���η� �̵�
        dir = DOWN;
    }
    else if (x == HEIGHT - 1) {
        x = HEIGHT - 2; // �ϴ� ��迡�� ���η� �̵�
        dir = UP;
    }
    else if (y == 0) {
        y = 1; // ���� ��迡�� ���η� �̵�
        dir = RIGHT;
    }
    else if (y == WIDTH - 1) {
        y = WIDTH - 2; // ���� ��迡�� ���η� �̵�
        dir = LEFT;
    }
    else if (map[x + 1][y] == WALL && map[x - 1][y] == WALL) {
        //���� ������ �¿�
        if (dir == LEFT || dir == DOWN) {
            y -= 1;
            dir = LEFT;
        }
        else if (dir == RIGHT || dir == UP) {
            y += 1;
            dir = RIGHT;
        }
    }
    else if (map[x][y + 1] == WALL && map[x][y - 1] == WALL) {
        // ���� ������ ����
        if (dir == LEFT || dir == DOWN) {
            x += 1;
            dir == DOWN;
        }
        else if (dir == RIGHT || dir == UP) {
            x -= 1;
            dir == UP;
        }
    }
    else if (map[x + 1][y] == WALL && dir == UP) {
        // ���� ������ ������ �����ִ� ���
        y += 1;
        dir = RIGHT;
    }
    else if (map[x - 1][y] == WALL && dir == DOWN) {
        // ���� ������ �Ʒ��� �����ִ� ���
        y -= 1;
        dir = LEFT;
    }
    else if (map[x][y - 1] == WALL && dir == LEFT) {
        // ���� ������ ���ʸ� �����ִ� ���
        x += 1;
        dir = DOWN;
    }
    else if (map[x][y + 1] == WALL && dir == RIGHT) {
        // ���� ������ �����ʸ� �����ִ� ���
        x -= 1;
        dir == UP;
    }
}

// ��Ż ���� �Լ�
void generatePortal(int map[HEIGHT][WIDTH]) {
    int x, y;
    // �� ��ġ�� ã�� ���� ���� ��ǥ�� ����
    do {
        x = rand() % HEIGHT;
        y = rand() % WIDTH;
    } while (map[x][y] != WALL);

    map[x][y] = STAGE_PORTAL; // ���� �迭�� ��Ż ��ġ ����
    portalGenerated = true;

    WINDOW* portal_win = newwin(2, 3, x * 2, y * 3); // 2x3 �����쿡 �°� ����
    wbkgd(portal_win, COLOR_PAIR(10)); // ��Ż ���� ����
    box(portal_win, 0, 0);
    mvwprintw(portal_win, 1, 1, "P");
    wrefresh(portal_win);
    walls.push_back(portal_win);
}
