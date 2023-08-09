#include "headers/game.h"
#include "util.c"

#define WIDTH 20
#define HEIGHT 16
#define FOOD_CNT 18 // food의 개수
#define ENEMY_CNT 8 // 적의 개수
#define SPEED 100   // 적이 움직이는 속도
#define DISTANCE 1  // 적이 움직이는 거리

// 방향키 27, 91, xx (3개 필요)
#define LEFT 68
#define RIGHT 67
#define UP 65
#define DOWN 66
#define ARROW 27
#define ARROW2 91
#define QUIT 113 // q

// void moveEnemies(struct enemyMotion *);

clock_t oldTime;

char map[HEIGHT][WIDTH] = {
    {"11111111111111111111"},
    {"10000000000000000001"},
    {"10000000000000000001"},
    {"10000000000000000001"},
    {"10000000000000000001"},
    {"10000000000000000001"},
    {"10000000000000000001"},
    {"10000000000000000001"},
    {"10000000000000000001"},
    {"10000000000000000001"},
    {"10000000000000000001"},
    {"10000000000000000001"},
    {"10000000000000000001"},
    {"10000000000000000001"},
    {"1000000000p000000001"},
    {"11111111111111111111"}};

// enemy의 움직임을 위한 x, y좌표 구조체 선언
struct EnemyMotion
{
  int x;
  int y;
};

// 구조체 배열 선언
struct EnemyMotion enemyArr[ENEMY_CNT];
void moveEnemies(struct EnemyMotion *enemyArr);

void gameLoop()
{
  int x, y;        // 플레이어 좌표 저장 변수 (x, y)
  int point = 0;   // point 점수
  int playing = 1; // 게임진행중. true

  showFoodsEnemies(FOOD_CNT, 'f');  // 먹이의 좌표를 map에 생성
  showFoodsEnemies(ENEMY_CNT, 'e'); // 적의 좌표를 map에 생성
  drawMap(&x, &y);                  // map 출력. 좌표 포인터를 전달

  drawPointInfo(&point); // point 정보

  while (playing)
  {
    /* // todo: 구조체 배열을 하나씩 꺼내와서 각 구조체에 저장되어 있는
    // enemies의 x, y좌표들을 꺼내서 distance만큼 이동한다.*/
    moveEnemies(enemyArr);

    switch (keyControl())
    {
    case UP:
      movePlayer(&x, &y, 0, -1, &point);
      break;
    case DOWN:
      movePlayer(&x, &y, 0, 1, &point);
      break;
    case LEFT:
      movePlayer(&x, &y, -1, 0, &point);
      break;
    case RIGHT:
      movePlayer(&x, &y, 1, 0, &point);
      break;

    case QUIT:
      playing = 0; // 게임 종료
      break;
    }
    refresh();
  }
}

// enemy를 움직인다.
// void moveEnemies(int *x, int *y)
void moveEnemies(struct EnemyMotion *enemyArr)
{
  /* // todo: 구조체 배열을 하나씩 꺼내와서 각 구조체에 저장되어 있는
    // enemies의 x, y좌표들을 꺼내서 distance만큼 이동한다.*/
  int ex, ey;
  clock_t curTime = clock(); // 현재 시간
  // 현재시간에서 e배열 생성 후 저장된 시간을 뺀 차이가 speed(100ms)보다 클 때 움직인다.

  // printw("Time2: %lu", curTime);
  if (curTime - oldTime > SPEED) // 현재시간 기준으로 이전 시간으로부터 speed만큼 지났을 경우
  {
    for (int i = 0; i < ENEMY_CNT; i++)
    {
      // e의 배열 x좌표를 distance만큼 움직여준다
      ex = enemyArr[i].x;
      ey = enemyArr[i].y;

      // 기존 좌표에 있던 enemy를 0으로 바꾸고 거리(1)만큼 e로 바꾼다.
      map[ey][ex] = '0';
      map[ey][ex + DISTANCE] = 'e';

      // // enemy들의 움직임을 업데이트해서 표현한다.
      updateMove(&ex, &ey);
    }
    // drawMap(x, y);
  }
}

// enemy들의 움직임을 업데이트해서 표현한다.
void updateMove(int *ex, int *ey)
{
  // 원래 있던 e는 0으로 바뀌고,

  // x축으로 distance만큼 맵에서 이동한다.
  // 기존 좌표에 있던 enemy를 0으로 바꾸고 거리(1)만큼 e로 바꾼다.
  map[*ey][*ex] = '0';
  map[*ey][*ex + DISTANCE] = 'e';

  gotoXY(*ex, *ey);
  printw(" ");

  gotoXY(*ex + DISTANCE, *ey);
  printw("◇");

  // for (int i = 0; i < ENEMY_CNT; i++)
  // {
  //   map[*ey][*ex] = ' ';
  // }

  // int h, w;
  // for (int h = 0; h < HEIGHT; h++)
  // {
  //   for (int w = 0; w < WIDTH; w++)
  //   {
  //     char loc = map[h][w];
  //     if (loc == 'e')
  //     { // enemy
  //       printw("◇");
  //     }
  //   }
  //   printw("\n");
  // }
}

// 먹이와 적을 map에 표시한다.
void showFoodsEnemies(int cnt, char type)
{
  int fy, fx; // food 혹은 enemy의 y좌표, x좌표
  int i;

  // 랜덤 숫자의 x, y좌표를 구한다.
  for (i = 0; i < cnt; i++)
  {
    // 현재 시간을 seed값으로 부여해서 무작위 난수 출력하기
    srand((unsigned int)time(NULL) + rand()); // 난수 초기화
    fy = 2 + rand() % (HEIGHT - 2);           // 난수의 범위 2~15.

    srand((unsigned int)time(NULL) + rand()); // 난수 초기화
    fx = 2 + rand() % (WIDTH - 2);            // 난수의 범위 2~19.

    // 1, p 좌표는 제외한다.
    if (map[fy][fx] == '0')
    {
      // map[fy][fx] = (type == 'f') ? 'f' : 'e'; // 랜덤 숫자를 배열 map에 넣는다.
      if (type == 'f')
      {
        map[fy][fx] = 'f';
      }
      else
      {
        map[fy][fx] = 'e';
        // enemies의 랜덤 x, y좌표(fy, fx)를 구조체 배열에 저장한다.
        enemyArr[i].x = fx;
        enemyArr[i].y = fy;
      }

      continue;
    }
    else
    {
      i--;
      continue;
    }
  }

  if (type == 'e')
    oldTime = clock(); // 먹이 생성 후 현재 시간
  // printw("Time1: %lu", oldTime);
}

// map을 화면에 표시한다.
void drawMap(int *x, int *y)
{
  int h, w;
  for (int h = 0; h < HEIGHT; h++)
  {
    for (int w = 0; w < WIDTH; w++)
    {
      char loc = map[h][w];
      if (loc == '0') // 빈 공간(공백)
        printw(" ");
      else if (loc == '1') // 벽
        printw("■");
      else if (loc == 'p')
      { // 플레이어
        *x = w;
        *y = h;
        printw("○");
      }
      else if (loc == 'f')
      { // 먹이
        printw("●");
      }
      else if (loc == 'e')
      { // enemy
        printw("◇");
      }
    }
    printw("\n");
  }

  refresh();
}

// 포인트 정보를 보여준다.
void drawPointInfo(int *point)
{
  mvprintw(10, 30, "Point : %04d", *point);
  refresh();
}

// 플레이어를 이동시킨다.
// 원래좌표 (x, y), 증감할 좌표 (nx, xy)
void movePlayer(int *x, int *y, int nx, int ny, int *point)
{
  char mapObject = map[*y + ny][*x + nx];

  if (mapObject != '1')
  {
    gotoXY(*x, *y);
    printw(" ");

    gotoXY(*x + nx, *y + ny);
    printw("○");

    if (mapObject == 'f')
    {                              // 먹이
      map[*y + ny][*x + nx] = '0'; // map에서 'f'를 '0'으로 바꾼다.
      getPoint(point);
      drawPointInfo(point);
    }

    // 이동에 따른 x, y좌표 변경
    *x += nx;
    *y += ny;
  }
}

// 먹이(f)를 먹으면 point 증가
int getPoint(int *point)
{
  *point += 10;

  return *point;
}

// 키보드 입력값을 받는다.
int keyControl()
{
  char firstKey = getchar();
  if (firstKey == ARROW && getchar() == ARROW2)
  {
    char key = getchar();
    if (key == UP)
      return UP;
    else if (key == DOWN)
      return DOWN;
    else if (key == LEFT)
      return LEFT;
    else if (key == RIGHT)
      return RIGHT;
  }

  else if (firstKey == QUIT)
    return QUIT;

  return 0;
}