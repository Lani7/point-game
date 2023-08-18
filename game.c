#include "headers/game.h"
#include "util.c"

#define WIDTH 20
#define HEIGHT 16
#define DISTANCE 1       // 적이 움직이는 거리
#define INIT_SPEED 20000 // 적의 속도
#define FOOD_CNT 2       // food의 개수
#define ENEMY_CNT 5      // 적의 개수

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
#define QUIT 113    // q
#define ENTER 13    // enter
#define Y 121       // y
#define N 110       // n
#define BACKSPACE 8 // 백스페이스

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

int speed;   // 적이 움직이는 속도
int fcnt;    // food 먹이의 개수
int userCnt; // 사용자수

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

struct EnemyMotion enemyArr[ENEMY_CNT]; // 구조체 배열 선언

// 구조체를 파라미터로 받는 함수들
void moveEnemies(struct EnemyMotion *enemyArr);
void saveRanking(struct User *ranking, int *);
void printRank(struct User *ranking);

void gameLoop(int *stage, int *point)
{
  clear();
  int x, y; // 플레이어 좌표 저장 변수 (x, y)

  fcnt = FOOD_CNT; // food 먹이의 개수
  int playing = 1; // 게임진행중. true

  printw("stage %d start!", *stage);
  refresh();
  sleep(1000000);
  clear();

  // 맵 초기화
  initMap();

  // stage단계별 enemies의 속도 설정
  setSpeed(stage, &speed);

  showFoodsEnemies(FOOD_CNT, 'f');  // 먹이의 좌표를 map에 생성
  showFoodsEnemies(ENEMY_CNT, 'e'); // 적의 좌표를 map에 생성
  drawMap(&x, &y);                  // map 출력. 좌표 포인터를 전달

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

    case QUIT:
      playing = 0; // 게임 종료
      break;
    }

    moveEnemies(enemyArr);

    waitrender(oldTime, speed);

    refresh();

    // 만약 플레이어와 enemy가 충돌하면 게임종료된다.
    if (crash(&x, &y))
    {
      saveScore(point);               // score 기록 저장
      saveRanking(ranking, &userCnt); // rank에 유저 정보 저장
      // 게임 종료 후 게임 오버 화면.
      drawGameOver();
      sleep(2000000);
      playing = 0;
      *point = 0; // 포인트 초기화
    }

    // 먹이를 다 먹으면 clear되고, stage가 올라가면서 다시 게임이 시작된다.
    if (fcnt == 0)
    {
      clear();
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
  // bool flag;
  while (1)
  {
    int key = keyControl();
    if (key == N)
    { // 나가기
      // flag = false;
      // break;
      saveScore(point);               // score 저장
      saveRanking(ranking, &userCnt); // rank에 유저 정보 저장
      return 0;
    }
    else if (key == Y)
    {
      // flag = true;
      // break;
      *stage += 1; // 단계 상승
      gameLoop(stage, point);
      return 1;
    }
  }
  // if (flag == true)
  // {
  //   *stage += 1; // 단계 상승
  //   gameLoop(stage, point);
  //   return 1;
  // }
  // else
  // {
  //   saveScore(point);               // score 저장
  //   saveRanking(ranking, &userCnt); // rank에 유저 정보 저장
  //   return 0;
  // }
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
  for (int i = 0; i < ENEMY_CNT; i++)
  {
    ex = enemyArr[i].x;
    ey = enemyArr[i].y;

    sleep(speed); // stage단계별 speed 속도로 움직임 조절

    if (map[ey][ex] == 'f')
    { // 현재 좌표에 저장되어 있는 게 f라면 먹이를 출력한다.
      gotoXY(ex, ey);
      printw("●");
    }
    else
    { // 현재 좌표의 e를 없어지게 한다.
      gotoXY(ex, ey);
      printw(" ");
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
      printw("◇");

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
      printw("◇");

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

  if (*speed >= 4000) // 최고 속도를 4000으로 제한한다
  {
    *speed -= 2000;
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
  return 0;
}

// 게임 오버 화면을 출력한다.
void drawGameOver()
{
  clear();
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
  printw("Menu (해당하는 번호를 입력하세요.) \n");
  printw("1. 게임 시작\n");
  printw("2. 게임 설명\n");
  printw("3. 종료\n");
  refresh();
}

// 게임 시작 타이틀을 표시한다.
void titlePrint()
{
  printw("Welcome\n"); // curses 모드에서는 printf() 불가
  printw("to the score game.\n");
  refresh(); // 화면의 내용을 갱신. 함수를 호출해야 해당 내용이 보인다.
}

// 게임 규칙을 설명한다.
void explainRules()
{
  printw("1. 키보드 화살키로 ○를 상하좌우로 움직일 수 있다.\n");
  printw("2. ●를 먹으면 포인트를 얻는다.\n");
  printw("3. ◇를 만나면 게임이 종료된다.\n");
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
  printw("사용자의 닉네임을 작성하신 후 엔터를 입력해주세요. (20자 미만)\n");
  printw("닉네임 : ");
  int key;
  char *ptr = userInfo.name;
  int i = 0;

  getch(); // 버퍼에 남아있는 문자 비우기
  // 입력한 문자를 userInfo.name 배열에 저장한다.
  while (i < NAME_LIMIT - 1)
  {
    key = getchar();
    if (key == ENTER)
      break;
    printw("%c", key); // 키보드로 문자를 입력할 때마다 콘솔에 표시한다.
    refresh();
    *(ptr + i++) = key;
  }
  *(ptr + i) = '\0'; // 문자열 끝에 null 문자를 추가한다.

  userCnt++; // 사용자수 증가

  printw("\n%s님 환영합니다. 즐거운 시간 되세요.\n", userInfo.name);
  printw("게임을 시작하시려면 엔터를 눌러주세요.\n");
  printw("메뉴로 돌아가시려면 스페이스바를 눌러주세요.\n");
  refresh();
  while (1)
  {
    int key = keyControl();
    if (key == ENTER)
    {
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
  printw("최종 기록\n");
  printw("닉네임 : %s\n", userInfo.name);
  printw("score : %d\n", userInfo.score);
  printw("\n\n");
  printw("---------ranking---------\n");
  printRank(ranking); // 랭킹 기록
  printw("\n\n메뉴로 돌아가려면 엔터를 눌러주세요.\n");

  while (1)
  {
    if (keyControl() == ENTER)
    {
      clear();
      break;
    }
  }
  refresh();
}

// score 기록을 저장한다.
int saveScore(int *point)
{
  userInfo.score = *point;
  return userInfo.score;
}

// 랭킹 배열에 사용자 정보를 저장한다.
void saveRanking(struct User *ranking, int *userCnt)
{
  printw("saveRanking()..!\n");
  printw("userCnt : %d\n", *userCnt);
  // 만약 사용자 정보가 랭킹 배열 요소보다 점수가 높으면

  // 구조체 배열에 사용자 정보를 저장한다.
  // 처음에는 하나만 저장해야 한다. ->
  // 배열 요소의 점수와 비교해서 저장할 인덱스를 구한 다음 그 곳에 저장한다.
  int i, j;
  struct User *rptr; // 구조체의 포인터

  if (*userCnt == 1)
  {
    rptr = ranking;
    rptr->score = userInfo.score;
    // name이 배열이기 때문에 문자열 저장을 위해 strcpy()함수를 사용한다.
    strcpy(rptr->name, userInfo.name);

    printw("rptr: %d ", rptr);
    printw("score: %d ", rptr->score);
    printw("name: %s \n", rptr->name);
    refresh();
  }

  else if (*userCnt < 6) // 최초 5인까지만 (*userCnt == 5일때까지만)
  {
    printw("size of array 1: %d ", sizeof(*ranking) / sizeof(struct User));

    // 구조체배열에 저장된 게 없으면 userinfo를 저장
    for (i = 0; i < *userCnt; i++)
    {
      // i = ranking + (*userCnt - 1);
      rptr = ranking + i;
      // userinfo구조체의 name배열이 비어있지 않다면 반복을 건너뛴다.
      if (strlen(rptr->name) != 0)
        continue;

      rptr->score = userInfo.score;
      // ranking[i].score = userInfo.score;
      // name이 배열이기 때문에 문자열 저장을 위해 strcpy()함수를 사용한다.
      strcpy(rptr->name, userInfo.name);
      // strcpy(ranking[i].name, userInfo.name);

      printw("i : %d ", i);
      printw("rptr: %d ", rptr);
      printw("score: %d ", rptr->score);
      printw("name: %s \n", rptr->name);
      printw("size of array 2: %d ", sizeof(*ranking) / sizeof(struct User));
      refresh();
    }
  }

  printw("\n sizeof(*ranking)2 : %d", sizeof(*ranking));
  printw(", sizeof(struct User)2 : %d \n ,", sizeof(ranking[0]));
  printw("size2 : %d\n", sizeof(*ranking) / sizeof(ranking[0]));

  while (1)
  {
    if (keyControl() == SPACE)
      break;
  }
}

// 랭커를 보여준다.
void printRank(struct User *rankingptr)
{
  printw("userCnt: %d\n", userCnt);
  // printw("sizeof(*ranking) : %d", sizeof(*ranking));
  // printw("sizeof(ranking[0]) : %d", sizeof(ranking[0]));

  // printw(", sizeof(struct User) : %d \n ,", sizeof(struct User));
  // printw("size : %d\n", sizeof(*ranking) / sizeof(struct User));
  int i, j;
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
  // for (i = 0; i < RANK_CNT; i++)
  // 구조체 배열의 크기만큼 반복한다.
  for (i = 0; i < sizeof(ranking) / sizeof(struct User); i++)
  {
    // rptr = ranking + i;
    rptr = rankingptr + i;
    if (strlen(rptr->name) == 0)
      continue;
    printw("%d위\t", i);
    printw("name : %s, ", rptr->name);
    printw("score : %d\n", rptr->score);
  }
  refresh();
}