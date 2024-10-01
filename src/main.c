#include "render.h"
#include "config/config.h"
#include "menu.h"
#include "block.h"
#include "input.h"

#include "singleplayer.h"
#include "settings.h"

int main() {

	loadConfig("./config/");
	initRender();
	initMenuConfig();
	initBlockConfig();
	initInput();

	Menu *start = new_Menu(1400, 1000);
	addMenuEntry(start, "Single Player", singlePlayer);
	addMenuEntry(start, "Settings", settingsPage);
	addMenuEntry(start, "Exit", stopMenu_ptr);
	startMenu(start);

	// menu exited
	freeMenu(start);
	freeRender();
	freeConfig();

	return 0;
}
