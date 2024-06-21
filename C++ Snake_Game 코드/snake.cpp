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

// 맵 초기화 함수
void initMap(int map[HEIGHT][WIDTH], int stageIndex) {
    // 주어진 스테이지 배열을 현재 맵 배열에 복사
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            map[i][j] = stage[stageIndex][i][j];
        }
    }
}

// 뱀 초기화 함수
void initSnake(Snake& snake) {
    //초기 위치는 맵의 가운데
    snake.x = HEIGHT / 2;
    snake.y = WIDTH / 2;
    snake.dir = RIGHT;
    snake.body = { {snake.x, snake.y}, {snake.x, snake.y - 1}, {snake.x, snake.y - 2} };
}

// 뱀 이동 처리 함수
void moveSnake(int map[HEIGHT][WIDTH], Snake& snake) {
    passingGate = false;

    //설정 방향에 따라 이동 좌표 설정
    int dx = 0, dy = 0;
    switch (snake.dir) {
    case UP:    dx = -1; break;
    case DOWN:  dx = 1; break;
    case LEFT:  dy = -1; break;
    case RIGHT: dy = 1; break;
    }
    //새로운 좌표로 이동
    int newX = snake.body.front().first + dx;
    int newY = snake.body.front().second + dy;

    // 벽이나 자기 자신과의 충돌 처리
    if (map[newX][newY] == WALL || map[newX][newY] == IMMUNE_WALL ||
        std::find(snake.body.begin(), snake.body.end(), std::make_pair(newX, newY)) != snake.body.end()) {
        // 충돌 시 게임 오버 처리
        showMessage("Game Over! You collided with an obstacle or yourself.");
        endwin();
        exit(0);
    }

    // 게이트 이동 처리
    if (map[newX][newY] == GATE) {
        passingGate = true;
        //출구 게이트로 이동 좌표 수정
        if (newX == gate.x1 && newY == gate.y1) {
            newX = gate.x2;
            newY = gate.y2;
        }
        else {
            newX = gate.x1;
            newY = gate.y1;
        }
        //진출방향 결정
        adjustGateExit(map, newX, newY, snake.dir);
        gateUses++;
    }
    else {
        passingGate = false;
    }

    // 포탈 이동 처리(다음스테이지 넘어갈때)
    if (map[newX][newY] == STAGE_PORTAL) {
        time_t endTime = time(NULL);
        clearTimes[currentStage] = difftime(endTime, stageStartTime);

        currentStage++;
        //마지막 스테이지 클리어 시
        if (currentStage >= 4) {
            // 총 클리어 시간 계산
            int totalClearTime = 0;
            for (int i = 0; i < 4; ++i) {
                totalClearTime += clearTimes[i];
            }
            // 총 클리어 시간 표시
            char totalTimeMessage[100];
            snprintf(totalTimeMessage, sizeof(totalTimeMessage), "Congratulations! You completed all stages.\nTotal clear time: %d sec", totalClearTime);
            showMessage(totalTimeMessage);

            endwin();
            exit(0);
        }

        // 안내문 표시
        showMessage("Proceeding to the next stage...");

        //창 초기화 작업
        items.clear();
        delete_windows(walls);
        delete_windows(item_windows);
        snake_positions.clear();
        delwin(game_screen);
        delwin(score_win);
        delwin(item_win);
        delwin(mission_win);
        delwin(clearTime_win);

        // 이전 게이트 제거
        if (gate.x1 != -1 && gate.y1 != -1 && gate.x2 != -1 && gate.y2 != -1) {
            map[gate.x1][gate.y1] = WALL;
            map[gate.x2][gate.y2] = WALL;
            gate = { -1, -1, -1, -1, 0 }; // 게이트 초기화
        }

        initMap(map, currentStage);
        initSnake(snake);

        // 미션 관련 변수 초기화
        growthItems = 0;
        poisonItems = 0;
        speeddownItems = 0;
        speedupItems = 0;
        gateUses = 0;
        portalGenerated = false; // 포탈 생성 여부 초기화

        // 이동 속도 초기화
        tick_duration = 300000; // 기본 이동 속도 초기화

        // 아이템 및 게이트 재생성
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

        // 화면 초기화
        screen_init(HEIGHT, WIDTH);
        nodelay(game_screen, TRUE);
        keypad(game_screen, TRUE);

        // 미션 전광판 초기화
        werase(mission_win);
        box(mission_win, 0, 0);
        mvwprintw(mission_win, 1, 1, "Mission");
        mvwprintw(mission_win, 2, 1, "B: 5 ( )");
        mvwprintw(mission_win, 3, 1, "+: 2 ( )");
        mvwprintw(mission_win, 4, 1, "-: 2 ( )");
        mvwprintw(mission_win, 5, 1, "G: 1 ( )");
        wrefresh(mission_win);

        // 클리어 시간 창 초기화 및 표시
        werase(clearTime_win);
        box(clearTime_win, 0, 0);
        mvwprintw(clearTime_win, 1, 1, "Clear Times:");
        for (int i = 0; i < currentStage; ++i) {  // 이전 스테이지까지만 표시
            mvwprintw(clearTime_win, 2 + i, 1, "Stage %d: %d sec", i + 1, clearTimes[i]);
        }
        wrefresh(clearTime_win);

        // 새 스테이지 요소 생성 및 화면 갱신
        create_wall_windows(map);
        create_item_windows(map);
        create_gate_windows(gate);
        create_snake_windows(snake);
        drawScore(snake, 0);

        stageStartTime = time(NULL); // 새로운 스테이지 시작 시간 기록

        return;
    }

    // 아이템 처리
    //뱀 길이 증가 아이템
    if (map[newX][newY] == GROWTH_ITEM) {
        score++;    //점수 증가
        growthItems++;  //아이템 섭취 개수 증가
        //사이즈 증가를 위한 임시 몸통 벡터 추가
        snake.body.push_back({ -1, -1 });
        map[newX][newY] = 0;    //일반 필드로 변경
        //아이템 벡터에서 삭제
        items.erase(std::remove_if(items.begin(), items.end(), [&](const Item& item) {
            return item.x == newX && item.y == newY && item.type == GROWTH_ITEM;
            }), items.end());
    }
    //뱀 길이 감소 아이템
    else if (map[newX][newY] == POISON_ITEM) {
        score--;    //점수 감소
        poisonItems++;  //아이템 섭취 개수 증가
        map[newX][newY] = 0;    //일반 필드로 변경
        //아이템 벡터에서 삭제
        items.erase(std::remove_if(items.begin(), items.end(), [&](const Item& item) {
            return item.x == newX && item.y == newY && item.type == POISON_ITEM;
            }), items.end());
        snake.body.pop_back();
        //길이가 3보다 줄어들면 게임오버
        if (snake.body.size() < 3) {
            showMessage("Game Over! The snake has no body left.");
            endwin();
            exit(0);
        }
    }
    else if (map[newX][newY] == SPEEDDOWN_ITEM) {
        score--;    //점수 감소
        speeddownItems++; // 아이템 섭취 개수 증가
        map[newX][newY] = 0;    //일반 필드로 변경
        //아이템 벡터에서 삭제
        items.erase(std::remove_if(items.begin(), items.end(), [&](const Item& item) {
            return item.x == newX && item.y == newY && item.type == SPEEDDOWN_ITEM;
            }), items.end());
        tick_duration *= 2; // 이동 속도 두 배 감소
    }
    else if (map[newX][newY] == SPEEDUP_ITEM) {
        score++;    //점수 증가
        speedupItems++; // 아이템 섭취 개수 증가
        map[newX][newY] = 0;    //일반 필드로 변경
        //아이템 벡터에서 삭제
        items.erase(std::remove_if(items.begin(), items.end(), [&](const Item& item) {
            return item.x == newX && item.y == newY && item.type == SPEEDUP_ITEM;
            }), items.end());
        tick_duration /= 2; // 이동 속도 두 배 증가
    }

    // 뱀 이동 업데이트
    std::pair<int, int> prev = snake.body.front();
    //새로운 좌표로 헤드생성 
    snake.body.front() = { newX, newY };
    //몸통 한 칸씩 앞으로 이동
    for (size_t i = 1; i < snake.body.size(); ++i) {
        std::pair<int, int> temp = snake.body[i];
        snake.body[i] = prev;
        prev = temp;
    }

    // 스테이지에 따른 게이트 생성 조건 체크 및 생성
    if (!passingGate &&
        ((currentStage == 0 && snake.body.size() >= 5) ||
            (currentStage == 1 && snake.body.size() >= 6) ||
            (currentStage == 2 && snake.body.size() >= 7) ||
            (currentStage == 3 && snake.body.size() >= 8))) {
        if (difftime(time(NULL), gate.spawnTime) >= GATE_DURATION) {
            generateGates(map);
        }
    }

    // 화면 업데이트
    delete_snake_windows(snake_positions);
    snake_positions.clear();
    create_snake_windows(snake);
}

// 미션 완료 체크 함수
bool checkMissionComplete(const Snake& snake) {
    const Mission& mission = missions[currentStage];
    return (snake.body.size() >= mission.requiredBodyLength &&
        growthItems >= mission.requiredGrowthItems &&
        poisonItems >= mission.requiredPoisonItems &&
        gateUses >= mission.requiredGateUses);
}