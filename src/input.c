#include "input.h"
#include "config/config.h"
#include "common/utils.h"
#include "common/intmap.h"

#include <SDL2/SDL.h>

static IntMap *keymap;

void initInputConfig() {
	if (!keymap) {
		keymap = newIntMap();
	}
	// corresponde to OPT_* definitions
	const char *options[] = {
		"empty", "Left", "Right", "Down", "Drop", "RotateR", "RotateC", "Hold", "Pause",
	};
	for (int i = OPT_LEFT; i < OPT_NUM; i++) {
		ArrayInt p = getConfigArray("KeyMap1", options[i]);
		for (int j = 0; j < p.length; j++) {
			insertIntMap(keymap, p.data[j], i);
		}
	}
}

int getKeyDownOption(const SDL_Event *event) {
	if (event->key.keysym.sym == SDLK_d)
		Debug("D");
	OptionInt result = getIntMap(keymap, event->key.keysym.sym);
	if (result.exist) {
		return result.data;
	}
	Debug("Skip unuseful keydown: %c", event->key.keysym.sym);
	return OPT_EMPTY;
}


