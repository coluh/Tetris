#include "animes.h"
#include "player.h"
#include "render.h"
#include "config/config.h"
#include "map.h"
#include "block.h"
#include "menu.h"
#include "strings.h"

#include "singleplayer.h"
#include "doubleplayer.h"
#include "settings.h"

#include <signal.h>
#include "common/errhandle.h"

int main() {

	signal(SIGSEGV, segv_handler);
	loadConfig("./config/config.json");
	initRender();
	initMenuConfig();
	initBlockConfig();
	initMapConfig();
	initPlayerConfig();
	initStringsConfig();

	Menu *start = new_Menu("start");
	addMenuTitle(start, makeStartpageTitle(getRenderer()));
	addMenuEntry(start, getString("single", 0), singlePlayer);
	addMenuEntry(start, getString("double", 0), doubleplayer);
	addMenuEntry(start, getString("setting", 0), settingsPage);
	addMenuEntry(start, getString("exit", 0), stopMenu_ptr);
	startMenu(start, 1);

	// menu exited
	freeMenu(start);
	freeRender();
	freeConfig();

	return 0;
}
