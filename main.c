#include <stdio.h>
#include <locale.h>  // setlocale()
#include <stdlib.h>  //rand(), srand()
#include <time.h>    //time(), clock()
#include <stdbool.h> // bool

#include "headers/main.h"
#include "game.c"
// #include "util.c"

int main()
{
  init(); // 초기화

  bool flag = true;
  int stage; // 난이도 단계
  while (flag)
  {
    gotoXY(5, 3);
    titlePrint();

    // 메뉴 선택하기
    int key = selectMenu();
    clear(); // 화면 초기화
    switch (key)
    {
    case 1:
      // todo: 사용자 입력 받기
      stage = 1; // 1단계부터 시작
      gameLoop(&stage);
      // todo: 본인의 이름과 점수, 랭킹 5 기록 보여주기
      break;
    case 2:
      explainRules();
      break;
    case 3:
      flag = false;
      break;
    }
    clear(); // 화면 초기화
  }

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
