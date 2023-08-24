#include "headers/game.h"
#include "util.c"

#define WIDTH 20
#define HEIGHT 16
#define DISTANCE 1        // 적이 움직이는 거리
#define INIT_SPEED 100000 // 적의 속도
#define FOOD_CNT 10       // food의 개수
#define INIT_ENEMY_CNT 4  // 적의 개수
#define MAX_ENEMY_CNT 16  // 최대 적의 개수

#define NAME_LIMIT 20 // 닉네임 글자수 제한
#define RANK_CNT 5    // 랭킹 수

// 방향키 27, 91, xx (3개 필요)
#define LEFT 68
#define RIGHT 67
#define UP 65
#define DOWN 66
#define ARROW 27
#define ARROW2 91
#define SPACE 32
#define QUIT 113      // q
#define ENTER 13      // enter
#define Y 121         // y
#define N 110         // n
#define BACKSPACE 127 // 백스페이스

// color
#define ALL_WHITE 1
#define ALL_BLACK 2
#define F_YELLOW 3
#define F_RED 4
#define F_CYAN 5

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

int speed, fcnt, userCnt, enemyCnt;
// 적이 움직이는 속도, food 먹이의 개수, 사용자수, 장애물 개수

clock_t oldTime;

// 사용자 정보
struct User
{
  char name[20]; // 이름
  int score;     // 점수
};

struct User userInfo; // 구조체 선언

// 랭킹 기록 구조체 배열 선언
struct User ranking[RANK_CNT];

// enemy의 움직임을 위한 x, y좌표 구조체 선언
struct EnemyMotion
{
  int x;          // x좌표
  int y;          // y좌표
  char direction; // 방향 (l : 왼쪽, r : 오른쪽)
};

// 구조체 배열 포인터 선언
struct EnemyMotion *enemyArr;

// 구조체를 파라미터로 받는 함수들
void moveEnemies(struct EnemyMotion *enemyArr);
void printRank(struct User *ranking);
void sortRank(struct User *rankingptr);
int getridx(struct User *rankingptr);

void gameLoop(int *stage, int *point)
{
  clear();
  int x, y; // 플레이어 좌표 저장 변수 (x, y)

  fcnt = FOOD_CNT; // food 먹이의 개수
  int playing = 1; // 게임진행중. true

  gotoXY(5, 3);
  printw("stage %d start!", *stage);
  refresh();
  sleep(1000000);
  clear();

  // 맵 초기화
  initMap();

  // stage단계별 enemies의 속도 설정
  setSpeed(stage, &speed);
  setEnemyCnt(stage, &enemyCnt);

  showFoodsEnemies(FOOD_CNT, 'f'); // 먹이의 좌표를 map에 생성
  showFoodsEnemies(enemyCnt, 'e'); // 적의 좌표를 map에 생성
  drawMap(&x, &y);                 // map 출력. 좌표 포인터를 전달

  drawPointInfo(point); // point 정보

  while (playing)
  {
    switch (keyControl())
    {
    case UP:
      movePlayer(&x, &y, 0, -1, point);
      break;
    case DOWN:
      movePlayer(&x, &y, 0, 1, point);
      break;
    case LEFT:
      movePlayer(&x, &y, -1, 0, point);
      break;
    case RIGHT:
      movePlayer(&x, &y, 1, 0, point);
      break;

    case QUIT:           // 나가기
      playing = 0;       // 게임 종료
      saveScore(point);  // score 기록 저장
      sortRank(ranking); // rank에 점수 순대로 정렬
      break;
    }

    moveEnemies(enemyArr);

    // waitrender(oldTime, speed);

    refresh();

    // 만약 플레이어와 enemy가 충돌하면 게임종료된다.
    if (crash(&x, &y))
    {
      saveScore(point);  // score 기록 저장
      sortRank(ranking); // rank에 점수 순대로 정렬
      // 게임 종료 후 게임 오버 화면.
      drawGameOver();
      sleep(2000000);
      playing = 0;
      *point = 0; // 포인트 초기화
      free(enemyArr);
    }

    // 먹이를 다 먹으면 clear되고, stage가 올라가면서 다시 게임이 시작된다.
    if (fcnt == 0)
    {
      clear();
      free(enemyArr);
      clearStage(stage, point);
      break;
    }
  }
}

// food를 다 먹으면 stage를 clear한다.
int clearStage(int *stage, int *point)
{
  printw("stage %d clear!\n", *stage);
  printw("다음 stage로 진행하겠습니까?\n");
  printw("(진행: y, 나가기: n)\n");
  refresh();
  while (1)
  {
    int key = keyControl();
    if (key == N)
    {                    // 나가기
      saveScore(point);  // score 저장
      sortRank(ranking); // rank에 점수 순대로 정렬
      return 0;
    }
    else if (key == Y)
    {
      *stage += 1; // 단계 상승
      gameLoop(stage, point);
      return 1;
    }
  }
}

// enemy를 움직인다.
void moveEnemies(struct EnemyMotion *enemyArr)
{
  int ex, ey, edrt;
  clock_t curTime = clock();

  // 현재시간에서 e배열 생성 후 저장된 시간을 뺀 차이가 speed(100ms)보다 클 때 움직인다.
  if (curTime - oldTime < speed)
    return;

  // 현재시간 기준으로 이전 시간으로부터 speed만큼 지났을 경우 실행
  for (int i = 0; i < enemyCnt; i++)
  {
    ex = enemyArr[i].x;
    ey = enemyArr[i].y;

    // sleep(speed); // stage단계별 speed 속도로 움직임 조절

    // 지나갈 좌표에 먹이가 있다면, 그 좌표에 다시 먹이를 출력해야 한다.
    if (map[ey][ex] == 'f')
    { // 현재 좌표에 저장되어 있는 게 f라면 먹이를 출력한다.
      gotoXY(ex, ey);
      attron(COLOR_PAIR(F_YELLOW));
      printw("●");
      attroff(COLOR_PAIR(F_YELLOW));
    }
    else
    { // 현재 좌표의 e를 없어지게 한다.
      gotoXY(ex, ey);
      attron(COLOR_PAIR(ALL_BLACK));
      printw(" ");
      attroff(COLOR_PAIR(ALL_BLACK));
      map[ey][ex] = '0';
    }

    // 왼쪽벽에 도달할 경우 방향을 오른쪽으로 바꿔준다.
    if (map[ey][ex - DISTANCE] == '1')
    {
      enemyArr[i].direction = 'r'; // right
    }
    // 오른쪽벽에 도달할 경우 방향을 왼쪽으로 바꿔준다.
    else if (map[ey][ex + DISTANCE] == '1')
    {
      enemyArr[i].direction = 'l'; // left
    }

    edrt = enemyArr[i].direction; // 진행 방향

    if (edrt == 'r')
    {
      // 다음 +1 x좌표에 e를 출력한다.
      gotoXY(ex + DISTANCE, ey);
      attron(COLOR_PAIR(F_RED));
      printw("◆");
      attroff(COLOR_PAIR(F_RED));

      // 지나갈 좌표에 먹이가 있다면, 그 좌표에 다시 먹이를 출력해야 한다.
      if (map[ey][ex + DISTANCE] == 'f')
      {
        map[ey][ex + DISTANCE] = 'f';
      }
      else
      {
        map[ey][ex + DISTANCE] = 'e'; // 먹이가 없다면 enemy를 표시
      }

      // e의 배열 x좌표를 distance만큼 움직여준다
      enemyArr[i].x += DISTANCE;
    }
    // 오른쪽 벽에 도달했을 경우 방향을 l(왼쪽)으로 바꿔준다.
    else if (edrt == 'l')
    {
      gotoXY(ex - DISTANCE, ey);
      attron(COLOR_PAIR(F_RED));
      printw("◆");
      attroff(COLOR_PAIR(F_RED));

      // 지나갈 좌표에 먹이가 있다면, 그 좌표에 다시 먹이를 출력해야 한다.
      if (map[ey][ex - DISTANCE] == 'f')
      {
        map[ey][ex - DISTANCE] = 'f';
      }
      else
      {
        map[ey][ex - DISTANCE] = 'e';
      }

      // e의 배열 x좌표를 distance만큼 움직여준다
      enemyArr[i].x -= DISTANCE;
    }

    refresh();
  }
  oldTime = curTime;
}

// 먹이와 적을 map에 표시한다.
void showFoodsEnemies(int cnt, char type)
{
  int fy, fx; // food 혹은 enemy의 y좌표, x좌표
  int i;

  // 장애물 배열의 동적 할당
  if (type == 'e')
  {
    enemyArr = (struct EnemyMotion *)malloc(sizeof(struct EnemyMotion) * enemyCnt);
  }

  // 랜덤 숫자의 x, y좌표를 구한다.
  for (i = 0; i < cnt; i++)
  {
    // 현재 시간을 seed값으로 부여해서 무작위 난수 출력하기
    srand((unsigned int)time(NULL) + rand()); // 난수 초기화
    fy = 1 + rand() % (HEIGHT - 3);           // 난수의 범위 1~13. (15행은 플레이어 초기 위치이므로 제외)

    srand((unsigned int)time(NULL) + rand()); // 난수 초기화
    fx = 1 + rand() % (WIDTH - 1);            // 난수의 범위 1~19.

    // 1, p 좌표는 제외한다.
    if (map[fy][fx] == '0')
    {
      // 랜덤 숫자를 배열 map에 넣는다.
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
        enemyArr[i].direction = 'r';
      }
    }
    else
    {
      i--; // 0이 아닌 것들과 중복될 경우 반복을 다시 실행한다.
    }
  }
  oldTime = clock(); // 시간 초기화
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
      switch (loc)
      {
      case '0': // 빈 공간(공백)
        attron(COLOR_PAIR(ALL_BLACK));
        printw(" ");
        attroff(COLOR_PAIR(ALL_BLACK));
        break;
      case '1': // 벽
        attron(COLOR_PAIR(ALL_WHITE));
        printw(" ");
        attroff(COLOR_PAIR(ALL_WHITE));
        break;
      case 'p': // 플레이어
        *x = w;
        *y = h;
        attron(COLOR_PAIR(F_CYAN));
        printw("○");
        attroff(COLOR_PAIR(F_CYAN));
        break;
      case 'f': // 먹이
        attron(COLOR_PAIR(F_YELLOW));
        printw("●");
        attroff(COLOR_PAIR(F_YELLOW));
        break;
      case 'e': // enemy
        attron(COLOR_PAIR(F_RED));
        printw("◆");
        attroff(COLOR_PAIR(F_RED));
        break;
      }
    }
    printw("\n");
  }

  refresh();
}

// 포인트 정보와 q:나가기를 보여준다.
void drawPointInfo(int *point)
{
  mvprintw(10, 30, "Point : %04d \n", *point);
  mvprintw(1, 30, "(q : 나가기)");
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
    attron(COLOR_PAIR(ALL_BLACK));
    printw(" "); // 빈 공간
    attroff(COLOR_PAIR(ALL_BLACK));

    gotoXY(*x + nx, *y + ny);
    attron(COLOR_PAIR(F_CYAN));
    printw("○"); // 플레이어
    attroff(COLOR_PAIR(F_CYAN));

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

// 플레이어와 enemy가 만나면 게임종료된다.(playing = 0)
bool crash(int *x, int *y)
{
  // 플레이어 좌표가 e이면 true
  if (map[*y][*x] == 'e')
    return true;
  else
    return false;
}

// 먹이(f)를 먹으면 point 증가
int getPoint(int *point)
{
  *point += 10;
  fcnt--;
  return *point;
}

// stage 단계별로 enemies의 속도를 설정한다.
void setSpeed(int *stage, int *speed)
{
  // stage증가시마다 속도가 빨라진다.
  if (*stage == 1)
    *speed = INIT_SPEED; // 초기 속도 설정

  else if (*speed >= 4000) // 최고 속도를 4000으로 제한한다
  {
    *speed -= 2000;
  }
}

// stage 단계별로 enemies의 개수를 설정한다.
void setEnemyCnt(int *stage, int *enemyCnt)
{
  // stage증가시마다 enemies의 개수가 많아진다.
  if (*stage == 1)
    *enemyCnt = INIT_ENEMY_CNT; // 초기 개수 설정

  else if (*enemyCnt <= MAX_ENEMY_CNT) // 최고 개수를 제한한다
  {
    *enemyCnt += 2;
  }
}

// 키보드 입력값을 받는다.
int keyControl()
{
  char firstKey = getch();
  if (firstKey == ARROW && getch() == ARROW2)
  {
    char key = getch();
    if (key == UP)
      return UP;
    else if (key == DOWN)
      return DOWN;
    else if (key == LEFT)
      return LEFT;
    else if (key == RIGHT)
      return RIGHT;
  }
  else if (firstKey == SPACE)
    return SPACE;
  else if (firstKey == '\n')
    return ENTER;
  else if (firstKey == QUIT)
    return QUIT;
  else if (firstKey == Y)
    return Y;
  else if (firstKey == N)
    return N;
  else if (firstKey == BACKSPACE)
    return BACKSPACE;
  return 0;
}

// 게임 오버 화면을 출력한다.
void drawGameOver()
{
  clear();
  gotoXY(5, 3);
  printw("GAME OVER!");
  refresh();
}

// 맵을 초기화한다.
void initMap()
{
  int h, w;
  for (h = 0; h < HEIGHT; h++)
  {
    for (w = 0; w < WIDTH; w++)
    {
      map[h][w] = (h == 0 || h == HEIGHT - 1) ? '1' : '0'; // 위아래 벽
      if (w == 0 || w == WIDTH - 1)
        map[h][w] = '1'; // 사이드 벽
    }
  }
  // 플레이어 초기 위치
  map[HEIGHT - 2][WIDTH / 2] = 'p';
}

// 메뉴를 선택한다.
int selectMenu()
{
  gotoXY(5, 6);
  printMenu();

  int key = getchar() - '0';
  switch (key)
  {
  case 1:
    return 1;
  case 2:
    return 2;
  case 3:
    return 3;
  }

  return 0;
}

// 메뉴를 출력한다.
void printMenu()
{
  mvprintw(10, 8, "Menu (키보드로 번호를 입력하세요.) \n");
  mvprintw(12, 8, "1. 게임 시작\n");
  mvprintw(13, 8, "2. 게임 설명\n");
  mvprintw(14, 8, "3. 종료\n");
  refresh();
}

// 게임 시작 타이틀을 표시한다.
void titlePrint()
{
  mvprintw(2, 8, " ------------------------- \n");
  mvprintw(3, 8, "|                         |");
  mvprintw(4, 8, "|         WELCOME         |\n"); // curses 모드에서는 printf() 불가
  mvprintw(5, 8, "|    TO THE POINT GAME.   |\n");
  mvprintw(6, 8, "|                         |");
  mvprintw(7, 8, " ------------------------- \n");
  refresh(); // 화면의 내용을 갱신. 함수를 호출해야 해당 내용이 보인다.
}

// 게임 규칙을 설명한다.
void explainRules()
{
  printw("< 게임 설명 >\n\n");
  printw("1. 키보드 화살키로 플레이어 ");
  attron(COLOR_PAIR(F_CYAN));
  printw("○");
  attroff(COLOR_PAIR(F_CYAN));
  printw("를 상하좌우로 움직일 수 있습니다.\n");

  printw("2. 먹이 ");
  attron(COLOR_PAIR(F_YELLOW));
  printw("●");
  attroff(COLOR_PAIR(F_YELLOW));
  printw("를 먹으면 포인트를 얻습니다.\n");

  printw("3. 장애물 ");
  attron(COLOR_PAIR(F_RED));
  printw("◆");
  attroff(COLOR_PAIR(F_RED));
  printw("를 만나면 게임이 종료됩니다.\n");
  printw("\n메뉴로 돌아가기 : 엔터를 누르세요.");
  refresh();

  while (1)
  {
    if (keyControl() == ENTER)
    {
      clear();
      break;
    }
  }
}

// 사용자 정보를 입력받는다.
int inputUser()
{
  int x, y, key, i;
  bool flag;
  char *ptr = userInfo.name;

  // printw("사용자의 닉네임을 작성하신 후 엔터를 입력해주세요. (20자 미만)\n");
  // printw("닉네임 : ");
  // refresh();
  // getch(); // 버퍼에 남아있는 문자 비우기
  // echo();
  // scanf("%s", userInfo.name);

  do
  {
    *ptr = 0; // 문자열 배열 초기화(null) // *ptr = '\0';
    x = 0;
    y = 1;
    gotoXY(x, y);
    printw("사용자의 닉네임을 작성하신 후 엔터를 입력해주세요.\n");
    printw("닉네임은 공백없이 영문과 숫자로만 가능합니다. (20자 미만)\n");
    printw("닉네임 : ");

    x = 8;
    y = 3;
    gotoXY(x, y);
    getch(); // 버퍼에 남아있는 문자 비우기
    // 입력한 문자를 userInfo.name 배열에 저장한다.
    i = 0;
    ///////// 이중 반복문 시작
    while (i < NAME_LIMIT - 1)
    {
      key = getchar();
      // 닉네임은  영문 또는 숫자만 가능
      // if (key < '0' || (key > '9' && key < 'A') || (key > 'Z' && key < 'a') || key > 'z')
      if ((key < 48 || (key > 57 && key < 65) || (key > 90 && key < 97) || key > 122) && key != BACKSPACE && key != ENTER)
      {
        continue;
      }
      // 백 스페이스를 누르면 바로 전의 문자가 사라진다.
      if (key == BACKSPACE)
      {
        // 닉네임 입력 칸 이전은 삭제를 못하도록 한다.
        if (x > 8)
        {
          gotoXY(x, y);
          printw(" ");
          refresh();
          x--;
          // 이전 글자를 지운다
          *(ptr + --i) = ' ';
          continue;
        }
        else
          continue;
      }
      // 엔터 누르면 입력 종료 (반복문 탈출)
      if (key == ENTER)
      {
        if (*ptr == '\0' || *ptr == SPACE)
        {
          clear();
          printw("닉네임을 입력하세요.\n\n");
          printw("(스페이스바를 누르시면 이전 화면으로 돌아갑니다.)");
          refresh();
          while (1)
          {
            if (keyControl() == SPACE)
              break;
          }
          clear();
          flag = true; // 반복
          break;
        }
        else
        {
          flag = false;
          break;
        }
      }
      x++;
      gotoXY(x, y);
      printw("%c", key); // 키보드로 문자를 입력할 때마다 콘솔에 표시한다.
      refresh();
      *(ptr + i++) = key;
    }
    *(ptr + i) = '\0'; // 문자열 끝에 null 문자를 추가한다.
    ///////////// 이중 반복문 끝
  } while (flag);
  // 닉네임 입력을 하나도 안 하면 다시 하게끔 유도한다.

  // printw("\n strings:%s, integer:%d", userInfo.name, userInfo.name);
  printw("\n\n%s님 환영합니다. 즐거운 시간 되세요.\n\n", userInfo.name);
  printw("게임을 [시작]하시려면 [엔터]를 눌러주세요.\n");
  printw("[메뉴]로 돌아가시려면 [스페이스바]를 눌러주세요.\n");
  refresh();
  while (1)
  {
    int key = keyControl();
    if (key == ENTER)
    {
      userCnt++; // 사용자수 증가
      clear();
      return 1; // 게임 시작
    }
    else if (key == SPACE)
    {
      clear();
      return 0; // 메뉴로 돌아가기
    }
  }
}

// 게임 종료 후 사용자 기록을 보여준다.
void printResult()
{
  clear();
  printw("\t< 최종 기록 >\n\n");
  printw("\t[닉네임] : %s\n", userInfo.name);
  printw("\t[score] : %d\n", userInfo.score);
  printw("\n\n");
  printw("-------------------- ranking --------------------\n");
  printRank(ranking); // 랭킹 기록
  printw("\n\n확인하셨으면 엔터를 눌러주세요.");
  refresh();
  while (1)
  {
    if (keyControl() == ENTER)
    {
      clear();
      break;
    }
  }
}

// score 기록을 저장한다.
int saveScore(int *point)
{
  userInfo.score = *point;
  return userInfo.score;
}

// 랭커를 보여준다.
void printRank(struct User *rankingptr)
{
  int i;
  struct User *rptr; // 구조체의 포인터

  /* for (i = 0; i < sizeof(*ranking) / sizeof(ranking[0]); i++)
    -> 안됐던 이유... void printRank(struct User *ranking)
    파라미터 이름을 배열 이름이랑 같은 걸로 해버려서
    전역변수로 선언된 배열 ranking의 사이즈를 가져오는 게 아니라
    매개변수에 ranking의 (첫번째)주소값이 복사되고 그 값을 가져오고 있던 것 같다...
    그래서 계속 구조체 하나의 크기값인 24만 가져오고 있었던 것 같다...
    매개변수 이름을 다른걸로 변경하고 전역변수에 선언된 배열 ranking의 사이즈를 가져오니
    배열의 크기가 잘 계산되었다.
  */
  for (i = 0; i < sizeof(ranking) / sizeof(struct User); i++)
  {
    rptr = rankingptr + i;
    if (strlen(rptr->name) == 0)
      continue;
    printw("%d \t", i + 1);
    printw("name : %s ", rptr->name);
    printw("\t\tscore : %d\n", rptr->score);
  }
  refresh();
}

// 사용자 정보의 점수와 랭킹배열에 저장되어 있는 점수를 비교한 후
// 사용자 정보보다 높은 점수의 랭킹 인덱스를 구한다.
int getridx(struct User *rankingptr)
{
  struct User *rptr; // 구조체의 포인터
  int ridx, i;
  struct User *nextPtr;
  for (i = 0; i < sizeof(ranking) / sizeof(struct User); i++)
  {
    rptr = rankingptr + i;
    nextPtr = rankingptr + i + 1;
    // 사용자 점수가 랭킹 점수보다 높을 경우
    if (userInfo.score > ranking[i].score)
    {
      ridx = i;
      break; // 반복 종료
    }
    // 사용자 점수가 랭킹 점수보다 낮거나 같을 경우
    // 다음 랭킹 인덱스에 이름이 없을 경우 그 인덱스에 저장한다.
    else if (i < sizeof(ranking) / (sizeof(struct User) - 1) && strlen(nextPtr->name) == 0)
    {
      if (userCnt == 1 && userInfo.score == ranking[i].score)
      {
        ridx = i;
        break;
      }
      else
      {
        ridx = i + 1;
        break;
      }
    }
    else
    {
      ridx = -1;
    }
  }
  return ridx;
}

// 사용자의 점수 순으로 정렬한 후 저장한다.
void sortRank(struct User *rankingptr)
{
  struct User *rptr; // 구조체의 포인터
  int i;

  int ridx = getridx(rankingptr);

  // 그 이하 인덱스의 사용자 정보는 한 칸씩 뒤로 밀려가 저장한다.
  // (5번째를 넘어가는 정보는 없어진다.)

  // 랭킹 인덱스 ridx에 사용자 정보가 저장되고
  // 기존 ridx 정보부터 그 이하는 한 순위씩 아래로 밀려난다.
  char *nextN = (char *)malloc(sizeof(char) * NAME_LIMIT);
  char *tempN = (char *)malloc(sizeof(char) * NAME_LIMIT);
  int nextS, tempS;

  // 기존 ridx정보를 temp에 담는다.
  strcpy(tempN, ranking[ridx].name);
  tempS = ranking[ridx].score;

  for (i = ridx; i < (sizeof(ranking) / sizeof(struct User)) - 1; i++)
  {
    // ridx 다음 정보를 next에 담는다.
    strcpy(nextN, ranking[i + 1].name);
    nextS = ranking[i + 1].score;

    // tempN,tempS를 한 칸 아래로 순위 변경.
    strcpy(ranking[i + 1].name, tempN);
    ranking[i + 1].score = tempS;

    // ridx 다음 인덱스 정보가 다음 반복에서 아래로 내려가는 해당 인덱스가 된다.
    strcpy(tempN, nextN);
    tempS = nextS;
  }

  // 배열의 ridx인덱스에 i번째 사용자 정보를 저장한다.
  strcpy(ranking[ridx].name, userInfo.name);
  ranking[ridx].score = userInfo.score;

  free(nextN);
  free(tempN);
}

// 다시 플레이할 것인지 묻는다.
int askAgain()
{
  clear();
  printw("다시 하시겠습니까?\n\n");
  printw("다시 하기 : y\n");
  printw("그만 하기 : n\n");
  refresh();
  while (1)
  {
    int key = keyControl();
    if (key == Y)
    {
      return 1;
    }
    else if (key == N)
    {
      return 0;
    }
  }
}