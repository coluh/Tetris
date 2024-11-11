#ifndef __MAP_H__
#define __MAP_H__

#include "block.h"

typedef struct FallingBlock FallingBlock;
typedef struct Map Map;

void initMapConfig();

Map *newMap(SDL_Rect *rect);
void getMapRect(Map *map, int *x, int *y, int *w, int *h);
void setMapRect(Map *map, SDL_Rect *rect);
void mapSetPlayer(Map *map, void *player);
void freeMap(Map *m);

bool hasFallingBlock(Map *map);
bool hasHold(Map *map);
bool reachBottom(Map *map);
// move in specific way
int move(Map *map, int dx, int dy);
void drop(Map *map);
// rotate clockwise for times
int rotate(Map *map, int times);
// hold
void holdb(Map *map);
// lock map->falling to map->block
void lock(Map *map);
// return lines cleared
int checkLine(Map *map);
// add gabbage lines
void addLines(Map *map, int lines);
// no blocks
int perfectClear(Map *map);
// place a new block on the map
int putBlock(Map *map, BlockType b);

// These should be removef from here!!
void drawMap(Map *m);
void drawBag(Map *m);
void drawHold(Map *m);
void drawLocktime(Map *map);

#endif // __MAP_H__
