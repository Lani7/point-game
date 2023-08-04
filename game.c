#include "headers/game.h"
#include "util.c"

#define WIDTH 20
#define HEIGHT 16

// 방향키 27, 91, xx
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
  int playing = 1; // 게임진행중. true

  drawMap(&x, &y); // 맵 출력
  // 맵에 좌표 포인터를 전달

  while (playing)
  {
    // if (getchar() == ARROW && getchar() == ARROW2)
    // {
    // int key = getchar();
    switch (keyControl())
    {
    case UP:
      movePlayer(&x, &y, 0, -1);
      break;
    case DOWN:
      movePlayer(&x, &y, 0, 1);
      break;
    case LEFT:
      movePlayer(&x, &y, -1, 0);
      break;
    case RIGHT:
      movePlayer(&x, &y, 1, 0);
      break;

    case QUIT:
      playing = 0; // 게임 종료
      break;
    }
    refresh();
    // }

    // if (getchar() == 113)
    //   break;
  }
}

// map을 화면에 표시한다.
void drawMap(int *x, int *y)
{
  // 화면 말끔하게 한번 지우고 맵 그리기

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
      // printw("%c", map[h][w]);
    }
    printw("\n");
  }

  refresh();
}

// 플레이어를 이동시킨다.
// 원래좌표 (x, y), 증감할 좌표 (nx, xy)
void movePlayer(int *x, int *y, int nx, int ny)
{
  char mapObject = map[*y + ny][*x + nx];

  if (mapObject != '1')
  {
    gotoXY(*x, *y);
    printw(" ");

    gotoXY(*x + nx, *y + ny);
    printw("○");

    *x += nx;
    *y += ny;
  }

  // 실제 좌표값으로 변경
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