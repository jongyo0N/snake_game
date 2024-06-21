#include <unistd.h>
#include <algorithm>
#include <cstring>
#include <vector>
#include <ctime>
#include <ncurses.h>

#include "variable.h"
#include "struct.h"
#include "item.h"

// ������ ���� �Լ�
void generateItem(int map[HEIGHT][WIDTH], const Snake& snake, int type) {
    int x, y;
    bool isOnSnake;
    do {
        x = rand() % (HEIGHT - 2) + 1;
        y = rand() % (WIDTH - 2) + 1;
        //�� ���뿡 ������ �ʰ� Ȯ��
        isOnSnake = std::find(snake.body.begin(), snake.body.end(), std::make_pair(x, y)) != snake.body.end();
    } while (map[x][y] != 0 || isOnSnake);
    //����� �ʵ尡 �ƴϰų� �� ���뿡 ������ �ٽû���
    map[x][y] = type;   //�Է°����� ���� ������ ������ ����
    items.push_back({ x, y, type, time(NULL) });    //������ ���Ϳ� �߰�
}