#include "player.h"
#include "block.h"
#include "common/utils.h"
#include "map.h"
#include "render.h"
#include "config/config.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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

	// keymaps
	ArrayInt keymap[OPT_NUM];
};

Player *newPlayer(int id) {
	Player *p = calloc(1, sizeof(struct Player));
	p->id = id;
	p->map = newMap(NULL);
	p->bag = newBlockBag();
	p->score.level = 3;
	return p;
}

void playerSetKeys(Player *p, int id) {
	Assert(id == 1 || id == 2, "Only two keymap");
	const char * kconfig = id == 1 ? "KeyMap1" : "KeyMap2";
	const char *kn[] = {
		"", "Left", "Right", "Down", "Drop",
		"RotateR", "RotateC", "Hold", "Pause",
	};
	for (int i = OPT_LEFT; i < OPT_NUM; i++) {
		const ArrayInt a = getConfigArray(kconfig, kn[i]);
		ArrayIntCopy(&p->keymap[i], &a);
	}
}

void playerSetMap(Player *p, Map *map) {
	if (p->map) {
		freeMap(p->map);
	}
	p->map = map;
}
Map *playerGetMap(Player *p) { return p->map; }
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

static void playerOperate(Player *p, int opt) {
	if (!hasFallingBlock(p->map)) {
		if (opt == OPT_PAUSE) {
			Debug("Pause");
		}
		return;
		// make sure there is a falling block before move
	}
	switch (opt) {
	case OPT_LEFT:
		move(p->map, -1, 0);
		break;
	case OPT_RIGHT:
		move(p->map, 1, 0);
		break;
	case OPT_SOFT:
		move(p->map, 0, -1);
		break;
	case OPT_DROP:
		drop(p->map);
		checkLineWrapper(p);
		playerForward(p);
		break;
	case OPT_ROTATER:
		rotate(p->map, 3);
		break;
	case OPT_ROTATEC:
		rotate(p->map, 1);
		break;
	case OPT_HOLD:
		if (!hasHold(p->map)) {
			hold(p->map);
			putBlock(p->map, popBlock(p->bag));
		} else {
			hold(p->map);
		}
		break;
	case OPT_PAUSE:
		Debug("Paused");
		break;
	}
}

void playerHandleKey(Player *p, int key) {
	for (int i = OPT_LEFT; i < OPT_NUM; i++) {
		const ArrayInt *a = &p->keymap[i];
		for (int j = 0; j < a->length; j++)
			if (a->data[j] == key)
				playerOperate(p, i);
	}
}

void playerForward(Player *p) {
	if (hasFallingBlock(p->map)) {
		if (move(p->map, 0, -1) != 0) {
			lock(p->map);
			checkLineWrapper(p);
			playerForward(p);
			return;
		}
	} else {
		int r = putBlock(p->map, popBlock(p->bag));
		if (r != 0) {
			p->over = true;
			return;
		} else {
			// immediately move one step
			// Tetris Guideline asked for this
			if (move(p->map, 0, -1) != 0) {
				lock(p->map);
				checkLineWrapper(p);
				playerForward(p);
				return;
			}
		}
	}
}

void playerDraw(Player *p) {
	drawMap(p->map);
	drawBag(p->bag, p->map);
	drawHold(p->map);
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

