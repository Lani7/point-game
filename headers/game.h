#pragma once

void gameLoop();
void movePlayer(int *, int *, int, int, int *);
int getPoint(int *);

void drawMap(int *, int *);
void drawPointInfo(int *);
void showFoodsEnemies();

int keyControl();