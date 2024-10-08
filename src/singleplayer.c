#include "singleplayer.h"
#include "render.h"
#include "common/utils.h"
#include "map.h"
#include "block.h"
#include "input.h"

#include <SDL2/SDL.h>

#include <stdbool.h>
#include <stdint.h>

static SDL_Color BGCOLOR = {0, 127, 233, 255};

static struct {
	int comboNow;
	int lines;
	int level;
	int points;
} s;

static void checkLineWrapper(Map *map) {
	int line = checkLine(map);
	if (line > 0)
		s.comboNow++;
	else
		s.comboNow = 0;
	s.lines += line;
	int k[5] = {0, 100, 300, 500, 800};
	s.points += k[line] * s.level;
	if (line)
		Debug("%d line! +%d", line, k[line] * s.level);
	if (s.comboNow > 1) {
		s.points += 50 * s.comboNow * s.level;
		Debug("\t %d Combo! +%d", s.comboNow, 50 * s.comboNow * s.level);
	}
	if (perfectClear(map)) {
		int kp[5] = {0, 800, 1200, 1800, 2000};
		s.points += kp[line] * s.level;
		Debug("\t Perfect Clear! +%d", kp[line] * s.level);
	}
}

// TODO: these are to ugly
static int forward(Map *map, BlockBag *bag) {
	if (hasFallingBlock(map)) {
		if (move(map, 0, -1) != 0) {
			lock(map);
			checkLineWrapper(map);
			return forward(map, bag);
		}
	} else {
		if (putBlock(map, popBlock(bag)) != 0) {
			return 1;
		} else {
			// Tetris Guideline asked for this.
			if (move(map, 0, -1) != 0) {
				lock(map);
				checkLineWrapper(map);
				return forward(map, bag);
			}
		}
	}
	return 0;
}

static int handleInput(Map *map, BlockBag *bag, int opt) {
	if (!hasFallingBlock(map)) {
		if (opt == OPT_PAUSE) {
			Debug("Pause");
		}
		return 0;
	}
	switch (opt) {
	case OPT_LEFT:
		move(map, -1, 0);
		break;
	case OPT_RIGHT:
		move(map, 1, 0);
		break;
	case OPT_SOFT:
		move(map, 0, -1);
		break;
	case OPT_DROP:
		drop(map);
		checkLineWrapper(map);
		Debug("Lines: %d", s.lines);
		return forward(map, bag);
		break;
	case OPT_ROTATER:
		rotate(map, 3);
		break;
	case OPT_ROTATEC:
		rotate(map, 1);
		break;
	case OPT_HOLD:
		if (!hasHold(map)) {
			hold(map);
			putBlock(map, popBlock(bag));
		} else {
			hold(map);
		}
		break;
	case OPT_PAUSE:
		Debug("Clicked Pause");
		break;
	default:
		break;
	}
	return 0;
}

void singlePlayer() {

	bool running = true;

	SDL_Event event;
	uint32_t last = SDL_GetTicks();
	uint32_t start, current, end;

	s.level = 3;
	Map *map = newMap(NULL);
	BlockBag *bag = newBlockBag();

	while (running) {
		start = SDL_GetTicks();
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				running = false;
			if (event.type == SDL_KEYDOWN) {
				if (handleInput(map, bag, getKeyDownOption(&event)) != 0) {
					Debug("Game Over");
					running = false;
				}
			}
		}

		SDL_RenderClear(getRendererColor(BGCOLOR));
		drawMap(map);
		drawBag(bag, map);
		drawHold(map);
		SDL_RenderPresent(getRenderer());

		current = SDL_GetTicks();
		if (current - last >= 500 && running) {
			if (forward(map, bag) != 0) {
				Debug("Game Over");
				running = false;
			}
			last = current;
		}

		end = SDL_GetTicks();
		if (end - start < 16) {
			SDL_Delay(16 - (end - start));
		} else {
			Warning("Busy");
		}
	}

	// game over
	Debug("Lines: %d\tScore: %d", s.lines, s.points);
	s.lines = 0;
	s.points = 0;
}
