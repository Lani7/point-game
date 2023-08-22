#include <stdio.h>
#include <locale.h>  // setlocale()
#include <stdlib.h>  // rand(), srand()
#include <time.h>    // time(), clock()
#include <stdbool.h> // bool
#include <string.h>  // strcpy(), strlen()

#include "headers/main.h"
#include "game.c"

int main()
{
  init(); // 초기 설정

  bool flag = true;
  int stage, point; // 난이도 단계, 포인트
  while (flag)
  {
    gotoXY(5, 3);
    titlePrint();

    int key = selectMenu(); // 메뉴 선택
    clear();                // 화면 초기화
    switch (key)
    {
    case 1:
      // 사용자 입력
      if (inputUser() != 1)
        continue;
      do
      {
        stage = 1;                // 1단계부터 시작
        point = 0;                // point 점수
        gameLoop(&stage, &point); // 게임 시작
        printResult();            // 게임 결과 (닉네임과 점수, 랭킹기록)
      } while (askAgain());
      break;
    case 2:
      explainRules(); // 게임 설명
      break;
    case 3:
      flag = false;
      break;
    }
    clear(); // 화면 초기화
  }

  endwin(); // curses모드 종료

  return 0;
}
