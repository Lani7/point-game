#pragma once

void printMenu();
int selectMenu();
void explainRules();

int inputUser();
void printResult();
int saveScore(int *);

void gameLoop(int *, int *);
void setSpeed(int *, int *);
void setEnemyCnt(int *, int *);

void initMap();
void drawMap(int *, int *);
void drawPointInfo(int *);
void showFoodsEnemies(int, char);

void movePlayer(int *, int *, int, int, int *);
int getPoint(int *);
bool crash(int *, int *);
int clearStage(int *, int *);

void drawGameOver();
// void printRank();
int askAgain();

int keyControl();
