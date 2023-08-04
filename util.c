#include "headers/util.h"

void titlePrint()
{
  printw("\n\n\n\n\n"); // curses 모드에서는 printf() 불가
  printw("Welcome\n");
  printw("to the game.\n");
  printw("\n\n\n\n\n");
  refresh(); // 화면의 내용을 갱신. 함수를 호출해야 해당 내용이 보인다.
}

void gotoXY(int x, int y)
{
  move(y, x); // 출력 위치 조절 (y좌표, x좌표)순으로 입력받음.
}
