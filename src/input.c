#include "input.h"
#include "config/config.h"
#include "common/utils.h"

#include <stdlib.h>

#include <SDL2/SDL.h>

static int *keys;

void initInput() {
	if (!keys) {
		keys = calloc(OPT_NUM, sizeof(int));
	}
	keys[OPT_LEFT] = getConfigModule("keymap")->getInt("Left");
	keys[OPT_RIGHT] = getConfigModule("keymap")->getInt("Right");
	keys[OPT_SOFT] = getConfigModule("keymap")->getInt("Down");
	keys[OPT_DROP] = getConfigModule("keymap")->getInt("Drop");
	keys[OPT_ROTATER] = getConfigModule("keymap")->getInt("RotateR");
	keys[OPT_ROTATEC] = getConfigModule("keymap")->getInt("RotateC");
	keys[OPT_HOLD] = getConfigModule("keymap")->getInt("Hold");
	keys[OPT_PAUSE] = getConfigModule("keymap")->getInt("Pause");
}

int getKeyDownOption(const SDL_Event *event) {
	for (int i = OPT_LEFT; i < OPT_NUM; i++) {
		if (keys[i] == event->key.keysym.sym) {
			return i;
		}
	}
	Debug("Skip unuseful keydown: %c", event->key.keysym.sym);
	return NULL;
}


