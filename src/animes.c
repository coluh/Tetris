#include "animes.h"
#include "block.h"
#include "common/utils.h"
#include "config/config.h"
#include "render.h"

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <stdlib.h>

SDL_Color bgC;

void startAnimation(SDL_Renderer *r) {
	static int init = 1;
	struct mBlock {
		float x, y, a, v;
		int alpha;
		BlockType b;
		RotateState r;
	};
	static struct mBlock *pool;
	static int poolsize;
	static int passed = 0;
	passed++;

	int ww, wh;
	SDL_GetWindowSize(getWindow(), &ww, &wh);
	if (init) {
		init = 0;
		const int *p = getConfigArrayInt(KeyChain { "color", "background" }, 2).data;
		bgC.r = p[0];
		bgC.g = p[1];
		bgC.b = p[2];
		const int total = 100;
		pool = calloc(total, sizeof(struct mBlock));
		poolsize = total;

		for (int i = 0; i < poolsize; i++) {
			struct mBlock *this = &pool[i];
			this->x = getrand(0, ww);
			this->y = getrand(-wh/3, wh);
			this->a = getrand(ww/80, ww/40);
			this->alpha = 255 * this->a / (ww/40.0f);
			this->v = getrandf(wh/24.0/60, wh/12.0/60, 0.1f);
			this->b = getrand(0, BLOCK_NUM);
			this->r = getrand(0, ROTATE_NUM);
		}
	}

	// update state
	for (int i = 0; i < poolsize; i++) {
		struct mBlock *this = &pool[i];
		this->y += this->v;
		if (this->y > wh + wh/6.f) {
			this->y = getrand(-wh/3.f, -wh/6.f);
			this->x = getrand(0, ww);
			this->a = getrand(ww/80, ww/40);
			this->alpha = 255 * this->a / (ww/40.0f);
			this->b = getrand(0, BLOCK_NUM);
			this->r = getrand(0, ROTATE_NUM);
		}
	}
	if (passed == 3) {
		passed = 0;
		pool[getrand(0, poolsize)].r = getrand(0, ROTATE_NUM);
		pool[getrand(0, poolsize)].r = getrand(0, ROTATE_NUM);
		pool[getrand(0, poolsize)].r = getrand(0, ROTATE_NUM);
		pool[getrand(0, poolsize)].r = getrand(0, ROTATE_NUM);
		pool[getrand(0, poolsize)].r = getrand(0, ROTATE_NUM);
	}

	// draw part
	for (int i = 0; i < poolsize; i++) {
		struct mBlock *this = &pool[i];
		const int (*shape)[2] = getBlockShape(this->b, this->r);
		for (int i = 0; i < 4; i++) {
			float x = shape[i][0] * this->a + this->x;
			float y = (2 - shape[i][1]) * this->a + this->y;
			drawBlock(this->b, &(SDL_Rect){x, y, this->a, this->a});
			SDL_SetRenderDrawColor(r, bgC.r, bgC.g, bgC.b, 255 - this->alpha);
			SDL_RenderFillRect(r, &(SDL_Rect){x, y, this->a, this->a});
		}
	}
}

SDL_Texture *makeStartpageTitle(SDL_Renderer *r) {
	static SDL_Texture *texture = NULL;

	if (texture)
		return texture;

	int ww;
	SDL_GetWindowSize(getWindow(), &ww, NULL);
	int a = ww * 0.07;
	texture = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 3*a, 2*a);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

	SDL_SetRenderTarget(r, texture);
	SDL_SetRenderDrawColor(r, 0, 0, 0, 0);
	SDL_RenderClear(r);

	drawBlock(BLOCK_T, &(SDL_Rect){ 0, 0, a, a });
	drawBlock(BLOCK_T, &(SDL_Rect){ a, 0, a, a });
	drawBlock(BLOCK_T, &(SDL_Rect){ a*2, 0, a, a });
	drawBlock(BLOCK_T, &(SDL_Rect){ a, a, a, a });
	SDL_SetRenderDrawColor(r, rgba(0, 255, 0, 191));
	for (int x = a*1; x < a*2.5; x++) {
		for (int b = a*3; b < a*5; b++) {
			int y = b - 2 * x;
			SDL_RenderDrawPoint(r, x, y);
		}
	}

	SDL_SetRenderTarget(r, NULL);

	return texture;
}

