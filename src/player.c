#include "player.h"
#include "block.h"
#include "common/utils.h"
#include "map.h"
#include <stdbool.h>
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
	bool over; // died

	// keymaps
	/*int keys[OPT_NUM];*/
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

void playerSetKeys(Player *p, ArrayInt keymap[OPT_NUM]) {
	for (int i = OPT_EMPTY; i < OPT_NUM; i++) {
		p->keymap[i].data = keymap[i].data;
		p->keymap[i].length = keymap[i].length;
	}
}

void playerSetMap(Player *p, Map *map) {
	if (p->map) {
		freeMap(p->map);
	}
	p->map = map;
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
bool playerOver(Player *p) {
	return p->over;
}

static void checkLineWithScore(Player *p) {
	int line = checkLine(p->map);
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
		checkLineWithScore(p);
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
			checkLineWithScore(p);
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
				checkLineWithScore(p);
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
}

