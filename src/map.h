#ifndef __MAP_H__
#define __MAP_H__

#include "block.h"

typedef struct FallingBlock FallingBlock;
typedef struct Map Map;

void initMapConfig();

Map *newMap(SDL_Rect *rect);
void getMapRect(Map *map, int *x, int *y, int *w, int *h);
void freeMap(Map *m);

bool hasFallingBlock(Map *map);
bool hasHold(Map *map);
// move in specific way
int move(Map *map, int dx, int dy);
void drop(Map *map);
// rotate clockwise for times
int rotate(Map *map, int times);
// hold
void hold(Map *map);
// lock map->falling to map->block
void lock(Map *map);
// return lines cleared
int checkLine(Map *map);
// add gabbage lines
void addLines(Map *map, int lines);
// no blocks empty
int perfectClear(Map *map);
// place a new block on the map
int putBlock(Map *map, BlockType b);

// These should be removef from here!!
void drawMap(Map *m);
void drawBag(BlockBag *g, Map *m);
void drawHold(Map *m);

void shakeMap(Map *m, int dx, int dy, int dt);

#endif // __MAP_H__
