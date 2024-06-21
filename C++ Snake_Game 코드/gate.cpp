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

// 게이트 생성 함수
void generateGates(int map[HEIGHT][WIDTH]) {
    if (passingGate) {
        return; // 뱀이 게이트를 통과하는 동안 새로운 게이트를 생성하지 않음
    }
    // gate는 전역변수
    // 이전 게이트를 벽으로 변환
    if (gate.x1 != -1 && gate.y1 != -1 && gate.x2 != -1 && gate.y2 != -1) {
        map[gate.x1][gate.y1] = WALL;
        map[gate.x2][gate.y2] = WALL;
    }

    // 모서리가 아닌 벽에 게이트1 생성
    int x1, y1, x2, y2;
    do {
        x1 = rand() % HEIGHT;
        y1 = rand() % WIDTH;
    } while (map[x1][y1] != WALL || map[x1][y1] == IMMUNE_WALL);

    // 게이트1과 다른 위치, 모서리가 아닌 벽에 게이트2 생성
    do {
        x2 = rand() % HEIGHT;
        y2 = rand() % WIDTH;
    } while ((x2 == x1 && y2 == y1) || map[x2][y2] != WALL || map[x2][y2] == IMMUNE_WALL);

    gate = { x1, y1, x2, y2, time(NULL) };
    map[x1][y1] = GATE;
    map[x2][y2] = GATE;
}

// 맵 경계 및 규칙에 따라 게이트 출구 방향 조정
void adjustGateExit(int map[HEIGHT][WIDTH], int& x, int& y, Direction& dir) {
    if (x == 0) {
        x = 1; // 상단 경계에서 내부로 이동
        dir = DOWN;
    }
    else if (x == HEIGHT - 1) {
        x = HEIGHT - 2; // 하단 경계에서 내부로 이동
        dir = UP;
    }
    else if (y == 0) {
        y = 1; // 좌측 경계에서 내부로 이동
        dir = RIGHT;
    }
    else if (y == WIDTH - 1) {
        y = WIDTH - 2; // 우측 경계에서 내부로 이동
        dir = LEFT;
    }
    else if (map[x + 1][y] == WALL && map[x - 1][y] == WALL) {
        //진출 방향이 좌우
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
        // 진출 방향이 상하
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
        // 진출 방향이 위에만 막혀있는 경우
        y += 1;
        dir = RIGHT;
    }
    else if (map[x - 1][y] == WALL && dir == DOWN) {
        // 진출 방향이 아래만 막혀있는 경우
        y -= 1;
        dir = LEFT;
    }
    else if (map[x][y - 1] == WALL && dir == LEFT) {
        // 진출 방향이 왼쪽만 막혀있는 경우
        x += 1;
        dir = DOWN;
    }
    else if (map[x][y + 1] == WALL && dir == RIGHT) {
        // 진출 방향이 오른쪽만 막혀있는 경우
        x -= 1;
        dir == UP;
    }
}

// 포탈 생성 함수
void generatePortal(int map[HEIGHT][WIDTH]) {
    int x, y;
    // 벽 위치를 찾기 위해 랜덤 좌표를 설정
    do {
        x = rand() % HEIGHT;
        y = rand() % WIDTH;
    } while (map[x][y] != WALL);

    map[x][y] = STAGE_PORTAL; // 지도 배열에 포탈 위치 설정
    portalGenerated = true;

    WINDOW* portal_win = newwin(2, 3, x * 2, y * 3); // 2x3 윈도우에 맞게 조정
    wbkgd(portal_win, COLOR_PAIR(10)); // 포탈 색상 설정
    box(portal_win, 0, 0);
    mvwprintw(portal_win, 1, 1, "P");
    wrefresh(portal_win);
    walls.push_back(portal_win);
}
