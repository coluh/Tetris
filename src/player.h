#ifndef TETRIS_PLAYER_H
#define TETRIS_PLAYER_H

#include "map.h"
#include "common/utils.h"

#define OPT_EMPTY	0
#define OPT_LEFT	1
#define OPT_RIGHT	2
#define OPT_SOFT	3
#define OPT_DROP	4
#define OPT_ROTATER	5
#define OPT_ROTATEC	6
#define OPT_HOLD	7
#define OPT_PAUSE	8
#define OPT_NUM		9

typedef struct Player Player;

Player *newPlayer(int id);
// re-set the map
void playerSetMap(Player *p, Map *map);
Map *playerGetMap(Player *p);
void playerSetKeys(Player *p, int id);
void playerGetScore(Player *p, int *lines, int *level, int *points);
int playerGetLinesCleared(Player *p);
void playerSetLinesCleared(Player *p, int linesCleared);
bool playerOver(Player *p);

void playerDraw(Player *p);

void playerHandleKey(Player *p, int key);
void playerForward(Player *p);


#endif // TETRIS_PLAYER_H

