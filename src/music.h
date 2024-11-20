#ifndef TETRIS_MUSIC_H
#define TETRIS_MUSIC_H

void initMusic();

typedef enum SoundEffectType {
	SoundEffect_move = 0,
	SoundEffect_rotate,
	SoundEffect_drop,
	SoundEffect_clear,
	SoundEffect_explode,
} SoundEffect;

void playSound(SoundEffect);

void freeMusic();

#endif
