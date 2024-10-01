#ifndef __INPUT_H__
#define __INPUT_H__

#include <SDL2/SDL.h>

#define OPT_EMPTY	0
#define OPT_LEFT	1
#define OPT_RIGHT	2
#define OPT_SOFT	3
#define OPT_DROP	4
#define OPT_ROTATER	5
#define OPT_ROTATEC	6
#define OPT_HOLD	7
#define OPT_PAUSE	8
#define OPT_NUM		9

void initInput();

int getKeyDownOption(const SDL_Event *event);

#endif // __INPUT_H__
