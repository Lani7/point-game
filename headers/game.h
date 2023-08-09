#pragma once

void gameLoop();

void drawMap(int *, int *);
void drawPointInfo(int *);
void showFoodsEnemies(int, char);

void movePlayer(int *, int *, int, int, int *);
int getPoint(int *);
// void moveEnemies(struct *);
void updateMove(int *, int *);

int keyControl();