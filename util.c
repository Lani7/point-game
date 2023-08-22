#include "headers/util.h"

#define ALL_WHITE 1
#define ALL_BLACK 2
#define F_YELLOW 3
#define F_RED 4
#define F_CYAN 5

void init()
{
  setlocale(LC_ALL, ""); // 프로그램 전체 로케일을 프로세스의 기본 로케일을 사용
  WINDOW *w = initscr(); // initscr(): curses모드 시작. 화면 초기화
  start_color();         // 색상을 사용하기 위해 호출
  init_pair(ALL_WHITE, COLOR_WHITE, COLOR_WHITE);
  init_pair(ALL_BLACK, COLOR_BLACK, COLOR_BLACK);
  init_pair(F_YELLOW, COLOR_YELLOW, COLOR_BLACK);
  init_pair(F_RED, COLOR_RED, COLOR_BLACK);
  init_pair(F_CYAN, COLOR_CYAN, COLOR_BLACK);
  nodelay(w, TRUE);
  cbreak();
  noecho();    // getch()사용 시 사용자로부터 입력을 받은 문자를 출력하지 않는다.
  curs_set(0); // 커서 없애기. 0~2사이의 인자값을 가지고 0에 가까울수록 커서가 보이지 않는다.
}

void gotoXY(int x, int y)
{
  move(y, x); // 출력 위치 조절 (y좌표, x좌표)순으로 입력받음.
}

// 렌더링되기까지 기다린다.
// void waitrender(clock_t oldTime, int speed)
// {
//   clock_t curTime;
//   while (1)
//   {
//     curTime = clock();
//     if (curTime - oldTime > speed)
//       break;
//   }
// }

// sleep
void sleep(int ms)
{
  clock_t start = clock();
  ms--;
  while (1)
  {
    if ((clock() - start) > ms)
      break;
  }
}
