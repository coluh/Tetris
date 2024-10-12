#include "render.h"
#include "config/config.h"
#include "map.h"
#include "menu.h"
#include "block.h"
#include "input.h"

#include "singleplayer.h"
#include "settings.h"

#include <signal.h>
#include "common/errhandle.h"

int main() {

	signal(SIGSEGV, segv_handler);
	loadConfig("./config/game.cfg");
	initRender();
	initMenuConfig();
	initBlockConfig();
	initMapConfig();
	initInputConfig();

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
