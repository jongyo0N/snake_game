#include <unistd.h>
#include <cstring>
#include <ctime>
#include <vector>
#include <ncurses.h>

#include "variable.h"
#include "struct.h"
#include "window.h"

// 1. ���� �ʱ�ȭ �Լ�(����� ������� �߰�, ������ Pair�� �´� ���� �ؽ�Ʈ ���� �������ش�.)
void init_colors() {
    // ���� ���� ���� ���Ǹ� �߰�
    // ��� ����
    init_color(COLOR_DARKGREEN, 0, 500, 0);
    // ��ο� ����� ����
    init_color(COLOR_DARKYELLOW, 500, 500, 0);
    // ��ο� ��Ʈ���� ����
    init_color(COLOR_DARKCYAN, 0, 500, 500);

    // pair�� �´� ���� �ؽ�Ʈ ���� �����Ѵ�.
    // init_pair(pair ��ȣ, �ؽ�Ʈ ����, ��� ����)
    // (1) �⺻ ���� ��Ÿ ȭ���� ����� �� ����Ѵ�.
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    // (2) ��(Wall, Immune Wall) ����
    init_pair(2, COLOR_RED, COLOR_BLACK);
    // (3) �� �Ӹ� ����
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    // (4) �� ���� ����
    init_pair(4, COLOR_DARKGREEN, COLOR_BLACK);
    // (5) ������ Growth item ����
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);
    // (6) ������ Poison item ����
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    // (7) ����Ʈ ����
    init_pair(7, COLOR_CYAN, COLOR_BLACK);
    // (8) ������ Speed Down item ����
    init_pair(8, COLOR_DARKYELLOW, COLOR_BLACK);
    // (9) ������ Speed Up item ����
    init_pair(9, COLOR_BLUE, COLOR_BLACK);
    // (10) ��Ż ����
    init_pair(10, COLOR_DARKCYAN, COLOR_BLACK);
}


// 2. �ʱ� ȭ�� ���� �Լ�(Game ȭ�� â, ���� ȭ�� â, ������ ȭ�� â, �̼� ȭ�� â, Ŭ���� �ð� ȭ�� â ����)
// ������ â ũ��� ȭ�� ������ ���߱� ���� ���̴� 2��, �ʺ�� 3��� ������ ���߾���.
void screen_init(int height, int width) {
    // (1) ���� ncurses �ʱ�ȭ
    initscr(); // curses ��� ���� �ʱ�ȭ
    start_color(); // �÷� ���
    use_default_colors(); // �⺻ ������ ����Ѵ�.
    init_colors(); // 1�� �Լ��� ���� ���� �ʱ�ȭ
    curs_set(0); // Ŀ�� �����


    // (2) Game ȭ�� ũ�� ���� �� ����
    int screen_height = height * 2; // ���� ȭ���� ���̸� 2��� ����
    int screen_width = width * 3; // ���� ȭ���� �ʺ� 3��� ����
    game_screen = newwin(screen_height, screen_width, 0, 0); // Game ȭ�� â ����
    wbkgd(game_screen, COLOR_PAIR(1)); // Game ȭ�� â�� ������ 1�� Pair�� ����
    box(game_screen, 0, 0); // ������ �����쿡 �׵θ��� �߰��Ѵ�.
    mvwprintw(game_screen, 1, screen_width / 2 - 5, "Snake Game"); // ������ �����쿡 â�� ������ ����Ѵ�.
    refresh(); // ���� ������ �ݿ��Ѵ�
    wrefresh(game_screen); // �����츦 �׷��ش�.


    // (3) ���� ȭ�� ũ�� ���� �� ����
    int score_height = 8; // ���� ȭ���� ���̸� 8�� ����
    int score_width = 30; // ���� ȭ���� �ʺ� 30���� ����
    int score_start_y = 3; // ���� ȭ���� y�� ��ġ ����
    int score_start_x = WIDTH * 3 + 2; // ����ȭ���� x�� ��ġ ����
    score_win = newwin(score_height, score_width, score_start_y, score_start_x); // ���� ȭ�� ����
    wbkgd(score_win, COLOR_PAIR(1)); // ���� ȭ�� â ������ 1�� Pair�� ����
    box(score_win, 0, 0); // ������ �����쿡 �׵θ��� �߰��Ѵ�.
    mvwprintw(score_win, 1, 1, "Score:"); // ������ �����쿡 â�� ������ ����Ѵ�.
    refresh(); // ���� ������ �ݿ��Ѵ�.
    wrefresh(score_win); // �����츦 �׷��ش�.


    // (4) ������ ȭ�� ũ�� ���� �� ����
    int item_height = 8; // ������ ȭ���� ���̸� 8�� ����
    int item_width = 30; // ������ ȭ���� �ʺ� 30���� ����
    int item_start_y = score_start_y + score_height + 1; // ������ ȭ���� y�� ��ġ ����
    int item_start_x = WIDTH * 3 + 2; // ������ ȭ���� x�� ��ġ ����
    item_win = newwin(item_height, item_width, item_start_y, item_start_x); // ������ ȭ�� ����
    wbkgd(item_win, COLOR_PAIR(1)); // ������ ȭ�� â ������ 1�� Pair�� ����
    box(item_win, 0, 0); // ������ �����쿡 �׵θ��� �߰��Ѵ�.
    mvwprintw(item_win, 1, 1, "Items:"); // ������ �����쿡 â�� ������ ����Ѵ�.


    // (5) item â���� ������ ���� �°� �ؽ�Ʈ�� ����Ѵ�.
    // Growth item�� ������ 5�� Pair�� ����
    wattron(item_win, COLOR_PAIR(5)); // �ؽ�Ʈ ������ 5������ ����
    mvwprintw(item_win, 2, 1, "+ : Growth Item"); // �ؽ�Ʈ ���
    wattroff(item_win, COLOR_PAIR(5)); // �ؽ�Ʈ ���� ������ ����.
    // Poison item�� ������ 6�� Pair�� ����
    wattron(item_win, COLOR_PAIR(6)); // �ؽ�Ʈ ������ 6������ ����
    mvwprintw(item_win, 3, 1, "- : Poison Item"); // �ؽ�Ʈ ���
    wattroff(item_win, COLOR_PAIR(6)); // �ؽ�Ʈ ���� ������ ����.
    // Speed Down item�� ������ 8�� Pair�� ����
    wattron(item_win, COLOR_PAIR(8)); // �ؽ�Ʈ ������ 8������ ����
    mvwprintw(item_win, 4, 1, "~ : Speed Down Item"); // �ؽ�Ʈ ���
    wattroff(item_win, COLOR_PAIR(8)); // �ؽ�Ʈ ���� ������ ����.
    // Speed Up item�� ������ 9�� Pair�� ����
    wattron(item_win, COLOR_PAIR(9)); // �ؽ�Ʈ ������ 9������ ����
    mvwprintw(item_win, 5, 1, "^ : Speed Up Item"); // �ؽ�Ʈ ���
    wattroff(item_win, COLOR_PAIR(9)); // �ؽ�Ʈ ���� ������ ����.
    // Portal item�� ������ 10�� Pair�� ����
    wattron(item_win, COLOR_PAIR(10)); // �ؽ�Ʈ ������ 10������ ����
    mvwprintw(item_win, 6, 1, "P : Portal"); // �ؽ�Ʈ ���
    wattroff(item_win, COLOR_PAIR(10)); // �ؽ�Ʈ ���� ������ ����.
    refresh(); // ���� ������ �ݿ��Ѵ�.
    wrefresh(item_win); // �����츦 �׷��ش�.


    // (6) �̼� ȭ�� ũ�� ���� �� ����
    int mission_height = 7; // �̼� ȭ���� ���̸� 7�� ����
    int mission_width = 30; // �̼� ȭ���� �ʺ� 30���� ����
    int mission_start_y = item_start_y + item_height + 1; // �̼� ȭ���� y�� ��ġ ����
    int mission_start_x = WIDTH * 3 + 2; // �̼� ȭ���� x�� ��ġ ����
    mission_win = newwin(mission_height, mission_width, mission_start_y, mission_start_x); // �̼� ȭ�� ����
    wbkgd(mission_win, COLOR_PAIR(1)); // �̼� ȭ�� â ������ 1�� Pair�� ����
    box(mission_win, 0, 0); // ������ �����쿡 �׵θ��� �߰��Ѵ�.
    mvwprintw(mission_win, 1, 1, "Mission:"); // ������ �����쿡 â�� ������ ����Ѵ�.
    refresh(); // ���� ������ �ݿ��Ѵ�.
    wrefresh(mission_win); // �����츦 �׷��ش�.


    // (7) Ŭ���� �ð� ȭ�� ũ�� ���� �� ����
    int clearTime_height = 7; // Ŭ���� �ð� ȭ���� ���̸� 7�� ����
    int clearTime_width = 30; // Ŭ���� �ð� ȭ���� �ʺ� 30���� ����
    int clearTime_start_y = mission_start_y + mission_height + 1; // Ŭ���� �ð� ȭ���� y�� ��ġ ����
    int clearTime_start_x = WIDTH * 3 + 2; // Ŭ���� �ð� ȭ���� x�� ��ġ ����
    clearTime_win = newwin(clearTime_height, clearTime_width, clearTime_start_y, clearTime_start_x); // Ŭ���� �ð� ȭ�� ����
    wbkgd(clearTime_win, COLOR_PAIR(1)); // Ŭ���� �ð� ȭ�� â ������ 1�� Pair�� ����
    box(clearTime_win, 0, 0); // ������ �����쿡 �׵θ��� �߰��Ѵ�.
    mvwprintw(clearTime_win, 1, 1, "Clear Times:"); // ������ �����쿡 â�� ������ ����Ѵ�.
    refresh(); // ���� ������ �ݿ��Ѵ�.
    wrefresh(clearTime_win); // �����츦 �׷��ش�.
}


// 3. ������ �����츦 ���ϴ� ��ġ�� ���ϴ� ����� ���� ���ִ� �Լ�
void update_window(WINDOW* win, int y, int x, int color_pair) {
    wbkgd(win, COLOR_PAIR(color_pair)); // �ش� �������� ������ ������Ʈ
    mvwin(win, y * 2, x * 3); // �����츦 ���ο� ��ġ�� �̵��Ѵ�.
    box(win, 0, 0); // �����쿡 �׵θ��� �߰����ش�.
    wrefresh(win); // ����� ������ �����쿡 �ݿ��Ͽ� �ٽ� �׸���.
}


// 4. map �迭�� �Է¹޾� ��(wall, immunewall) �����츦 �߰��ϴ� �Լ�
void create_wall_windows(int map[HEIGHT][WIDTH]) {
    for (int i = 0; i < HEIGHT; ++i) { // map�� ���� ��ŭ �ݺ�
        for (int j = 0; j < WIDTH; ++j) { // map�� �ʺ� ��ŭ �ݺ�
            if (map[i][j] == WALL || map[i][j] == IMMUNE_WALL) { // wall Ȥ�� immune wall�� ��� ���
                WINDOW* wall_win = newwin(2, 3, i * 2, j * 3); // �� ������ â�� ���� �����Ѵ�.
                wbkgd(wall_win, COLOR_PAIR(2)); // �� �������� ������ 2�� Pair�� ����
                box(wall_win, 0, 0); // �� �����쿡 �׵θ��� �߰��Ѵ�.
                wrefresh(wall_win); // ����� ������ �����쿡 �ݿ��Ͽ� �ٽ� �׸���.
                walls.push_back(wall_win); // ���� ���� ������ ���� �� ������ ������ walls�� �߰��Ѵ�.
            }
        }
    }
}


// 5. Snake ����ü�� �Է¹޾� �� �����츦 �߰��ϴ� �Լ�
void create_snake_windows(const Snake& snake) {
    for (size_t i = 0; i < snake.body.size(); ++i) { // ���� �� ���� ��ŭ �ݺ�
        WINDOW* snake_win = newwin(2, 3, snake.body[i].first * 2, snake.body[i].second * 3); // ���� ������ â�� ���� �����Ѵ�.
        if (i == 0) {
            wbkgd(snake_win, COLOR_PAIR(3)); // �� �Ӹ��� ������ 3�� Pair�� ����
        }
        else {
            wbkgd(snake_win, COLOR_PAIR(4)); // �� ������ ������ 4�� Pair�� ����
        }
        box(snake_win, 0, 0); // �� �����쿡 �׵θ��� �߰��Ѵ�.
        wrefresh(snake_win); // ����� ������ �����쿡 �ݿ��Ͽ� �ٽ� �׸���.
        snake_positions.push_back({ snake.body[i].first, snake.body[i].second }); // ���� ��ġ�� �����ϱ� ���� ���Ϳ� �����Ѵ�.
    }
}


// 6. map �迭�� �Է¹޾� item �����츦 �߰��ϴ� �Լ�
void create_item_windows(int map[HEIGHT][WIDTH]) {
    for (const auto& item : items) { // item ��Ҹ� �ҷ��´�.
        WINDOW* item_win = newwin(2, 3, item.x * 2, item.y * 3); // �������� ������ â�� ���� �����Ѵ�.
        int color_pair;

        // ������ ������ ���� ������ �����Ѵ�.
        // Growth item = 5
        // Poison item = 6
        // Speed down item = 8
        // Speed up item = 9
        if (item.type == GROWTH_ITEM) {
            color_pair = COLOR_PAIR(5);
        }
        else if (item.type == POISON_ITEM) {
            color_pair = COLOR_PAIR(6);
        }
        else if (item.type == SPEEDDOWN_ITEM) {
            color_pair = COLOR_PAIR(8);
        }
        else if (item.type == SPEEDUP_ITEM) {
            color_pair = COLOR_PAIR(9);
        }
        wbkgd(item_win, color_pair); // item ������ ���� ����
        box(item_win, 0, 0); // item �����쿡 �׵θ��� �߰��Ѵ�.

        // ������ �����쿡 �������� �����ϴ� �ؽ�Ʈ �߰�
        // Growth item -> +
        // Poison item -> -
        // Speed down item -> ~
        // Speed up item -> ^
        if (item.type == GROWTH_ITEM) {
            mvwprintw(item_win, 1, 1, "+");
        }
        else if (item.type == POISON_ITEM) {
            mvwprintw(item_win, 1, 1, "-");
        }
        else if (item.type == SPEEDDOWN_ITEM) {
            mvwprintw(item_win, 1, 1, "~");
        }
        else if (item.type == SPEEDUP_ITEM) {
            mvwprintw(item_win, 1, 1, "^");
        }
        wrefresh(item_win); // ����� ������ �����쿡 �ݿ��Ͽ� �ٽ� �׸���.
        item_windows.push_back(item_win); // ���� ���� ������ �����츦 ������ ������ ������ item_windows�� �߰��Ѵ�.
    }
}


// 7. Gate ����ü�� �Է¹޾� gate �����츦 �߰��ϴ� �Լ�
void create_gate_windows(const Gate& gate) {
    if (gate.x1 != -1 && gate.y1 != -1 && gate.x2 != -1 && gate.y2 != -1) { // immunewall�� ��쿡 ����
        // ����Ʈ ������� 2���� ����Ʈ�� �־���Ѵ�.(���°� �����°�(��ȯ ��Ģ ����))
        WINDOW* gate_win1 = newwin(2, 3, gate.x1 * 2, gate.y1 * 3); // ����Ʈ�� ������ â�� ���� �����Ѵ�.
        WINDOW* gate_win2 = newwin(2, 3, gate.x2 * 2, gate.y2 * 3); // ����Ʈ�� ������ â�� ���� �����Ѵ�.
        wbkgd(gate_win1, COLOR_PAIR(7)); // ����Ʈ ������ ���� ����
        wbkgd(gate_win2, COLOR_PAIR(7)); // ����Ʈ ������ ���� ����
        box(gate_win1, 0, 0); // ����Ʈ �����쿡 �׵θ��� �߰��Ѵ�.
        box(gate_win2, 0, 0); // ����Ʈ �����쿡 �׵θ��� �߰��Ѵ�.
        wrefresh(gate_win1); // ����� ������ �����쿡 �ݿ��Ͽ� �ٽ� �׸���.
        wrefresh(gate_win2); // ����� ������ �����쿡 �ݿ��Ͽ� �ٽ� �׸���.
        walls.push_back(gate_win1); // ���� ���� ����Ʈ �����츦 walls ���Ϳ� �߰��Ѵ�.
        walls.push_back(gate_win2); // ���� ���� ����Ʈ �����츦 walls ���Ϳ� �߰��Ѵ�.
    }
}


// 8. ������ ���͸� �Է¹޾� �����츦 �����ϴ� �Լ�
void delete_windows(std::vector<WINDOW*>& windows) {
    for (auto& win : windows) { // ���Ϳ� �ִ� �� �����쿡 ���� �ݺ��Ѵ�.
        delwin(win); // �����츦 �����Ѵ�.
    }
    windows.clear(); // ���͸� ����ش�.
}


// 9. Snake ������ ���͸� �Է¹޾� �����츦 �����ϴ� �Լ�
void delete_snake_windows(const std::vector<std::pair<int, int>>& positions) {
    for (const auto& pos : positions) { // Snake ��ġ ������ ������ �ִ� ������ ���Ϳ� ���� �ݺ��Ѵ�.
        WINDOW* win = newwin(2, 3, pos.first * 2, pos.second * 3); // �ش� ��ġ�� ���� ����
        wbkgd(win, COLOR_PAIR(1));
        wrefresh(win);
        delwin(win);
    }
}


// 10. ���� ���� �� �̼��� ��� ���ִ� �Լ�(������, �̼� ȭ���� ����� �ٽ� �����Ѵ�.)
void drawScore(const Snake& snake, int elapsed_time) {
    const Mission& mission = missions[currentStage];

    werase(score_win); // ������ ������ �ؽ�Ʈ�� �����Ѵ�.
    box(score_win, 0, 0); // ������ �����쿡 �׵θ��� �߰��Ѵ�.
    mvwprintw(score_win, 1, 1, "Score Board"); // �����쿡 â�� ������ ����Ѵ�.
    mvwprintw(score_win, 2, 1, "B: (%d) / (Max Length)", snake.body.size()); // �����쿡 �� ���� ���
    mvwprintw(score_win, 3, 1, "+: %d", growthItems); // �����쿡 Growthitem ���� Ƚ�� ���
    mvwprintw(score_win, 4, 1, "-: %d", poisonItems); // �����쿡 Posionitem ���� Ƚ�� ���
    mvwprintw(score_win, 5, 1, "G: %d", gateUses); // �����쿡 Gate�� �̿��� Ƚ�� ���
    mvwprintw(score_win, 6, 1, "Time: %d sec", elapsed_time); // �ش� Stage �ð� ���
    wrefresh(score_win); // ����� ������ �����쿡 �ݿ��Ͽ� �ٽ� �׸���.

    // �̼� ������ ������Ʈ
    werase(mission_win); // ������ ������ �ؽ�Ʈ�� �����Ѵ�.
    box(mission_win, 0, 0); // ������ �����쿡 �׵θ��� �߰��Ѵ�.
    mvwprintw(mission_win, 1, 1, "Mission"); // �����쿡 â�� ������ ����Ѵ�.
    // �����쿡 ��ǥ �� ���� ���
    mvwprintw(mission_win, 2, 1, "B: %d (%s)", mission.requiredBodyLength, snake.body.size() >= mission.requiredBodyLength ? "v" : " ");
    // �����쿡 ��ǥ Growthitem Ƚ�� ���
    mvwprintw(mission_win, 3, 1, "+: %d (%s)", mission.requiredGrowthItems, growthItems >= mission.requiredGrowthItems ? "v" : " ");
    // �����쿡 ��ǥ Posionitem Ƚ�� ���
    mvwprintw(mission_win, 4, 1, "-: %d (%s)", mission.requiredPoisonItems, poisonItems >= mission.requiredPoisonItems ? "v" : " ");
    // �����쿡 ��ǥ Gate�� �̿� Ƚ�� ���
    mvwprintw(mission_win, 5, 1, "G: %d (%s)", mission.requiredGateUses, gateUses >= mission.requiredGateUses ? "v" : " ");
    wrefresh(mission_win); // ����� ������ �����쿡 �ݿ��Ͽ� �ٽ� �׸���.
}


// 11. �̺�Ʈ �߻��� ȭ�鿡 �޽����� 3�� ���� ������ִ� �Լ�(���� ����, ���� ���������� �̵�, ���� Ŭ����) 
void showMessage(const char* message) {
    int height, width;
    getmaxyx(stdscr, height, width); // ���� �͹̳� �����쿡 �ִ� ũ�⸦ ����, ���η� ���Ѵ�.
    int msg_width = width / 2; // �޽��� ���� ũ�⸦ 2���� 1�� ����
    WINDOW* msg_win = newwin(5, msg_width, height / 2 - 2, (width - msg_width) / 2); // �޽����� ȭ�� �߾ӿ� ��ġ�ϰ� �Ѵ�.

    for (int i = 3; i >= 1; --i) { // 3�� ���� �ݺ�
        werase(msg_win); // �޽��� â ���� �ʱ�ȭ
        box(msg_win, 0, 0); //�����쿡 �׵θ��� �߰��Ѵ�.
        mvwprintw(msg_win, 2, (msg_width - strlen(message)) / 2, "%s", message); // �޽����� ����Ѵ�.
        mvwprintw(msg_win, 3, msg_width / 2 - 1, " %d ", i); // �� �� �ڿ� ���� �ൿ �Ѿ���� ����Ѵ�.(3,2,1)
        wrefresh(msg_win); // ����� ������ �����쿡 �ݿ��Ͽ� �ٽ� �׸���.
        usleep(1000000); // 1�ʵ��� ���
    }

    werase(msg_win); // �޽��� â ���� �����
    wrefresh(msg_win); // ȭ�� ����
    delwin(msg_win); // �޽��� â ����
    refresh(); // ��ü ȭ�� ����
}