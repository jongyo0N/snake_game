#include <unistd.h>
#include <algorithm>
#include <cstring>
#include <vector>
#include <ctime>
#include <ncurses.h>

#include "variable.h"
#include "struct.h"
#include "item.h"

// 아이템 생성 함수
void generateItem(int map[HEIGHT][WIDTH], const Snake& snake, int type) {
    int x, y;
    bool isOnSnake;
    do {
        x = rand() % (HEIGHT - 2) + 1;
        y = rand() % (WIDTH - 2) + 1;
        //뱀 몸통에 생기지 않게 확인
        isOnSnake = std::find(snake.body.begin(), snake.body.end(), std::make_pair(x, y)) != snake.body.end();
    } while (map[x][y] != 0 || isOnSnake);
    //평범한 필드가 아니거나 뱀 몸통에 있으면 다시생성
    map[x][y] = type;   //입력값으로 받은 아이템 종류로 생성
    items.push_back({ x, y, type, time(NULL) });    //아이템 벡터에 추가
}