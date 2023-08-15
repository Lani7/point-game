#pragma once

void printMenu();
int selectMenu();
void explainRules();

void gameLoop(int *);
void setSpeed(int *);

void initMap();
void drawMap(int *, int *);
void drawPointInfo(int *);
void showFoodsEnemies(int, char);

void movePlayer(int *, int *, int, int, int *);
int getPoint(int *);
bool crash(int *, int *);
int clearStage(int *);

int keyControl();

void drawGameOver();