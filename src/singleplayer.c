#include "singleplayer.h"
#include "common/utils.h"
#include "player.h"
#include "render.h"
#include "menu.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>


void singlePlayer() {
	bool running;

	Player *p = newPlayer(1);
	playerSetKeys(p, 0);

	uint32_t last = SDL_GetTicks();
	uint32_t current;
	uint32_t start, end;
	SDL_Event event;
	running = true;
	while (running) {
		start = SDL_GetTicks();

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) { running = false; break; }
			if (event.type == SDL_KEYDOWN) {
				playerHandleKey(p, event.key.keysym.sym);
			}
		}

		SDL_RenderClear(getRenderer());
		playerDraw(p);
		SDL_RenderPresent(getRenderer());

		current = SDL_GetTicks();
		if (current - last >= 500 && running) {
			playerForward(p);
			last = current;
		}

		end = SDL_GetTicks();
		if (end - start < 16) {
			SDL_Delay(16 - (end - start));
		} else {
			Warning("Busy");
		}

		if (playerOver(p)) {
			running = false;
		}
	}

	Debug("Game Over");
	int lines, points;
	playerGetScore(p, &lines, NULL, &points);
	Debug("Lines: " CSI_YELLOW "%d" CSI_END "\tPoints: " CSI_YELLOW "%d" CSI_END, lines, points);
	freePlayer(p);

	char linebuf[16];
	char pointsbuf[24];
	sprintf(linebuf, "Line:\t%-4d", lines);
	sprintf(pointsbuf, "Points:\t%-8d", points);

}
