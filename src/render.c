#include "render.h"
#include "common/utils.h"
#include "config/config.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

static struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
	TTF_Font *font;
} render;

SDL_Color backgroundColor;
int windowWidth;
int windowHeight;
int fontsize;


SDL_Window *getWindow() { return render.window; }
SDL_Renderer *getRenderer() {
	SDL_SetRenderDrawColor(render.renderer, ColorUnpack(backgroundColor));
	return render.renderer;
}
SDL_Renderer *getRendererColor(SDL_Color color) {
	SDL_SetRenderDrawColor(render.renderer, ColorUnpack(color));
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

void initRender() {
	const int *p = getConfigModule("layout")->getIntArray("BackgroundColor");
	backgroundColor = Color(p[0], p[1], p[2]);
	windowWidth = getConfigModule("layout")->getInt("WindowWidth");
	windowHeight = getConfigModule("layout")->getInt("WindowHeight");
	fontsize = getConfigModule("layout")->getInt("FontSize");
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	render.window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		windowWidth, windowHeight, SDL_WINDOW_RESIZABLE);
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
