#ifndef __RENDER_H__
#define __RENDER_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define Color(r, g, b) ((SDL_Color){r & 0xFF, g & 0xFF, b & 0xFF, 255})
#define ColorUnpack(sdlc) sdlc.r, sdlc.g, sdlc.b, sdlc.a

void initRender();
void freeRender();
SDL_Window *getWindow();
SDL_Renderer *getRenderer();
SDL_Renderer *getRendererColor(SDL_Color color);
SDL_Texture *createTextTexture(const char *string, SDL_Color color);

#endif // __RENDER_H__
