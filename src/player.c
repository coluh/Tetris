#include "player.h"
#include "block.h"
#include "common/utils.h"
#include "config/json.h"
#include "map.h"
#include "render.h"
#include "config/config.h"
#include "music.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_timer.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

struct Player {
	int id;
	Map *map;
	BlockBag *bag;
	struct {
		int lines;
		int level;
		int points;
		int combo;
	} score;
	int linesCleared;
	bool over; // died

	uint32_t locktime;

	// keymaps
	ArrayString keymap[OPT_NUM];
	uint32_t dasFrame;
	bool fast;
	uint32_t arrFrame;
	uint32_t sarrFrame;
};

static uint32_t LockDelay;
static int DAS;
static int ARR;
static int SARR;

void initPlayerConfig() {
	LockDelay = getConfigInt(KeyChain { "rule", "lockDelay" }, 2);
	DAS = getConfigInt(KeyChain { "control", "DAS" }, 2);
	ARR = getConfigInt(KeyChain { "control", "ARR" }, 2);
	SARR = getConfigInt(KeyChain { "control", "SARR" }, 2);
}

Player *newPlayer(int id) {
	Player *p = calloc(1, sizeof(struct Player));
	p->id = id;
	p->map = newMap(NULL);
	mapSetPlayer(p->map, p);
	p->bag = newBlockBag();
	p->score.level = 3;
	p->locktime = 0;
	return p;
}

void freePlayer(Player *p) {
	freeMap(p->map);
	for (int i = 0; i < OPT_NUM; i++)
		freeArrayString(p->keymap[i]);
	free(p->bag);
	free(p);
}

void playerSetKeys(Player *p, int id) {
	Assert(id == 0 || id == 1, "Only two keymap");
	const char *kn[] = {
		"", "Left", "Right", "Down", "Drop",
		"RotateR", "RotateC", "Hold", "Pause",
	};
	const jsonVal *keymaps = jsonGetArr(getConfig(), KeyChain { "keymap" }, 1, NULL);
	const jsonObj *keymap = keymaps[id].object;
	for (int i = OPT_LEFT; i <= OPT_PAUSE; i++) {
		const jsonVal *kv = jsonGetVal(keymap, KeyChain { kn[i] }, 1);
		if (kv->type == JSONT_ARR) {
			const jsonVal *keys = kv->array;
			for (int j = 0; j < kv->arrayLen; j++) {
				const char *key = keys[j].string;
				arrayStringAdd(&p->keymap[i], key);
			}
		} else {
			Assert(kv->type == JSONT_STR, "kv->type == JSONT_STR");
			arrayStringAdd(&p->keymap[i], kv->string);
		}
	}
}

void playerGetScore(Player *p, int *lines, int *level, int *points) {
	if (lines) {
		*lines = p->score.lines;
	}
	if (level) {
		*level = p->score.level;
	}
	if (points) {
		*points = p->score.points;
	}
}

BlockBag *playerGetBlockBag(Player *p) { return p->bag; }

uint32_t playerGetLocktime(Player *p) { return p->locktime; }

void *playerGetMap(Player *p) { return p->map; }

int playerGetLinesCleared(Player *p) {
	return p->linesCleared;
}

void playerSetLinesCleared(Player *p, int linesCleared) {
	p->linesCleared = linesCleared;
}

bool playerOver(Player *p) {
	return p->over;
}

static void checkLineWrapper(Player *p) {
	int line = checkLine(p->map);
	p->linesCleared = line;
	if (line == 0) {
		p->score.combo = 0;
		return;
	}
	p->score.combo++;
	p->score.lines += line;
	int k[5] = { 0, 100, 300, 500, 800};
	p->score.points += k[line] * p->score.level;

	if (p->score.combo > 1) {
		p->score.points += 50 * p->score.combo * p->score.level;
		Debug("\t %d Combo!", p->score.combo);
	}
	if (perfectClear(p->map)) {
		int kp[5] = {0, 800, 1200, 1800, 2000};
		p->score.points += kp[line] * p->score.level;
		Debug("\t Perfect Clear!");
	}
}

void updatePlayerLocktime(Player *p) {
	if (!reachBottom(p->map))
		return;
	p->locktime = SDL_GetTicks();
}

void pause() {
	SDL_Event e;
	bool waiting = true;
	while (waiting) {
		SDL_WaitEvent(&e);
		if (e.type == SDL_QUIT || e.type == SDL_KEYDOWN) {
			waiting = false;
		}
	}
}

static void left(Player *p) {
	if (move(p->map, -1, 0) == 0) {
		updatePlayerLocktime(p);
	} else {
		shakeMap(p->map, -10, 0, 10);
	}
}
static void right(Player *p) {
	if (move(p->map, 1, 0) == 0) {
		updatePlayerLocktime(p);
	} else {
		shakeMap(p->map, 10, 0, 10);
	}
}
static void soft(Player *p) {
	move(p->map, 0, -1);
}
static void hard(Player *p) {
	drop(p->map);
	effectFall();
	checkLineWrapper(p);
	playerForward(p);
}

static void playerOperate(Player *p, int opt) {
	if (!hasFallingBlock(p->map)) {
		if (opt == OPT_PAUSE) {
			pause();
		}
		return;
		// make sure there is a falling block before move
	}
	switch (opt) {
	case OPT_LEFT:
		break;
	case OPT_RIGHT:
		break;
	case OPT_SOFT:
		break;
	case OPT_DROP:
		hard(p);
		break;
	case OPT_ROTATER:
		if (rotate(p->map, 3) == 0) {
			effectRotate();
			updatePlayerLocktime(p);
		}
		break;
	case OPT_ROTATEC:
		if (rotate(p->map, 1) == 0) {
			effectRotate();
			updatePlayerLocktime(p);
		}
		break;
	case OPT_HOLD:
		if (!hasHold(p->map)) {
			hold(p->map);
			putBlock(p->map, popBlock(p->bag));
		} else {
			hold(p->map);
		}
		p->locktime = 0;
		break;
	case OPT_PAUSE:
		pause();
		break;
	}
}

void playerHandleKey(Player *p, int key) {
	for (int i = 0; i < OPT_NUM; i++) {
		for (int j = 0; j < p->keymap[i].length; j++) {
			if (key == SDL_GetKeyFromName(p->keymap[i].data[j])) {
				playerOperate(p, i);
			}
		}
	}
}

void playerHandleKeyUp(Player *p, int key) {
	for (int i = 0; i < OPT_NUM; i++) {
		for (int j = 0; j < p->keymap[i].length; j++) {
			if (key == SDL_GetKeyFromName(p->keymap[i].data[j])) {
				if (i == OPT_LEFT && p->dasFrame > 0) {
					left(p);
				}
				if (i == OPT_RIGHT && p->dasFrame > 0) {
					right(p);
				}
			}
		}
	}
}

static bool optDown(Player *p, int opt) {
	const uint8_t *state = SDL_GetKeyboardState(NULL);
	for (int j = 0; j < p->keymap[opt].length; j++) {
		if (state[SDL_GetScancodeFromName(p->keymap[opt].data[j])]) {
			return true;
		}
	}
	return false;
}

void playerUpdate(Player *p) {

	if (optDown(p, OPT_LEFT) || optDown(p, OPT_RIGHT)) {
		p->dasFrame++;
		if (p->dasFrame == DAS) {
			p->fast = true;
			p->arrFrame = 0;
		}
	} else {
		p->dasFrame = 0;
		p->fast = false;
	}

	if (p->fast) {
		if (optDown(p, OPT_LEFT) || optDown(p, OPT_RIGHT)) {
			p->arrFrame++;
		}
		if (p->arrFrame == ARR) {
			if (optDown(p, OPT_LEFT))
				left(p);
			if (optDown(p, OPT_RIGHT))
				right(p);
			p->arrFrame = 0;
		}
	}

	if (optDown(p, OPT_SOFT)) {
		p->sarrFrame++;
		if (p->sarrFrame == SARR) {
			soft(p);
			p->sarrFrame = 0;
		}
	} else {
		p->sarrFrame = 0;
	}
}

static void playerLock(Player *p) {
	lock(p->map);
	checkLineWrapper(p);
	playerForward(p);
	p->locktime = 0;
}

static void playerDown(Player *p) {
	if (move(p->map, 0, -1) != 0) {
		if (p->locktime == 0)
			p->locktime = SDL_GetTicks();
		else {
			uint32_t current = SDL_GetTicks();
			if (current - p->locktime >= LockDelay) {
				playerLock(p);
			}
		}
	}
}

void playerForward(Player *p) {
	if (hasFallingBlock(p->map)) {
		playerDown(p);
	} else {
		int r = putBlock(p->map, popBlock(p->bag));
		p->locktime = 0;
		if (r != 0) {
			p->over = true;
			return;
		} else {
			// immediately move one step
			// Tetris Guideline asked for this
			playerDown(p);
		}
	}
}

void playerDraw(Player *p) {
	drawMap(p->map);
	drawBag(p->map);
	drawHold(p->map);
	drawLocktime(p->map);
	int x, y, w, h;
	getMapRect(p->map, &x, &y, &w, &h);
	y += h/2;
	x -= w/4;
	int fontsize = getFontSize();
	char buf[32];
	sprintf(buf, "Line: %d", p->score.lines);
	drawText(buf, x, y);
	y += fontsize * 1.2;
	sprintf(buf, "Level: %d", p->score.level);
	drawText(buf, x, y);
	y += fontsize * 1.2;
	sprintf(buf, "Score: %d", p->score.points);
	drawText(buf, x, y);
	y += fontsize * 1.2;
	if (p->score.combo > 0) {
		sprintf(buf, "Combo %d", p->score.combo);
		drawText(buf, x, y+150);
	}
}

