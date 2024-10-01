#ifndef __MAP_H__
#define __MAP_H__

#include "block.h"

typedef struct FallingBlock FallingBlock;
typedef struct Map Map;

void initMapConfig();

Map *newMap(SDL_Rect *rect);
void freeMap(Map *m);

bool hasFallingBlock(Map *map);
// returm 1 on fail
int fall(Map *map);
// move in specific way
int move(Map *map, int dx, int dy);
// rotate clockwise for times
int rotate(Map *map, int times);
// lock map->falling to map->block
void lock(Map *map);
// return lines cleared
int checkLine(Map *map);
// place a new block on the map
int putBlock(Map *map, BlockType b);

void drawMap(Map *m);
void drawBag(BlockBag *g, SDL_Rect *rect);

#endif // __MAP_H__
