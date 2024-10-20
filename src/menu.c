#include "menu.h"
#include "render.h"
#include "common/utils.h"
#include "config/config.h"

#include "animes.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_hints.h>
#include <SDL2/SDL_ttf.h>

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// config variables
static SDL_Color fontColorInactive;
static SDL_Color fontColorActive;
static SDL_Color entryColorInactive;
static SDL_Color entryColorActive;
static SDL_Color menuColor;

static int paddingH;
static int paddingV;
static int marginH;
static int marginV;
extern int fontsize;

void initMenuConfig() {
	fontColorInactive = ColorArray(getConfigArray("Color", "FontColorInactive").data);
	fontColorActive = ColorArray(getConfigArray("Color", "FontColorActive").data);
	entryColorInactive = ColorArrayA(getConfigArray("Color", "EntryColorInactive").data);
	entryColorActive = ColorArrayA(getConfigArray("Color", "EntryColorActive").data);
	menuColor = ColorArrayA(getConfigArray("Color", "MenuColor").data);
	paddingH = getConfigInt("Layout", "PaddingH");
	paddingV = getConfigInt("Layout", "PaddingV");
	marginH = getConfigInt("Layout", "MarginH");
	marginV = getConfigInt("Layout", "MarginV");
}

struct MenuEntry {
	const char *string;
	void (*func)(void);

	// Layout 
	SDL_Texture *text_inactive;
	SDL_Texture *text_active;
	int text_w, text_h; // for convenient
	int x, y, w, h;
	bool active;
};

struct Menu {
	int x, y, w, h;
	MenuEntry *list;
	int list_len;
};

extern int windowWidth;
extern int windowHeight;
Menu *new_Menu(int width, int height) {
	Menu *m = calloc(1, sizeof(struct Menu));
	m->x = (windowWidth - width) / 2;
	m->y = (windowHeight - height) / 2;
	m->w = width;
	m->h = height;
	return m;
}

void addMenuEntry(Menu *m, const char *string, void (*func)(void)) {
	if (!m->list) {
		m->list = calloc(1, sizeof(struct MenuEntry));
	} else {
		m->list = realloc(m->list, (m->list_len + 1) * sizeof(struct MenuEntry));
		/* memset(m->list + m->list_len, 0, sizeof(struct MenuEntry)); */
	}
	MenuEntry *this = &m->list[m->list_len];
	this->string = string;
	this->func = func;
	this->text_inactive = createTextTexture(this->string, 1, fontColorInactive);
	this->text_active = createTextTexture(this->string, 1, fontColorActive);
	SDL_QueryTexture(this->text_active, NULL, NULL, &this->text_w, &this->text_h);
	this->active = false;
	m->list_len++;
}

// assign locations for menu and each menu entry
static void assignLocations(Menu *m) {
	int total_height = m->list_len * (fontsize + paddingV * 2 + marginV * 2) + 2 * paddingV;
	if (total_height > m->h) Warning("Menu Entrys overflow in y");
	int starty = (m->h - total_height) / 2 + paddingV + marginV;

	/* +-----------------------+
	 * |      +----------+     |
	 * |      |  Hello!  |     |
	 * |      +----------+     |
	 * |      +----------+     |
	 * |      |          |     |
	 * |      +----------+     |
	 * +-----------------------+
	 * */
	for (int i = 0; i < m->list_len; i++) {
		 m->list[i].h = m->list[i].text_h + 2 * paddingV;
		 m->list[i].y = starty + i * (fontsize + 2 * paddingV + 2 * marginV);
	}
	int max_width = 0;
	for (int i = 0; i < m->list_len; i++)
		if (m->list[i].text_w > max_width)
			max_width = m->list[i].text_w;
	max_width += paddingH * 2;
	int total_width = max_width + marginH * 2 + paddingH * 2;
	if (total_width > m->w) Warning("Menu Entrys overflow in x");
	int startx = (windowWidth - max_width) / 2;
	for (int i = 0; i < m->list_len; i++) {
		 m->list[i].x = startx;
		 m->list[i].w = max_width;
	}
}

static void updateMenuStatus(Menu *m, int mousex, int mousey, int type) {
	for (int i = 0; i < m->list_len; i++) {
		MenuEntry *this = &m->list[i];
		if (inRect(mousex, mousey, Rect(this->x, this->y, this->w, this->h))) {
			this->active = true;
			if (type == SDL_MOUSEBUTTONDOWN) {
				this->func();
				return;
			}
		} else {
			this->active = false;
		}
	}
}

static void drawMenu(Menu *m) {
	SDL_Renderer *r = getRenderer();
	SDL_Rect rect = {m->x, m->y, m->w, m->h};
	SDL_SetRenderDrawColor(r, ColorUnpack(menuColor));
	SDL_RenderFillRect(r, &rect);
	for (int i = 0; i < m->list_len; i++) {
		MenuEntry *this = &m->list[i];
		if (this->active)
			SDL_SetRenderDrawColor(r, ColorUnpack(entryColorActive));
		else
			SDL_SetRenderDrawColor(r, ColorUnpack(entryColorInactive));
		rect = (SDL_Rect) {this->x, this->y, this->w, this->h};
		SDL_RenderFillRect(r, &rect);
		SDL_Rect dst = {this->x + paddingH, this->y + paddingV, this->text_w, this->text_h};
		if (this->active)
			SDL_RenderCopy(r, this->text_active, NULL, &dst);
		else
			SDL_RenderCopy(r, this->text_inactive, NULL, &dst);
	}
}

static bool running;
void startMenu(Menu *m, int poll) {

	assignLocations(m);
	running = true;
	SDL_Event event;

	while (running) {
		if (!poll) {
			SDL_RenderClear(getRenderer());
			startAnimation(getRenderer());
			drawMenu(m);
			SDL_RenderPresent(getRenderer());

			SDL_WaitEvent(&event);
			if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN &&
						((event.key.keysym.sym == SDLK_BACKSPACE) || (event.key.keysym.sym == SDLK_ESCAPE)))) {
				running = false;
			}
			if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN) {
				int mx, my;
				SDL_GetMouseState(&mx, &my);
				updateMenuStatus(m, mx, my, event.type);
			}
		} else {
			uint32_t start = SDL_GetTicks();
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN &&
							((event.key.keysym.sym == SDLK_BACKSPACE) || (event.key.keysym.sym == SDLK_ESCAPE)))) {
					running = false;
				}
				if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN) {
					int mx, my;
					SDL_GetMouseState(&mx, &my);
					updateMenuStatus(m, mx, my, event.type);
				}
			}
			SDL_RenderClear(getRenderer());
			startAnimation(getRenderer());
			drawMenu(m);
			SDL_RenderPresent(getRenderer());
			uint32_t end = SDL_GetTicks();
			if (end - start < 16)
				SDL_Delay(16 - (end - start));
		}
	}
}

void freeMenu(Menu *m) {
	for (int i = 0; i < m->list_len; i++) {
		SDL_DestroyTexture(m->list[i].text_active);
		SDL_DestroyTexture(m->list[i].text_inactive);
	}
	free(m->list);
	free(m);
}

// TODO: FIX THIS
void stopMenu_ptr() {
	running = false;
}
