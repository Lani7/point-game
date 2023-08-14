#pragma once

void printMenu();
int selectMenu();
void explainRules();

void gameLoop();

void drawMap(int *, int *);
void drawPointInfo(int *);
void showFoodsEnemies(int, char);

void movePlayer(int *, int *, int, int, int *);
int getPoint(int *);
bool crash(int *, int *);

int keyControl();

void drawGameOver();