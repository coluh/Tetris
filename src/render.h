#ifndef __RENDER_H__
#define __RENDER_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define Color(r, g, b) ((SDL_Color){r & 0xFF, g & 0xFF, b & 0xFF, 255})
#define ColorA(r, g, b, a) ((SDL_Color){r & 0xFF, g & 0xFF, b & 0xFF, a & 0xFF})
#define ColorArray(p) ((SDL_Color){p[0] & 0xFF, p[1] & 0xFF, p[2] & 0xFF, 255})
#define ColorArrayA(p) ((SDL_Color){p[0] & 0xFF, p[1] & 0xFF, p[2] & 0xFF, p[3] & 0xFF})
#define ColorUnpack(sdlc) sdlc.r, sdlc.g, sdlc.b, sdlc.a
#define rgb(r, g, b) r,g,b,255
#define rgba(r, g, b, a) r,g,b,a

void initRender();
void freeRender();
int getFontSize();
SDL_Window *getWindow();
SDL_Renderer *getRenderer();
SDL_Renderer *getRendererColor(SDL_Color color);
SDL_Texture *createTextTexture(const char *string, int small, SDL_Color color);

void drawText(const char *text, int cx, int cy);

#endif // __RENDER_H__
