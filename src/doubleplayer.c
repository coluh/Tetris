#include "doubleplayer.h"
#include "common/utils.h"
#include "map.h"
#include "player.h"
#include "render.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdint.h>

void doubleplayer() {
	bool running;

	// calculate the map position
	int ww, wh;
	SDL_GetWindowSize(getWindow(), &ww, &wh);
	int b = wh * 0.1f;
	wh *= 0.9f;
	int wb = (ww/2-wh/2)/2;

	Player *p = newPlayer(1);
	playerSetKeys(p, 1);
	setMapRect(playerGetMap(p), &(SDL_Rect){wb, b, wh/2, wh});
	Player *q = newPlayer(2);
	playerSetKeys(q, 0);
	setMapRect(playerGetMap(q), &(SDL_Rect){ww/2+wb, b, wh/2, wh});

	uint32_t last, current, start, end;
	last = SDL_GetTicks();
	SDL_Event event;
	running = true;
	while (running) {
		start = SDL_GetTicks();

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) { running = false; }
			if (event.type == SDL_KEYDOWN) {
				playerHandleKey(p, event.key.keysym.sym);
				playerHandleKey(q, event.key.keysym.sym);
			}
		}

		playerUpdate(p);
		playerUpdate(q);
		SDL_RenderClear(getRenderer());
		playerDraw(p);
		playerDraw(q);
		SDL_RenderPresent(getRenderer());

		current = SDL_GetTicks();
		if (current - last >= 500 && running) {
			playerForward(p);
			playerForward(q);
			addLines(playerGetMap(q), playerGetLinesCleared(p));
			addLines(playerGetMap(p), playerGetLinesCleared(q));
			playerSetLinesCleared(p, 0);
			playerSetLinesCleared(q, 0);
			last = current;
		}

		end = SDL_GetTicks();
		if (end - start < 16) {
			SDL_Delay(16 - (end - start));
		} else {
			Warning("Busy");
		}
		if (playerOver(p) || playerOver(q)) {
			running = false;
		}
	}

	Debug("Game Over");
	int lines, points;
	playerGetScore(p, &lines, NULL, &points);
	Debug("Player 1: Lines: " CSI_YELLOW "%d" CSI_END "\tPoints: " CSI_YELLOW "%d" CSI_END, lines, points);
	playerGetScore(q, &lines, NULL, &points);
	Debug("Player 2: Lines: " CSI_YELLOW "%d" CSI_END "\tPoints: " CSI_YELLOW "%d" CSI_END, lines, points);
}
