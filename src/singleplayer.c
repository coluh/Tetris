#include "singleplayer.h"
#include "common/utils.h"

#include <SDL2/SDL.h>

#include <stdbool.h>
#include <stdint.h>

void singlePlayer() {
	bool running = true;
	SDL_Event event;
	uint32_t last = SDL_GetTicks();
	uint32_t start, end;
	while (running) {
		start = SDL_GetTicks();
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				running = false;
			if (event.type == SDL_KEYDOWN)
				Error("Debug: keydown");
		}

		/* SDL_RenderClear() */

		uint32_t current = SDL_GetTicks();
		if (current - last >= 1000) {
			Error("Debug: 1s passed~~");
			last = current;
		}

		end = SDL_GetTicks();
		if (end - start < 16) {
			SDL_Delay(16 - (end - start));
		}
	}
}
