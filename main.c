#include <stdio.h>
#include <locale.h> // setlocale()
#include <stdlib.h> //rand(), srand()
#include <time.h>   //time(), clock()

#include "headers/main.h"
#include "game.c"
// #include "util.c"

int main()
{
  setlocale(LC_ALL, ""); // 프로그램 전체 로케일을 프로세스의 기본 로케일을 사용
  // initscr();             // curses모드 시작. 화면 초기화
  WINDOW *w = initscr();
  nodelay(w, TRUE);

  curs_set(0); // 커서 없애기. 0~2사이의 인자값을 가지고 0에 가까울수록 커서가 보이지 않는다.

  // gotoXY(5, 5);
  // 콘솔 창
  // titlePrint();

  gameLoop();

  // int key;
  // while (1)
  // {
  //   key = getchar();
  //   printw("%d\n", key);
  //   refresh();
  // }

  // addstr("\n press any key to exit...");
  // refresh();
  // getch();

  endwin(); // curses모드 종료.

  return 0;
}
