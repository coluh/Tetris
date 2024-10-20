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
	loadConfig("./config/game.cfg");
	initRender();
	initMenuConfig();
	initBlockConfig();
	initMapConfig();
	initPlayerConfig();
	initStringsConfig();

	Menu *start = new_Menu(1400, 1000);
	addMenuEntry(start, getString("single_player", 1), singlePlayer);
	addMenuEntry(start, getString("double_player", 1), doubleplayer);
	addMenuEntry(start, getString("settings", 1), settingsPage);
	addMenuEntry(start, getString("exit", 1), stopMenu_ptr);
	startMenu(start, 1);

	// menu exited
	freeMenu(start);
	freeRender();
	freeConfig();

	return 0;
}
