#include "render.h"
#include "config/config.h"
#include "menu.h"

#include "singleplayer.h"
#include "settings.h"

int main(int argc, char *argv[]) {

	initRender();
	loadConfig("./config/");
	initMenuConfig();
	loadConfig("./config/");

	Menu *start = new_Menu(800, 700);
	addMenuEntry(start, "Single Player", singlePlayer);
	addMenuEntry(start, "Settings", settingsPage);
	addMenuEntry(start, "Exit", stopMenu_ptr);
	startMenu(start);

	// menu exited
	freeMenu(start);
	freeRender();

	return 0;
}
