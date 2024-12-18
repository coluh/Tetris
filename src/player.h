#ifndef TETRIS_PLAYER_H
#define TETRIS_PLAYER_H

#include <stdint.h>
#include "common/utils.h"
#include "block.h"

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

typedef enum KeyState {
	KEY_Down,
	KEY_Up,
	KEY_IsDown,
} KeyState;

typedef struct Player Player;

void initPlayerConfig();

Player *newPlayer(int id);
void freePlayer(Player *p);
void playerSetKeys(Player *p, int id);
void playerMoveMap(Player *p, SDL_Rect *newRect);
void playerGetScore(Player *p, int *lines, int *level, int *points);
BlockBag *playerGetBlockBag(Player *p);
uint32_t playerGetLocktime(Player *p);
void *playerGetMap(Player *p);
int playerGetLinesCleared(Player *p);
void playerSetLinesCleared(Player *p, int linesCleared);

bool playerOver(Player *p);
void updatePlayerLocktime(Player *p);

void playerDraw(Player *p);

void playerHandleKey(Player *p, int k);
void playerHandleKeyUp(Player *p, int k);
void playerUpdate(Player *p);
void playerForward(Player *p);


#endif // TETRIS_PLAYER_H

