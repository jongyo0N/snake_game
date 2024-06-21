#include <unistd.h>
#include <cstring>
#include <ctime>
#include <vector>
#include <ncurses.h>

#include "variable.h"
#include "struct.h"
#include "window.h"

// 1. 색깔 초기화 함수(사용할 색깔들을 추가, 아이템 Pair에 맞는 배경과 텍스트 색을 정의해준다.)
void init_colors() {
    // 없는 색깔에 대한 정의를 추가
    // 녹색 정의
    init_color(COLOR_DARKGREEN, 0, 500, 0);
    // 어두운 노란색 정의
    init_color(COLOR_DARKYELLOW, 500, 500, 0);
    // 어두운 민트색깔 정의
    init_color(COLOR_DARKCYAN, 0, 500, 500);

    // pair에 맞는 배경과 텍스트 색을 정의한다.
    // init_pair(pair 번호, 텍스트 색깔, 배경 색깔)
    // (1) 기본 색상 기타 화면을 출력할 때 사용한다.
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    // (2) 벽(Wall, Immune Wall) 색상
    init_pair(2, COLOR_RED, COLOR_BLACK);
    // (3) 뱀 머리 색상
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    // (4) 뱀 몸통 색상
    init_pair(4, COLOR_DARKGREEN, COLOR_BLACK);
    // (5) 지렁이 Growth item 색상
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);
    // (6) 지렁이 Poison item 색상
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    // (7) 게이트 색상
    init_pair(7, COLOR_CYAN, COLOR_BLACK);
    // (8) 지렁이 Speed Down item 색상
    init_pair(8, COLOR_DARKYELLOW, COLOR_BLACK);
    // (9) 지렁이 Speed Up item 색상
    init_pair(9, COLOR_BLUE, COLOR_BLACK);
    // (10) 포탈 색상
    init_pair(10, COLOR_DARKCYAN, COLOR_BLACK);
}


// 2. 초기 화면 생성 함수(Game 화면 창, 점수 화면 창, 아이템 화면 창, 미션 화면 창, 클리어 시간 화면 창 생성)
// 윈도우 창 크기는 화면 비율을 맞추기 위해 높이는 2배, 너비는 3배로 비율을 맞추었다.
void screen_init(int height, int width) {
    // (1) 과정 ncurses 초기화
    initscr(); // curses 모드 시작 초기화
    start_color(); // 컬러 사용
    use_default_colors(); // 기본 색상을 사용한다.
    init_colors(); // 1번 함수를 통해 색상 초기화
    curs_set(0); // 커서 숨기기


    // (2) Game 화면 크기 설정 및 생성
    int screen_height = height * 2; // 게임 화면의 높이를 2배로 설정
    int screen_width = width * 3; // 게임 화면의 너비를 3배로 설정
    game_screen = newwin(screen_height, screen_width, 0, 0); // Game 화면 창 생성
    wbkgd(game_screen, COLOR_PAIR(1)); // Game 화면 창의 색깔을 1번 Pair로 설정
    box(game_screen, 0, 0); // 생성한 윈도우에 테두리를 추가한다.
    mvwprintw(game_screen, 1, screen_width / 2 - 5, "Snake Game"); // 생성한 윈도우에 창의 제목을 출력한다.
    refresh(); // 변경 사항을 반영한다
    wrefresh(game_screen); // 윈도우를 그려준다.


    // (3) 점수 화면 크기 설정 및 생성
    int score_height = 8; // 점수 화면의 높이를 8로 설정
    int score_width = 30; // 점수 화면의 너비를 30으로 설정
    int score_start_y = 3; // 점수 화면의 y축 위치 설정
    int score_start_x = WIDTH * 3 + 2; // 점수화면의 x축 위치 설정
    score_win = newwin(score_height, score_width, score_start_y, score_start_x); // 점수 화면 생성
    wbkgd(score_win, COLOR_PAIR(1)); // 점수 화면 창 색깔을 1번 Pair로 설정
    box(score_win, 0, 0); // 생성한 윈도우에 테두리를 추가한다.
    mvwprintw(score_win, 1, 1, "Score:"); // 생상한 윈도우에 창의 제목을 출력한다.
    refresh(); // 변경 사항을 반영한다.
    wrefresh(score_win); // 윈도우를 그려준다.


    // (4) 아이템 화면 크기 설정 및 생성
    int item_height = 8; // 아이템 화면의 높이를 8로 설정
    int item_width = 30; // 아이템 화면의 너비를 30으로 설정
    int item_start_y = score_start_y + score_height + 1; // 아이템 화면의 y축 위치 설정
    int item_start_x = WIDTH * 3 + 2; // 아이템 화면의 x축 위치 설정
    item_win = newwin(item_height, item_width, item_start_y, item_start_x); // 아이템 화면 생성
    wbkgd(item_win, COLOR_PAIR(1)); // 아이템 화면 창 색깔을 1번 Pair로 설정
    box(item_win, 0, 0); // 생성한 윈도우에 테두리를 추가한다.
    mvwprintw(item_win, 1, 1, "Items:"); // 생상한 윈도우에 창의 제목을 출력한다.


    // (5) item 창에서 아이템 색깔에 맞게 텍스트를 출력한다.
    // Growth item의 색상을 5번 Pair로 설정
    wattron(item_win, COLOR_PAIR(5)); // 텍스트 색상을 5번으로 설정
    mvwprintw(item_win, 2, 1, "+ : Growth Item"); // 텍스트 출력
    wattroff(item_win, COLOR_PAIR(5)); // 텍스트 색상 설정을 끈다.
    // Poison item의 색상을 6번 Pair로 설정
    wattron(item_win, COLOR_PAIR(6)); // 텍스트 색상을 6번으로 설정
    mvwprintw(item_win, 3, 1, "- : Poison Item"); // 텍스트 출력
    wattroff(item_win, COLOR_PAIR(6)); // 텍스트 색상 설정을 끈다.
    // Speed Down item의 색상을 8번 Pair로 설정
    wattron(item_win, COLOR_PAIR(8)); // 텍스트 색상을 8번으로 설정
    mvwprintw(item_win, 4, 1, "~ : Speed Down Item"); // 텍스트 출력
    wattroff(item_win, COLOR_PAIR(8)); // 텍스트 색상 설정을 끈다.
    // Speed Up item의 색상을 9번 Pair로 설정
    wattron(item_win, COLOR_PAIR(9)); // 텍스트 색상을 9번으로 설정
    mvwprintw(item_win, 5, 1, "^ : Speed Up Item"); // 텍스트 출력
    wattroff(item_win, COLOR_PAIR(9)); // 텍스트 색상 설정을 끈다.
    // Portal item의 색상을 10번 Pair로 설정
    wattron(item_win, COLOR_PAIR(10)); // 텍스트 색상을 10번으로 설정
    mvwprintw(item_win, 6, 1, "P : Portal"); // 텍스트 출력
    wattroff(item_win, COLOR_PAIR(10)); // 텍스트 색상 설정을 끈다.
    refresh(); // 변경 사항을 반영한다.
    wrefresh(item_win); // 윈도우를 그려준다.


    // (6) 미션 화면 크기 설정 및 생성
    int mission_height = 7; // 미션 화면의 높이를 7로 설정
    int mission_width = 30; // 미션 화면의 너비를 30으로 설정
    int mission_start_y = item_start_y + item_height + 1; // 미션 화면의 y축 위치 설정
    int mission_start_x = WIDTH * 3 + 2; // 미션 화면의 x축 위치 설정
    mission_win = newwin(mission_height, mission_width, mission_start_y, mission_start_x); // 미션 화면 생성
    wbkgd(mission_win, COLOR_PAIR(1)); // 미션 화면 창 색깔을 1번 Pair로 설정
    box(mission_win, 0, 0); // 생성한 윈도우에 테두리를 추가한다.
    mvwprintw(mission_win, 1, 1, "Mission:"); // 생상한 윈도우에 창의 제목을 출력한다.
    refresh(); // 변경 사항을 반영한다.
    wrefresh(mission_win); // 윈도우를 그려준다.


    // (7) 클리어 시간 화면 크기 설정 및 생성
    int clearTime_height = 7; // 클리어 시간 화면의 높이를 7로 설정
    int clearTime_width = 30; // 클리어 시간 화면의 너비를 30으로 설정
    int clearTime_start_y = mission_start_y + mission_height + 1; // 클리어 시간 화면의 y축 위치 설정
    int clearTime_start_x = WIDTH * 3 + 2; // 클리어 시간 화면의 x축 위치 설정
    clearTime_win = newwin(clearTime_height, clearTime_width, clearTime_start_y, clearTime_start_x); // 클리어 시간 화면 생성
    wbkgd(clearTime_win, COLOR_PAIR(1)); // 클리어 시간 화면 창 색깔을 1번 Pair로 설정
    box(clearTime_win, 0, 0); // 생성한 윈도우에 테두리를 추가한다.
    mvwprintw(clearTime_win, 1, 1, "Clear Times:"); // 생상한 윈도우에 창의 제목을 출력한다.
    refresh(); // 변경 사항을 반영한다.
    wrefresh(clearTime_win); // 윈도우를 그려준다.
}


// 3. 생성한 윈도우를 원하는 위치와 원하는 색깔로 변경 해주는 함수
void update_window(WINDOW* win, int y, int x, int color_pair) {
    wbkgd(win, COLOR_PAIR(color_pair)); // 해당 윈도우의 색상을 업데이트
    mvwin(win, y * 2, x * 3); // 윈도우를 새로운 위치로 이동한다.
    box(win, 0, 0); // 윈도우에 테두리를 추가해준다.
    wrefresh(win); // 변경된 내용을 윈도우에 반영하여 다시 그린다.
}


// 4. map 배열을 입력받아 벽(wall, immunewall) 윈도우를 추가하는 함수
void create_wall_windows(int map[HEIGHT][WIDTH]) {
    for (int i = 0; i < HEIGHT; ++i) { // map의 높이 만큼 반복
        for (int j = 0; j < WIDTH; ++j) { // map의 너비 만큼 반복
            if (map[i][j] == WALL || map[i][j] == IMMUNE_WALL) { // wall 혹은 immune wall일 경우 출력
                WINDOW* wall_win = newwin(2, 3, i * 2, j * 3); // 벽 윈도우 창을 새로 생성한다.
                wbkgd(wall_win, COLOR_PAIR(2)); // 벽 윈도우의 색깔을 2번 Pair로 설정
                box(wall_win, 0, 0); // 벽 윈도우에 테두리를 추가한다.
                wrefresh(wall_win); // 변경된 내용을 윈도우에 반영하여 다시 그린다.
                walls.push_back(wall_win); // 새로 생긴 윈도우 벽을 벽 윈도우 벡터인 walls에 추가한다.
            }
        }
    }
}


// 5. Snake 구조체를 입력받아 뱀 윈도우를 추가하는 함수
void create_snake_windows(const Snake& snake) {
    for (size_t i = 0; i < snake.body.size(); ++i) { // 뱀의 몸 길이 만큼 반복
        WINDOW* snake_win = newwin(2, 3, snake.body[i].first * 2, snake.body[i].second * 3); // 뱀의 윈도우 창을 새로 생성한다.
        if (i == 0) {
            wbkgd(snake_win, COLOR_PAIR(3)); // 뱀 머리의 색깔을 3번 Pair로 설정
        }
        else {
            wbkgd(snake_win, COLOR_PAIR(4)); // 뱀 몸통의 색깔을 4번 Pair로 설정
        }
        box(snake_win, 0, 0); // 뱀 윈도우에 테두리를 추가한다.
        wrefresh(snake_win); // 변경된 내용을 윈도우에 반영하여 다시 그린다.
        snake_positions.push_back({ snake.body[i].first, snake.body[i].second }); // 뱀위 위치를 저장하기 위해 벡터에 저장한다.
    }
}


// 6. map 배열을 입력받아 item 윈도우를 추가하는 함수
void create_item_windows(int map[HEIGHT][WIDTH]) {
    for (const auto& item : items) { // item 요소를 불러온다.
        WINDOW* item_win = newwin(2, 3, item.x * 2, item.y * 3); // 아이템의 윈도우 창을 새로 생성한다.
        int color_pair;

        // 아이템 종류에 따라 색깔을 설정한다.
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
        wbkgd(item_win, color_pair); // item 윈도우 색깔 설정
        box(item_win, 0, 0); // item 윈도우에 테두리를 추가한다.

        // 아이템 윈도우에 아이템을 구별하는 텍스트 추가
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
        wrefresh(item_win); // 변경된 내용을 윈도우에 반영하여 다시 그린다.
        item_windows.push_back(item_win); // 새로 생긴 아이템 윈도우를 아이템 윈도우 벡터인 item_windows에 추가한다.
    }
}


// 7. Gate 구조체를 입력받아 gate 윈도우를 추가하는 함수
void create_gate_windows(const Gate& gate) {
    if (gate.x1 != -1 && gate.y1 != -1 && gate.x2 != -1 && gate.y2 != -1) { // immunewall일 경우에 제외
        // 게이트 윈도우는 2개의 게이트가 있어야한다.(들어가는곳 나가는곳(교환 법칙 성립))
        WINDOW* gate_win1 = newwin(2, 3, gate.x1 * 2, gate.y1 * 3); // 게이트의 윈도우 창을 새로 생성한다.
        WINDOW* gate_win2 = newwin(2, 3, gate.x2 * 2, gate.y2 * 3); // 게이트의 윈도우 창을 새로 생성한다.
        wbkgd(gate_win1, COLOR_PAIR(7)); // 게이트 윈도우 색깔 설정
        wbkgd(gate_win2, COLOR_PAIR(7)); // 게이트 윈도우 색깔 설정
        box(gate_win1, 0, 0); // 게이트 윈도우에 테두리를 추가한다.
        box(gate_win2, 0, 0); // 게이트 윈도우에 테두리를 추가한다.
        wrefresh(gate_win1); // 변경된 내용을 윈도우에 반영하여 다시 그린다.
        wrefresh(gate_win2); // 변경된 내용을 윈도우에 반영하여 다시 그린다.
        walls.push_back(gate_win1); // 새로 생긴 게이트 윈도우를 walls 벡터에 추가한다.
        walls.push_back(gate_win2); // 새로 생긴 게이트 윈도우를 walls 벡터에 추가한다.
    }
}


// 8. 윈도우 벡터를 입력받아 윈도우를 삭제하는 함수
void delete_windows(std::vector<WINDOW*>& windows) {
    for (auto& win : windows) { // 벡터에 있는 각 윈도우에 대해 반복한다.
        delwin(win); // 윈도우를 삭제한다.
    }
    windows.clear(); // 벡터를 비워준다.
}


// 9. Snake 윈도우 벡터를 입력받아 윈도우를 삭제하는 함수
void delete_snake_windows(const std::vector<std::pair<int, int>>& positions) {
    for (const auto& pos : positions) { // Snake 위치 정보를 가지고 있는 윈도우 벡터에 대해 반복한다.
        WINDOW* win = newwin(2, 3, pos.first * 2, pos.second * 3); // 해당 위치에 대한 새로
        wbkgd(win, COLOR_PAIR(1));
        wrefresh(win);
        delwin(win);
    }
}


// 10. 현재 점수 및 미션을 출력 해주는 함수(점수판, 미션 화면을 지우고 다시 생성한다.)
void drawScore(const Snake& snake, int elapsed_time) {
    const Mission& mission = missions[currentStage];

    werase(score_win); // 점수판 윈도우 텍스트를 삭제한다.
    box(score_win, 0, 0); // 점수판 윈도우에 테두리를 추가한다.
    mvwprintw(score_win, 1, 1, "Score Board"); // 윈도우에 창의 제목을 출력한다.
    mvwprintw(score_win, 2, 1, "B: (%d) / (Max Length)", snake.body.size()); // 윈도우에 몸 길이 출력
    mvwprintw(score_win, 3, 1, "+: %d", growthItems); // 윈도우에 Growthitem 얻은 횟수 출력
    mvwprintw(score_win, 4, 1, "-: %d", poisonItems); // 윈도우에 Posionitem 얻은 횟수 출력
    mvwprintw(score_win, 5, 1, "G: %d", gateUses); // 윈도우에 Gate를 이용한 횟수 출력
    mvwprintw(score_win, 6, 1, "Time: %d sec", elapsed_time); // 해당 Stage 시간 출력
    wrefresh(score_win); // 변경된 내용을 윈도우에 반영하여 다시 그린다.

    // 미션 전광판 업데이트
    werase(mission_win); // 점수판 윈도우 텍스트를 삭제한다.
    box(mission_win, 0, 0); // 점수판 윈도우에 테두리를 추가한다.
    mvwprintw(mission_win, 1, 1, "Mission"); // 윈도우에 창의 제목을 출력한다.
    // 윈도우에 목표 몸 길이 출력
    mvwprintw(mission_win, 2, 1, "B: %d (%s)", mission.requiredBodyLength, snake.body.size() >= mission.requiredBodyLength ? "v" : " ");
    // 윈도우에 목표 Growthitem 횟수 출력
    mvwprintw(mission_win, 3, 1, "+: %d (%s)", mission.requiredGrowthItems, growthItems >= mission.requiredGrowthItems ? "v" : " ");
    // 윈도우에 목표 Posionitem 횟수 출력
    mvwprintw(mission_win, 4, 1, "-: %d (%s)", mission.requiredPoisonItems, poisonItems >= mission.requiredPoisonItems ? "v" : " ");
    // 윈도우에 목표 Gate를 이용 횟수 출력
    mvwprintw(mission_win, 5, 1, "G: %d (%s)", mission.requiredGateUses, gateUses >= mission.requiredGateUses ? "v" : " ");
    wrefresh(mission_win); // 변경된 내용을 윈도우에 반영하여 다시 그린다.
}


// 11. 이벤트 발생시 화면에 메시지를 3초 동안 출력해주는 함수(게임 종료, 다음 스테이지로 이동, 게임 클리어) 
void showMessage(const char* message) {
    int height, width;
    getmaxyx(stdscr, height, width); // 현재 터미널 윈도우에 최대 크기를 높이, 가로로 정한다.
    int msg_width = width / 2; // 메시지 가로 크기를 2분의 1로 설정
    WINDOW* msg_win = newwin(5, msg_width, height / 2 - 2, (width - msg_width) / 2); // 메시지를 화면 중앙에 위치하게 한다.

    for (int i = 3; i >= 1; --i) { // 3초 동안 반복
        werase(msg_win); // 메시지 창 내용 초기화
        box(msg_win, 0, 0); //윈도우에 테두리를 추가한다.
        mvwprintw(msg_win, 2, (msg_width - strlen(message)) / 2, "%s", message); // 메시지를 출력한다.
        mvwprintw(msg_win, 3, msg_width / 2 - 1, " %d ", i); // 몇 초 뒤에 다음 행동 넘어갈지를 출력한다.(3,2,1)
        wrefresh(msg_win); // 변경된 내용을 윈도우에 반영하여 다시 그린다.
        usleep(1000000); // 1초동안 대기
    }

    werase(msg_win); // 메시지 창 내용 지우기
    wrefresh(msg_win); // 화면 갱신
    delwin(msg_win); // 메시지 창 삭제
    refresh(); // 전체 화면 갱신
}