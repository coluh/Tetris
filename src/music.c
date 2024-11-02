#include "music.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_stdinc.h>
#include <stdlib.h>

static Mix_Chunk fallEffect;

static void createFallEffect();

void initMusic() {
	SDL_Init(SDL_INIT_AUDIO);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	createFallEffect();
}

void effectFall() {
	Mix_PlayChannel(-1, &fallEffect, 0);
}

void freeMusic() {
	if (fallEffect.abuf)
		free(fallEffect.abuf);
	Mix_CloseAudio();
}

static void createFallEffect() {
	fallEffect.allocated = 1;
	fallEffect.volume = MIX_MAX_VOLUME;

	int length = 4096;
	Sint16 *buffer = calloc(length, sizeof(Sint16));

	float lamda = 20;
	int t = 0;
	for (int i = 0; i < length; i++) {
		if ((int)(t/lamda) % 2 == 0)
			buffer[i] = 1000;
		else
			buffer[i] = -1000;

		t++;
		lamda += 0.015;
	}

	fallEffect.abuf = (Uint8*)buffer;
	fallEffect.alen = length * sizeof(Sint16);
}

