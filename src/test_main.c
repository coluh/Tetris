
#include "config/config.h"
#include "common/utils.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
	loadConfig("./config");
	Debug("Value: %s", getConfigModule("rules")->getString("LockDelay"));
	Debug("Value: %s", getConfigModule("keymap")->getString("RotateR"));
	Debug("Value: %s", getConfigModule("keymap")->getString("Pause"));
	Debug("Value: %s", getConfigModule("layout")->getString("FontSize"));
	Debug("Value: %s", getConfigModule("strings")->getString("double_player"));
	return 0;
}
