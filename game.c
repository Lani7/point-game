#include "headers/game.h"
#include "util.c"

#define WIDTH 20
#define HEIGHT 16
#define FOOD_CNT 18 // food의 개수

// 방향키 27, 91, xx (3개 필요)
#define LEFT 68
#define RIGHT 67
#define UP 65
#define DOWN 66
#define ARROW 27
#define ARROW2 91
#define QUIT 113 // q

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

void gameLoop()
{
  int x, y;        // 플레이어 좌표 저장 변수 (x, y)
  int point = 0;   // point 점수
  int playing = 1; // 게임진행중. true

  showFoodsEnemies(); // 먹이와 적의 좌표를 map에 생성
  drawMap(&x, &y);    // map 출력. 좌표 포인터를 전달

  drawPointInfo(&point); // point 정보

  while (playing)
  {
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

// 먹이와 적을 map에 표시한다.
void showFoodsEnemies()
{
  int fy, fx; // food의 y좌표, x좌표
  int i;
  // 랜덤 숫자의 x, y좌표를 구한다.
  for (i = 0; i < FOOD_CNT; i++)
  {
    // 현재 시간을 seed값으로 부여해서 무작위 난수 출력하기
    srand((unsigned int)time(NULL) + rand()); // 난수 초기화
    fy = 2 + rand() % (HEIGHT - 2);           // 난수의 범위 2~15.

    srand((unsigned int)time(NULL) + rand()); // 난수 초기화
    fx = 2 + rand() % (WIDTH - 2);            // 난수의 범위 2~19.

    // 1, p 좌표는 제외한다.
    if (map[fy][fx] == '0')
    {
      map[fy][fx] = 'f'; // 랜덤 숫자를 배열 map에 넣는다.
      continue;
    }
    else
    {
      i--;
      continue;
    }
  }
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
      {
        // 먹이
        printw("●");
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