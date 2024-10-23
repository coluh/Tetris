#include "menu.h"
#include "render.h"
#include "common/utils.h"
#include "config/config.h"

#include "animes.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_hints.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
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

static int menuPadding;
static int paddingH;
static int paddingV;
static int marginH;
static int marginV;
extern int fontsize;

void initMenuConfig() {
	fontColorInactive = ColorArray(getConfigArrayInt(KeyChain { "color", "font", "inactive" }, 3).data);
	fontColorActive = ColorArray(getConfigArrayInt(KeyChain { "color", "font", "active" }, 3).data);
	entryColorInactive = ColorArrayA(getConfigArrayInt(KeyChain { "color", "entry", "inactive" }, 3).data);
	entryColorActive = ColorArrayA(getConfigArrayInt(KeyChain { "color", "entry", "active" }, 3).data);
	menuColor = ColorArrayA(getConfigArrayInt(KeyChain { "color", "menu" }, 2).data);
	paddingH = getConfigInt(KeyChain { "layout", "padding", "h" }, 3);
	paddingV = getConfigInt(KeyChain { "layout", "padding", "v" }, 3);
	marginH = getConfigInt(KeyChain { "layout", "margin", "h" }, 3);
	marginV = getConfigInt(KeyChain { "layout", "margin", "v" }, 3);
	menuPadding = getConfigInt(KeyChain { "layout", "menuPadding" }, 2);
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
	char *label;
	int x, y, w, h;
	SDL_Texture *title;
	SDL_Rect title_r;
	MenuEntry *list;
	int list_len;
};

Menu *new_Menu(const char *label) {
	Menu *m = calloc(1, sizeof(struct Menu));
	m->label = copyString(label);
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

void addMenuTitle(Menu *m, SDL_Texture *title) {
	m->title = title;
}

// assign locations for menu and each menu entry
static void assignLocations(Menu *m) {

	int win_w, win_h;
	SDL_GetWindowSize(getWindow(), &win_w, &win_h);

	int title_w, title_h;
	SDL_QueryTexture(m->title, NULL, NULL, &title_w, &title_h);
	m->title_r.x = (win_w - title_w) / 2;
	m->title_r.w = title_w;
	m->title_r.h = title_h;

	int max_text_w = 0;
	for (int i = 0; i < m->list_len; i++) {
		int w, h;
		SDL_QueryTexture(m->list[i].text_active, NULL, NULL, &w, &h);
		m->list[i].text_w = w;
		m->list[i].text_h = h;
		if (w > max_text_w)
			max_text_w = w;
	}
	int max_w = max_text_w+paddingH*2+marginH*2 > title_w ? max_text_w+paddingH*2+marginH*2 : title_w;

	m->h = m->list_len * (fontsize+paddingV*2+marginV*2) + 2*menuPadding + title_h;
	m->w = max_w+menuPadding*2;
	m->x = (win_w - m->w) / 2;
	m->y = (win_h - m->h) / 2;
	if (m->w > win_w) Warning("Menu overflow in x");
	if (m->w > win_w) Warning("Menu overflow in y");
	m->title_r.y = m->y + menuPadding;

	/* +-----------------------+
	 * |     I AM THE TITLE    |
	 * |      +----------+     |
	 * |      |  Hello!  |     |
	 * |      +----------+     |
	 * |      +----------+     |
	 * |      |   Exit   |     |
	 * |      +----------+     |
	 * +-----------------------+
	 * */

	int start_y = m->y + menuPadding + title_h + marginV;
	for (int i = 0; i < m->list_len; i++) {
		struct MenuEntry *this = &m->list[i];
		this->w = max_text_w + paddingH * 2;
		this->h = fontsize + paddingV * 2;
		this->x = (win_w - this->w) / 2;
		this->y = start_y + i * (fontsize+paddingV*2+marginV*2);
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
	SDL_SetRenderDrawColor(r, ColorUnpack(menuColor));
	SDL_RenderFillRect(r, &(SDL_Rect){m->x, m->y, m->w, m->h});
	SDL_RenderCopy(r, m->title, NULL, &m->title_r);
	for (int i = 0; i < m->list_len; i++) {
		MenuEntry *this = &m->list[i];
		if (this->active)
			SDL_SetRenderDrawColor(r, ColorUnpack(entryColorActive));
		else
			SDL_SetRenderDrawColor(r, ColorUnpack(entryColorInactive));
		SDL_RenderFillRect(r, &(SDL_Rect){this->x, this->y, this->w, this->h});
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
				if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
					assignLocations(m);
				}
			}
			SDL_RenderClear(getRenderer());
			if (strcmp(m->label, "start") == 0)
				startAnimation(getRenderer());
			drawMenu(m);
			SDL_RenderPresent(getRenderer());
			uint32_t end = SDL_GetTicks();
			if (end - start < 16)
				SDL_Delay(16 - (end - start));
		}
	}
	running = true;
}

void freeMenu(Menu *m) {
	for (int i = 0; i < m->list_len; i++) {
		SDL_DestroyTexture(m->list[i].text_active);
		SDL_DestroyTexture(m->list[i].text_inactive);
	}
	free(m->list);
	free(m->label);
	free(m);
}

// TODO: FIX THIS
void stopMenu_ptr() {
	running = false;
}
