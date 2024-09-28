#include "menu.h"
#include "render.h"
#include "common/utils.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static const SDL_Color FONTCOLOR_INACTIVE = {255, 255, 255, 255};
static const SDL_Color FONTCOLOR_ACTIVE = {0, 255, 0, 255};
static const SDL_Color ENTRYCOLOR_INACTIVE = {0, 0, 0, 0};
static const SDL_Color ENTRYCOLOR_ACTIVE = {0, 0, 0, 127};
static const SDL_Color MENUCOLOR = {40, 120, 255, 255};

static int padding = 20;
static int margin = 5;
int fontsize = 48;

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

extern int window_width;
extern int window_height;
Menu *new_Menu(int width, int height) {
	Menu *m = calloc(1, sizeof(struct Menu));
	m->x = (window_width - width) / 2;
	m->y = (window_height - height) / 2;
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
	this->text_inactive = createTextTexture(this->string, FONTCOLOR_INACTIVE);
	this->text_active = createTextTexture(this->string, FONTCOLOR_ACTIVE);
	SDL_QueryTexture(this->text_active, NULL, NULL, &this->text_w, &this->text_h);
	Debug("text_w: %d, text_h: %d", this->text_w, this->text_h);
	this->active = false;
	m->list_len++;
}

// assign locations for menu and each menu entry
static void assignLocations(Menu *m) {
	int total_height = m->list_len * (fontsize + padding * 2 + margin * 2) - margin * 2;
	if (total_height > m->h) Warning("Menu Entrys overflow in y");
	int starty = (m->h - total_height) / 2;

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
		 /* m->list[i].w = m->list[i].text_w + 2 * padding; */
		 m->list[i].h = m->list[i].text_h + 2 * padding;
		 m->list[i].y = starty + padding + i * (fontsize + 2 * padding + 2 * margin);
	}
	int total_width = 0;
	for (int i = 0; i < m->list_len; i++)
		if (m->list[i].text_w > total_width)
			total_width = m->list[i].text_w;
	total_width += padding * 2;
	Debug("total_width: %d\tm->w:%d", total_width, m->w);
	if (total_width > m->w) Warning("Menu Entrys overflow in x");
	for (int i = 0; i < m->list_len; i++) {
		 int startx = (window_width - total_width) / 2;
		 m->list[i].x = startx;
		 m->list[i].w = total_width;
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
	SDL_SetRenderDrawColor(r, ColorUnpack(MENUCOLOR));
	SDL_RenderFillRect(r, &rect);
	for (int i = 0; i < m->list_len; i++) {
		MenuEntry *this = &m->list[i];
		if (this->active)
			SDL_SetRenderDrawColor(r, ColorUnpack(ENTRYCOLOR_ACTIVE));
		else
			SDL_SetRenderDrawColor(r, ColorUnpack(ENTRYCOLOR_INACTIVE));
		rect = (SDL_Rect) {this->x, this->y, this->w, this->h};
		SDL_RenderFillRect(r, &rect);
		SDL_Rect dst = {this->x + padding, this->y + padding, this->text_w, this->text_h};
		if (this->active)
			SDL_RenderCopy(r, this->text_active, NULL, &dst);
		else
			SDL_RenderCopy(r, this->text_inactive, NULL, &dst);
	}
	SDL_RenderPresent(r);
}

static bool running;
void startMenu(Menu *m) {
	assignLocations(m);
	running = true;
	SDL_Event event;
	while (running) {

		SDL_RenderClear(getRenderer());
		drawMenu(m);

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

void stopMenu_ptr() {
	running = false;
}
