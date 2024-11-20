#include "music.h"
#include "common/utils.h"
#include "config/config.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_stdinc.h>
#include <math.h>
#include <stdlib.h>

#define AMPLITUDE 4000

static Mix_Chunk *sounds[5];

void initMusic() {
	SDL_Init(SDL_INIT_AUDIO);
	Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 4096);
	sounds[SoundEffect_move] = Mix_LoadWAV(getConfigString(KeyChain { "sounds", "move" }, 2));
	sounds[SoundEffect_rotate] = Mix_LoadWAV(getConfigString(KeyChain { "sounds", "rotate" }, 2));
	sounds[SoundEffect_drop] = Mix_LoadWAV(getConfigString(KeyChain { "sounds", "drop" }, 2));
	sounds[SoundEffect_clear] = Mix_LoadWAV(getConfigString(KeyChain { "sounds", "clear" }, 2));
	sounds[SoundEffect_explode] = Mix_LoadWAV(getConfigString(KeyChain { "sounds", "explode" }, 2));
}

void playSound(SoundEffect soundEffect) {
	Mix_PlayChannel(-1, sounds[soundEffect], 0);
}

void freeMusic() {
	Mix_CloseAudio();
}

