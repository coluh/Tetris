#include "music.h"
#include "common/utils.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_stdinc.h>
#include <stdlib.h>

#define AMPLITUDE 4000

void initMusic() {
	SDL_Init(SDL_INIT_AUDIO);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
}

void effectFall() {
	static Mix_Chunk fallEffect;

	if (fallEffect.allocated == 0) {

		fallEffect.allocated = 1;
		fallEffect.volume = MIX_MAX_VOLUME;

		int length = 4096;
		Sint16 *buffer = calloc(length, sizeof(Sint16));

		float lamda = 20;
		int t = 0;
		for (int i = 0; i < length; i++) {
			if ((int)(t/lamda) % 2 == 0)
				buffer[i] = AMPLITUDE/2;
			else
				buffer[i] = -AMPLITUDE/2;

			t++;
			lamda += 0.015;
		}

		fallEffect.abuf = (Uint8*)buffer;
		fallEffect.alen = length * sizeof(Sint16);

	}

	Mix_PlayChannel(-1, &fallEffect, 0);
}

void effectRotate() {
	static Mix_Chunk eff;

	if (eff.allocated == 0) {

		eff.allocated = 1;
		eff.volume = MIX_MAX_VOLUME;

		int length = 4096;
		Sint16 *buffer = calloc(length, sizeof(Sint16));

		for (int i = 0; i < length; i++) {
			float k = (float)(length - i) / length;
			k *= k;
			k *= 0.9;
			k += 0.1;
			buffer[i] = getrand(0, AMPLITUDE * k);
		}

		eff.abuf = (Uint8*)buffer;
		eff.alen = length * sizeof(Sint16);
	}

	Mix_PlayChannel(-1, &eff, 0);
}

void freeMusic() {
	Mix_CloseAudio();
}

