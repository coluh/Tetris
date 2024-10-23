#ifndef __MENU_H__
#define __MENU_H__

#include <SDL2/SDL_render.h>

typedef struct MenuEntry MenuEntry;
typedef struct Menu Menu;

void initMenuConfig();

Menu *new_Menu(const char *label);
void freeMenu(Menu *m);
void addMenuEntry(Menu *m, const char *string, void (*func)(void));
void addMenuTitle(Menu *m, SDL_Texture *title);

void startMenu(Menu *m, int poll);


void stopMenu_ptr();

#endif // __MENU_H__
