
#include "block.h"
#include "common/utils.h"
#include "render.h"

#include <math.h>
#include <stdlib.h>

// these are of rotate_0
// base point is the second on second line!!
static const char *blocks_description[BLOCK_NUM] = {/*{{{*/
	"    "
	"@@@@",

	" @@ "
	" @@ ",

	" @  "
	"@@@ ",

	" @@ "
	"@@  ",

	"@@  "
	" @@ ",

	"@   "
	"@@@ ",

	"  @ "
	"@@@ ",
};/*}}}*/

struct BlockBag {
	BlockType current[BLOCK_NUM];
	BlockType later[BLOCK_NUM];
};

// for analyser program
static int blockColor[BLOCK_NUM][3] = {
	{0, 255, 255},
	{255, 255, 0},
	{127, 0, 255},
	{0, 255, 0},
	{255, 0, 0},
	{0, 0, 255},
	{255, 127, 0},
};
const int *getBlockColor(int btype) { return blockColor[btype]; }

// block type; rotate type; block index; x y offset
static int blockShape[BLOCK_NUM][ROTATE_NUM][4][2];
const int (*getBlockShape(int type, int rotate))[2] {
	return blockShape[type][rotate];
}

int getBlockWidth(BlockType b, RotateState r) {
	const int (*shape)[2] = getBlockShape(b, r);
	int minx = shape[0][0];
	int maxx = shape[0][0];
	for (int i = 0; i < 4; i++) {
		if (shape[i][0] < minx)
			minx = shape[i][0];
		if (shape[i][0] > maxx)
			maxx = shape[i][0];
	}
	return maxx - minx + 1;
}

float getBlockCenterX(BlockType b, RotateState r) {
	const int (*shape)[2] = getBlockShape(b, r);
	int minx = shape[0][0];
	int maxx = shape[0][0];
	for (int i = 0; i < 4; i++) {
		if (shape[i][0] < minx)
			minx = shape[i][0];
		if (shape[i][0] > maxx)
			maxx = shape[i][0];
	}
	return (float)(maxx + minx) / 2 + 0.5;
}

void initBlockConfig() {/*{{{*/
	for (int t = 0; t < BLOCK_NUM; t++) {
		const char *s = blocks_description[t];
		int b[4];
		int index = 0;
		for (int i = 0; i < 8; i++) {
			if (s[i] == '@') {
				b[index] = i;
				index++;
			}
		}
		Assert(index == 4, "Wrong blocks_description");
		int x[4], y[4];
		for (int i = 0; i < 4; i++) {
			// base point is the second on the second line!!
			x[i] = b[i] % 4 - 1;
			y[i] = b[i] >= 4 ? 0 : 1;
		}
		if (t == BLOCK_I) {
			for (int i = 0; i < 4; i++) {
				blockShape[t][0][i][0] = i - 1;	// (-1 ~ 2, 0)
				blockShape[t][0][i][1] = 0;
				blockShape[t][1][i][0] = 1;	// (1, -2 ~ 1)
				blockShape[t][1][i][1] = i - 2;
				blockShape[t][2][i][0] = i - 1;	// (-1 ~ 2, -1)
				blockShape[t][2][i][1] = -1;
				blockShape[t][3][i][0] = 0;	// (0, -2 ~ 1)
				blockShape[t][3][i][1] = i - 2;
			}
			continue;
		}
		if (t == BLOCK_O) {
			for (int j = 0; j < 4; j++) {
				blockShape[t][j][0][0] = 0;
				blockShape[t][j][0][1] = 0;
				blockShape[t][j][1][0] = 1;
				blockShape[t][j][1][1] = 0;
				blockShape[t][j][2][0] = 0;
				blockShape[t][j][2][1] = 1;
				blockShape[t][j][3][0] = 1;
				blockShape[t][j][3][1] = 1;
			}
			continue;
		}
		for (int r = 0; r < ROTATE_NUM; r++) {
			for (int i = 0; i < 4; i++) {
				blockShape[t][r][i][0] = x[i];
				blockShape[t][r][i][1] = y[i];
			}
			for (int i = 0; i < 4; i++) {
				int ty = y[i];
				y[i] = -x[i];
				x[i] = ty;
			}
		}
	}
	for (int t = 0; t < BLOCK_NUM; t++) {
		for (int r = 0; r < ROTATE_NUM; r++) {
			/*Debug("Block %d_%d:", t, r);*/
			for (int i = 0; i < 4; i++) {
				/*Debug("\t(%d, %d)", blockShape[t][r][i][0], blockShape[t][r][i][1]);*/
			}
		}
	}
}/*}}}*/

BlockBag *newBlockBag() {
	BlockBag *bag = calloc(1, sizeof(struct BlockBag));
	for (int i = 0; i < BLOCK_NUM; i++) {
		bag->current[i] = i;
		bag->later[i] = i;
	}
	shuffle(bag->current, BLOCK_NUM);
	shuffle(bag->later, BLOCK_NUM);
	return bag;
}

const BlockType *listBag(BlockBag *bag) {
	static BlockType blocks[BLOCK_NUM * 2];
	for (int i = 0; i < BLOCK_NUM; i++)
		blocks[i] = bag->current[i];
	for (int i = 0; i < BLOCK_NUM; i++)
		blocks[i + BLOCK_NUM] = bag->later[i];
	for (int i = 0; i < BLOCK_NUM; i++)
		if (bag->current[i] != BLOCK_NE)
			return &blocks[i];
	for (int i = 0; i < BLOCK_NUM; i++)
		if (bag->later[i] != BLOCK_NE)
			return &blocks[i + BLOCK_NUM];
}

BlockType popBlock(BlockBag *bag) {
	for (int i = 0; i < BLOCK_NUM; i++) {
		if (bag->current[i] != BLOCK_NE) {
			BlockType b = bag->current[i];
			bag->current[i] = BLOCK_NE;
			return b;
		}
	}

	for (int i = 0; i < BLOCK_NUM; i++) {
		bag->current[i] = bag->later[i];
		bag->later[i] = i;
	}
	shuffle(bag->later, BLOCK_NUM);
	// WARNING: this assumes that BLOCK_* is 0 to 6
	return popBlock(bag);
}

void drawBlock(BlockType b, const SDL_Rect *rect) {
	if (b == BLOCK_NE)
		return;
	const int *c = blockColor[b];
	SDL_Renderer *r = getRenderer();
	SDL_SetRenderDrawColor(r, c[0], c[1], c[2], 255);
	SDL_RenderFillRect(r, rect);
	if (b == BLOCK_NE)
		return;

	SDL_SetRenderDrawColor(r, 255, 255, 255, 191);
	for (int h = 0; h < rect->h / 4; h++) {
		SDL_RenderDrawLine(r, rect->x+h, rect->y+h, rect->x+rect->w-h, rect->y+h);
	}
	SDL_SetRenderDrawColor(r, 255, 255, 255, 127);
	for (int w = 0; w < rect->w / 4; w++) {
		SDL_RenderDrawLine(r, rect->x+w, rect->y+w, rect->x+w, rect->y+rect->h-w);
	}
	SDL_SetRenderDrawColor(r, 0, 0, 0, 127);
	for (int h = 0; h < rect->h / 4; h++) {
		SDL_RenderDrawLine(r, rect->x+h, rect->y+rect->h-h, rect->x+rect->w-h, rect->y+rect->h-h);
	}
	SDL_SetRenderDrawColor(r, 0, 0, 0, 63);
	for (int w = 0; w < rect->w / 4; w++) {
		SDL_RenderDrawLine(r, rect->x+rect->w-w, rect->y+w, rect->x+rect->w-w, rect->y+rect->h-w);
	}
}

void drawBlockShadow(BlockType b, const SDL_Rect *rect) {
	const int *c = blockColor[b];
	SDL_Renderer *r = getRenderer();

	int range = rect->w / 2;
	for (int a = 0; a < range; a++) {
		float k = (float)a / range;
		k = sqrt(k);
		SDL_SetRenderDrawColor(r, c[0], c[1], c[2], 255*(1-k));
		/*SDL_SetRenderDrawColor(r, c[0]+(255-c[0])*k, c[1]+(255-c[1])*k, c[2]+(255-c[2])*k, 200+55*(1-k));*/
		/*SDL_SetRenderDrawColor(r, 255, 255, 255, 255*(1-k));*/
		SDL_RenderDrawLines(r, (SDL_Point []){
			{rect->x + a, rect->y + a},
			{rect->x + rect->w - a, rect->y + a},
			{rect->x + rect->w - a, rect->y + rect->h - a},
			{rect->x + a, rect->y + rect->h - a},
			{rect->x + a, rect->y + a},
		}, 5);
	}
}

