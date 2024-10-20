#ifndef __MENU_H__
#define __MENU_H__

typedef struct MenuEntry MenuEntry;
typedef struct Menu Menu;

void initMenuConfig();

Menu *new_Menu(int width, int height);
void addMenuEntry(Menu *m, const char *string, void (*func)(void));

void startMenu(Menu *m, int poll);

void freeMenu(Menu *m);

void stopMenu_ptr();

#endif // __MENU_H__
