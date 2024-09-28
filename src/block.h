#ifndef __BLOCK_H__
#define __BLOCK_H__

typedef enum BlockType {
	BLOCK_I, BLOCK_O, BLOCK_T, BLOCK_S, BLOCK_Z, BLOCK_J, BLOCK_L, BLOCK_COUNT
} BlockType;
typedef enum RotateState {
	ROTATE_0, ROTATE_R, ROTATE_2, ROTATE_L
} RotateState;

typedef struct FallingBlock FallingBlock;

extern int block_color[BLOCK_COUNT][3];

#endif // __BLOCK_H__
