#include "map.h"
#include "block.h"
#include "common/utils.h"
#include "common/arraylist.h"
#include "config/config.h"
#include "render.h"

#include <SDL2/SDL.h>

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <stdbool.h>
#include <stdlib.h>

struct FallingBlock {
	int x;
	int y;
	BlockType type;
	RotateState rotate;
};
struct Map {
	BlockType *block;
	FallingBlock *falling;
	BlockType hold;

	// Layout
	SDL_Rect rect;
	List *dropEffects;
};

struct DropEffect {
	BlockType b;
	float sx;
	int sy;
	int hw;
	int range;
	int remaining;
};

#define REMAIN_TOTAL 60

// config variables
// game rules related to map
static int fieldWidth;
static int fieldHeight;
static int fieldHeightVisible;
static int blockStartX;
static int blockStartY;
static int fieldMargin;

void initMapConfig() {
	fieldWidth = getConfigModule("rules")->getInt("FieldWidth");
	fieldHeight = getConfigModule("rules")->getInt("FieldHeight");
	fieldHeightVisible = getConfigModule("rules")->getInt("FieldHeightVisible");
	blockStartX = getConfigModule("rules")->getInt("BlockStartX");
	blockStartY = getConfigModule("rules")->getInt("BlockStartY");
	fieldMargin = getConfigModule("rules")->getInt("FieldMargin");
}

static BlockType blockAt(const Map *map, int x, int y) {
	return map->block[x + y * fieldWidth];
}
static void setBlockAt(Map *map, int x, int y, BlockType bt) {
	map->block[x + y * fieldWidth] = bt;
}

Map *newMap(SDL_Rect *rect) {
	Map *m = calloc(1, sizeof(struct Map));
	m->block = calloc(fieldWidth * fieldHeight, sizeof(BlockType));
	for (int i = 0; i < fieldWidth; i++) {
		for (int j = 0; j < fieldHeight; j++) {
			setBlockAt(m, i, j, BLOCK_NE);
		}
	}
	m->hold = BLOCK_NE;
	m->dropEffects = newList(sizeof(struct DropEffect));

	if (rect) {
		m->rect = Rect(rect->x, rect->y, rect->w, rect->h);
	} else {
		int ww, wh;
		SDL_GetWindowSize(getWindow(), &ww, &wh);
		wh -= 2 * fieldMargin;
		m->rect = Rect(ww / 3, fieldMargin, wh/2, wh);
	}
	return m;
}

void freeMap(Map *m) {
	free(m->block);
}

static int stuckWith(const Map *map, const FallingBlock *falling) {
	const int (*shape)[2] = getBlockShape(falling->type, falling->rotate);
	for (int i = 0; i < 4; i++) {
		int x = falling->x + shape[i][0];
		int y = falling->y + shape[i][1];
		if (x < 0 || y < 0 || x >= fieldWidth || y >= fieldHeight)
			return 1;
		if (blockAt(map, x, y) != BLOCK_NE)
			return 1;
	}
	return 0;
}

static int stuck(const Map *map) {
	return stuckWith(map, map->falling);
}

bool hasFallingBlock(Map *map) {
	return map->falling != NULL;
}

bool hasHold(Map *map) {
	return map->hold != BLOCK_NE;
}

int move(Map *map, int dx, int dy) {
	Assert(map->falling != NULL, "move() when no falling block");
	map->falling->x += dx;
	map->falling->y += dy;
	if (stuck(map)) {
		map->falling->x -= dx;
		map->falling->y -= dy;
		return 1;
	}
	return 0;
}

void drop(Map *map) {
	while (move(map, 0, -1) == 0)
		;

	int a = map->rect.h / fieldHeightVisible;
	int hw = getBlockWidth(map->falling->type, map->falling->rotate) * a / 2;
	float sx = getBlockCenterX(map->falling->type, map->falling->rotate);
	sx += map->falling->x;
	sx *= a;
	sx += map->rect.x;
	int sy = map->falling->y;
	sy = 19 - sy;
	sy *= a;
	sy += map->rect.y;
	listAdd(map->dropEffects, &(struct DropEffect){
			.b = map->falling->type,
			.sx = sx,
			.sy = sy,
			.hw = hw,
			.range = a * 10,
			.remaining = REMAIN_TOTAL,
	});
	lock(map);
}

int rotate(Map *map, int times) {
	Assert(map->falling != NULL, "rotate() when no falling block");
	if (map->falling->rotate == ROTATE_L) {
		map->falling->rotate = ROTATE_0;
	} else {
		map->falling->rotate++;
	}
	if (stuck(map)) {
		rotate(map, 4 - times);
		return 1;
	}
	return 0;
}

// TODO: MISS A RULE
void hold(Map *map) {
	if (map->hold == BLOCK_NE) {
		map->hold = map->falling->type;
		free(map->falling);
		map->falling = NULL;
	} else {
		BlockType h = map->hold;
		map->hold = map->falling->type;

		map->falling->x = blockStartX;
		map->falling->y = blockStartY;
		map->falling->type = h;
		map->falling->rotate = ROTATE_0;
		// TODO: maybe should check
	}
}

void lock(Map *map) {
	Assert(map->falling != NULL, "lock when no falling block");
	const int (*shape)[2] = getBlockShape(map->falling->type, map->falling->rotate);
	for (int i = 0; i < 4; i++) {
		int x = map->falling->x + shape[i][0];
		int y = map->falling->y + shape[i][1];
		setBlockAt(map, x, y, map->falling->type);
	}
	free(map->falling);
	map->falling = NULL;
}

int checkLine(Map *map) {
	int lines = 0;
	for (int y = 0; y < fieldHeight; y++) {
		bool full = true;
		/*if (y < 5)*/
			/*Debug("\tLine %2d: %2d %2d %2d %2d %2d %2d %2d %2d %2d %2d", y, blockAt(map, 0, y), blockAt(map, 1, y), blockAt(map, 2, y), blockAt(map, 3, y), blockAt(map, 4, y), blockAt(map, 5, y), blockAt(map, 6, y), blockAt(map, 7, y), blockAt(map, 8, y), blockAt(map, 9, y));*/
		for (int x = 0; x < fieldWidth; x++) {
			if (blockAt(map, x, y) == BLOCK_NE) {
				full = false;
				break;
			}
		}
		if (full) {
			lines++;
			for (int j = y; j < fieldHeight-1; j++) {
				for(int i = 0; i < fieldWidth; i++) {
					setBlockAt(map, i, j, blockAt(map, i, j+1));
				}
			}
			y--;
		}
	}
	return lines;
}

int perfectClear(Map *map) {
	for (int y = 0; y < fieldHeight; y++)
		for (int x = 0; x < fieldWidth; x++)
			if (blockAt(map, x, y) != BLOCK_NE)
				return false;
	return true;
}

int putBlock(Map *map, BlockType b) {
	if (map->falling)
		free(map->falling);
	map->falling = calloc(1, sizeof(struct FallingBlock));
	FallingBlock *this = map->falling;
	this->x = blockStartX;
	this->y = blockStartY;
	this->type = b;
	this->rotate = ROTATE_0;
	if (stuck(map)) {
		free(map->falling);
		map->falling = NULL;
		return 1;
	}
	return 0;
}

static void drawDropEffect(Map *m, struct DropEffect *eff);

void drawMap(Map *m) {
	SDL_Renderer *r = getRenderer();
	SDL_RenderFillRect(r, &m->rect);
	int a = m->rect.w / fieldWidth;
	// draw drop dropEffects
	for (int i = 0; i < listLength(m->dropEffects); i++) {
		struct DropEffect *eff = (struct DropEffect *)listGet(m->dropEffects, i);
		drawDropEffect(m, eff);
		eff->remaining--;
		if (eff->remaining <= 0) {
			listDelete(m->dropEffects, i);
		}
	}
	// draw blocks in map
	for (int i = 0; i < fieldWidth; i++) {
		for (int j = 0; j < fieldHeightVisible; j++) {
			// top left
			int x = i;
			int y = fieldHeightVisible - 1 - j;
			x *= a;
			y *= a;
			SDL_Rect rect = {x + m->rect.x, y + m->rect.y, a, a};
			drawBlock(blockAt(m, i, j), &rect);
		}
	}
	if (!m->falling)
		return;
	const int (*shape)[2] = getBlockShape(m->falling->type, m->falling->rotate);
	// draw shadow
	FallingBlock t = {
		.x = m->falling->x,
		.y = m->falling->y,
		.type = m->falling->type,
		.rotate = m->falling->rotate,
	};
	while (!stuckWith(m, &t))
		t.y--;
	t.y++;
	for (int i = 0; i < 4; i++) {
		int x = t.x + shape[i][0];
		int y = t.y + shape[i][1];
		y = fieldHeightVisible - 1 - y;
		x *= a;
		y *= a;
		drawBlockShadow(t.type, &(SDL_Rect){x + m->rect.x, y + m->rect.y, a, a});
	}
	// draw block
	for (int i = 0; i < 4; i++) {
		int x = m->falling->x + shape[i][0];
		int y = m->falling->y + shape[i][1];
		if (y >= fieldHeightVisible)
			continue;
		y = fieldHeightVisible - 1 - y;
		x *= a;
		y *= a;
		SDL_Rect rect = {x + m->rect.x, y + m->rect.y, a, a};
		drawBlock(m->falling->type, &rect);
	}
}

void drawBag(BlockBag *g, Map *map) {
	SDL_Rect rect;
	rect.x = map->rect.x + map->rect.w + fieldMargin;
	rect.y = fieldMargin;
	rect.w = 5 * map->rect.h / fieldHeightVisible;
	rect.w /= 1.5;
	rect.h = rect.w * 3.5;
	SDL_Renderer *r = getRendererColor(Color(0, 0, 0));
	SDL_RenderFillRect(r, &rect);

	int a = (float)rect.w / 5;
	const BlockType *list = listBag(g);
	for (int t = 0; t < BLOCK_NUM - 1; t++) {
		BlockType b = list[t];
		const int (*shape)[2] = getBlockShape(b, ROTATE_0);
		for (int i = 0; i < 4; i++) {
			int x = shape[i][0] + 1;
			int y = shape[i][1];
			y = 2 - y;
			x *= a;
			y *= a;
			y += t * 3 * a - a;
			if (b != BLOCK_I && b != BLOCK_O)
				x += a/2;
			drawBlock(b, &(SDL_Rect){x + rect.x + a/2, y + rect.y + a/2, a, a});
		}
	}
}

void drawHold(Map *map) {
	SDL_Rect rect;
	int a = map->rect.h / fieldHeightVisible;
	rect.x = map->rect.x - fieldMargin - 5 * a;
	rect.y = fieldMargin;
	rect.w = 5 * a;
	rect.h = 4 * a;
	SDL_Renderer *r = getRendererColor(Color(0, 0, 0));
	SDL_RenderFillRect(r, &rect);

	const int (*shape)[2] = getBlockShape(map->hold, ROTATE_0);
	for (int i = 0; i < 4; i++) {
		int x = shape[i][0] + 1;
		int y = shape[i][1];
		y = 2 - y;
		x *= a;
		y *= a;
		drawBlock(map->hold, &(SDL_Rect){x+rect.x+a/2, y+rect.y+a/2, a, a});
	}
}

// sx sy is center of effect beginning
static void drawDropEffect(Map *m, struct DropEffect *eff) {

	const int *c = getBlockColor(eff->b);
	SDL_Renderer *r = getRenderer();
	for (int h = 0; h < eff->range; h++) {
		float a = 255 - (float)255 * h / eff->range;
		a *= (float)eff->remaining / REMAIN_TOTAL;
		SDL_SetRenderDrawColor(r, c[0], c[1], c[2], (int)a);
		SDL_RenderDrawLine(r, eff->sx - eff->hw, eff->sy - h, eff->sx + eff->hw, eff->sy - h);
		a *= 0.5;
		SDL_SetRenderDrawColor(r, c[0], c[1], c[2], (int)a);
		SDL_RenderDrawLine(r, eff->sx - 5, eff->sy - h, eff->sx + 5, eff->sy - h);
	}
}
