#include "render.h"
#include "common/utils.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

static struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
	TTF_Font *font;
} render;

const SDL_Color BGCOLOR = {0, 0, 31, 255};

SDL_Renderer *getRenderer() {
	SDL_SetRenderDrawColor(render.renderer, ColorUnpack(BGCOLOR));
	return render.renderer;
}

SDL_Texture *createTextTexture(const char *string, SDL_Color color) {
	SDL_Surface *surface = TTF_RenderUTF8_Blended(render.font, string, color);
	Assert(surface != NULL, "null surface");
	SDL_Texture *texture = SDL_CreateTextureFromSurface(render.renderer, surface);
	Assert(texture!= NULL, "null texture");
	SDL_FreeSurface(surface);
	return texture;
}

int window_width = 1400;
int window_height = 800;
extern int fontsize;

void initRender() {
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	render.window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		window_width, window_height, SDL_WINDOW_RESIZABLE);
	if (render.window == NULL) Error(SDL_GetError());
	render.renderer = SDL_CreateRenderer(render.window, -1, 0);
	render.font = TTF_OpenFont("./assets/fonts/noto/NotoMono-Regular.ttf", fontsize);
	if (render.renderer == NULL || render.font == NULL) {
		Error(SDL_GetError());
	}
	SDL_SetRenderDrawBlendMode(render.renderer, SDL_BLENDMODE_BLEND);
}

void freeRender() {
	TTF_CloseFont(render.font);
	SDL_DestroyRenderer(render.renderer);
	SDL_DestroyWindow(render.window);
	TTF_Quit();
	SDL_Quit();
}
