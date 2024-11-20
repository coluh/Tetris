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
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_timer.h>
#include <bits/types/stack_t.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#define MAX_KEY_COUNT 32

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
	struct {
		SDL_Scancode key;
		void (*f)(Player *p, KeyState state);
	} keymap[MAX_KEY_COUNT];
	bool movePressing;
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
	Debug("read config: DAS: %d\tARR: %d\tSARR: %d\n", DAS, ARR, SARR);
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
	free(p->bag);
	free(p);
}

static void left(Player *p, KeyState state);
static void right(Player *p, KeyState state);
static void soft(Player *p, KeyState state);
static void hard(Player *p, KeyState state);
static void rotater(Player *p, KeyState state);
static void rotatec(Player *p, KeyState state);
static void hold(Player *p, KeyState state);
static void pause(Player *p, KeyState state);

void playerSetKeys(Player *p, int id) {
	Assert(id == 0 || id == 1, "Only two keymap");
	const char *kn[] = {
		"", "Left", "Right", "Down", "Drop",
		"RotateR", "RotateC", "Hold", "Pause",
	};
	void (*callbacks[])(Player *p, KeyState state) = {
		NULL, left, right, soft, hard, rotater, rotatec, hold, pause,
	};
	int keyIndex = 0;
	const jsonVal *keymaps = jsonGetArr(getConfig(), KeyChain { "keymap" }, 1, NULL);
	const jsonObj *keymap = keymaps[id].object;
	for (int i = OPT_LEFT; i <= OPT_PAUSE; i++) {
		const jsonVal *kv = jsonGetVal(keymap, KeyChain { kn[i] }, 1);
		if (kv->type == JSONT_ARR) {
			const jsonVal *keys = kv->array;
			for (int j = 0; j < kv->arrayLen; j++) {
				const char *key = keys[j].string;
				p->keymap[keyIndex].key = SDL_GetScancodeFromName(key);
				p->keymap[keyIndex].f = callbacks[i];
				keyIndex++;
			}
		} else {
			Assert(kv->type == JSONT_STR, "kv->type == JSONT_STR");
			p->keymap[keyIndex].key = SDL_GetScancodeFromName(kv->string);
			p->keymap[keyIndex].f = callbacks[i];
			keyIndex++;
		}
	}
	p->keymap[keyIndex].key = 0;
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

int playerGetLinesCleared(Player *p) { return p->linesCleared; }

void playerSetLinesCleared(Player *p, int linesCleared) { p->linesCleared = linesCleared; }

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
	if (line < 4)
		playSound(SoundEffect_clear);
	else
		playSound(SoundEffect_explode);
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

static void left(Player *p, KeyState state) {
	switch (state) {
	case KEY_Down:
		if (move(p->map, -1, 0) == 0) {
			playSound(SoundEffect_move);
			if (reachBottom(p->map))
				updatePlayerLocktime(p);
		}
		p->movePressing = true;
		p->dasFrame = 0;
		break;
	case KEY_Up:
		p->movePressing = false;
		p->fast = false;
		p->dasFrame = 0;
		p->arrFrame = 0;
		break;
	case KEY_IsDown:
		p->movePressing = true;
		if (p->fast) {
			p->arrFrame++;
			if (p->arrFrame == ARR) {
				p->arrFrame = 0;
				if (move(p->map, -1, 0) == 0) {
					playSound(SoundEffect_move);
					if (reachBottom(p->map))
						updatePlayerLocktime(p);
				}
			}
		}
		break;
	}
}

static void right(Player *p, KeyState state) {
	switch (state) {
	case KEY_Down:
		if (move(p->map, 1, 0) == 0) {
			playSound(SoundEffect_move);
			if (reachBottom(p->map))
				updatePlayerLocktime(p);
		}
		p->movePressing = true;
		p->dasFrame = 0;
		break;
	case KEY_Up:
		p->movePressing = false;
		p->fast = false;
		p->dasFrame = 0;
		p->arrFrame = 0;
		break;
	case KEY_IsDown:
		// FIX: if you press left and right in one time, speed will double
		p->movePressing = true;
		if (p->fast) {
			p->arrFrame++;
			if (p->arrFrame == ARR) {
				p->arrFrame = 0;
				if (move(p->map, 1, 0) == 0) {
					playSound(SoundEffect_move);
					if (reachBottom(p->map))
						updatePlayerLocktime(p);
				}
			}
		}
		break;
	}
}

static void soft(Player *p, KeyState state) {
	switch (state) {
	case KEY_Down:
		p->sarrFrame = 0;
		break;
	case KEY_Up:
		break;
	case KEY_IsDown:
		p->sarrFrame++;
		if (p->sarrFrame == SARR) {
			move(p->map, 0, -1);
			p->sarrFrame = 0;
		}
		break;
	}
}

static void hard(Player *p, KeyState state) {
	switch (state) {
	case KEY_Down:
		drop(p->map);
		playSound(SoundEffect_drop);
		checkLineWrapper(p);
		playerForward(p);
		break;
	case KEY_Up:
		break;
	case KEY_IsDown:
		break;
	}
}

static void rotater(Player *p, KeyState state) {
	switch (state) {
	case KEY_Down:
		if (rotate(p->map, 3) == 0) {
			playSound(SoundEffect_rotate);
			if (reachBottom(p->map))
				updatePlayerLocktime(p);
		}
		break;
	case KEY_Up:
		break;
	case KEY_IsDown:
		break;
	}
}

static void rotatec(Player *p, KeyState state) {
	switch (state) {
	case KEY_Down:
		if (rotate(p->map, 1) == 0) {
			playSound(SoundEffect_rotate);
			if (reachBottom(p->map))
				updatePlayerLocktime(p);
		}
		break;
	case KEY_Up:
		break;
	case KEY_IsDown:
		break;
	}
}

static void hold(Player *p, KeyState state) {
	switch (state) {
	case KEY_Down:
		if (!hasHold(p->map)) {
			holdb(p->map);
			putBlock(p->map, popBlock(p->bag));
		} else {
			holdb(p->map);
		}
		p->locktime = 0;
		break;
	case KEY_Up:
		break;
	case KEY_IsDown:
		break;
	}
}

static void pause(Player *p, KeyState state) {
	SDL_Event e;
	bool waiting = true;
	switch (state) {
	case KEY_Down:
		while (waiting) {
			SDL_WaitEvent(&e);
			if (e.type == SDL_QUIT || e.type == SDL_KEYDOWN) {
				waiting = false;
			}
		}
		break;
	case KEY_Up:
		break;
	case KEY_IsDown:
		break;
	}
}

void playerHandleKey(Player *p, int k) {
	for (int i = 0; ; i++) {
		int key = p->keymap[i].key;
		if (key == 0)
			break;
		if (SDL_GetScancodeFromKey(k) == key)
			p->keymap[i].f(p, KEY_Down);
	}
}

void playerHandleKeyUp(Player *p, int k) {
	for (int i = 0; ; i++) {
		int key = p->keymap[i].key;
		if (key == 0)
			break;
		if (SDL_GetScancodeFromKey(k) == key)
			p->keymap[i].f(p, KEY_Up);
	}
}

// only call this function one time per frame please
void playerUpdate(Player *p) {

	if (p->movePressing && p->dasFrame != DAS) {
		p->dasFrame++;
		if (p->dasFrame == DAS) {
			p->fast = true;
			p->arrFrame = 0;
		}
	}

	p->movePressing = false;

	const uint8_t *state = SDL_GetKeyboardState(NULL);

	for (int i = 0; ; i++) {
		int key = p->keymap[i].key;
		if (key == 0)
			break;
		if (state[key])
			p->keymap[i].f(p, KEY_IsDown);
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

