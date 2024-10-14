#include "render.h"
#include "common/utils.h"
#include "config/config.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>

static struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
	TTF_Font *font;
	TTF_Font *sfont;
} render;

SDL_Color backgroundColor;
int windowWidth;
int windowHeight;
int fontsize;

int getFontSize() { return fontsize; }
SDL_Window *getWindow() { return render.window; }
SDL_Renderer *getRenderer() {
	SDL_SetRenderDrawColor(render.renderer, ColorUnpack(backgroundColor));
	return render.renderer;
}
SDL_Renderer *getRendererColor(SDL_Color color) {
	SDL_SetRenderDrawColor(render.renderer, ColorUnpack(color));
	return render.renderer;
}

SDL_Texture *createTextTexture(const char *string, int small, SDL_Color color) {
	SDL_Surface *surface;
	if (!small)
		surface = TTF_RenderUTF8_Blended(render.font, string, color);
	else
		surface = TTF_RenderUTF8_Blended(render.sfont, string, color);
	Assert(surface != NULL, "null surface");
	SDL_Texture *texture = SDL_CreateTextureFromSurface(render.renderer, surface);
	Assert(texture!= NULL, "null texture");
	SDL_FreeSurface(surface);
	return texture;
}

void initRender() {
	const int *p = getConfigArray("Color", "BackgroundColor").data;
	backgroundColor = Color(p[0], p[1], p[2]);
	windowWidth = getConfigInt("Layout", "WindowWidth");
	windowHeight = getConfigInt("Layout", "WindowHeight");
	fontsize = getConfigInt("Layout", "FontSize");
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	render.window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		windowWidth, windowHeight, SDL_WINDOW_RESIZABLE);
	if (render.window == NULL) Error(SDL_GetError());
	render.renderer = SDL_CreateRenderer(render.window, -1, 0);
	render.font = TTF_OpenFont("./assets/fonts/noto/NotoSansMono-Bold.ttf", fontsize);
	render.sfont = TTF_OpenFont("./assets/fonts/noto/NotoSansMono-Bold.ttf", fontsize*0.75);
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

void drawText(const char *text, int cx, int cy) {
	SDL_Texture *texture = createTextTexture(text, 1, Color(255, 255, 255));
	int w, h;
	SDL_QueryTexture(texture, NULL, NULL, &w, &h);
	int x = cx - w / 2;
	int y = cy - h / 2;
	SDL_RenderCopy(render.renderer, texture, NULL, &Rect(x, y, w, h));
}
