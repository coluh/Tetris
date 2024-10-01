#ifndef __BLOCK_H__
#define __BLOCK_H__

#include <stdbool.h>

#include <SDL2/SDL.h>

#define BLOCK_NE -1
#define BLOCK_I	0
#define BLOCK_O	1
#define BLOCK_T	2
#define BLOCK_S	3
#define BLOCK_Z	4
#define BLOCK_J	5
#define BLOCK_L	6
#define BLOCK_NUM 7

// in clockwise direction
#define ROTATE_0 0
#define ROTATE_R 1
#define ROTATE_2 2
#define ROTATE_L 3
#define ROTATE_NUM 4

typedef int BlockType;
typedef int RotateState;
typedef struct BlockBag BlockBag;


void initBlockConfig();
const int *getBlockColor(int btype);
const int (*getBlockShape(int type, int rotate))[2];

// in fact two bags to show preview
BlockBag *newBlockBag();
const BlockType *listBag(BlockBag *bag);
BlockType popBlock(BlockBag *bag);

// x, y are coordinate to window
void drawBlock(BlockType b, const SDL_Rect *rect);
void drawBlockShadow(BlockType b, const SDL_Rect *rect);

#endif // __BLOCK_H__
