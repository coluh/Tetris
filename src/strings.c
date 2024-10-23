#include "strings.h"
#include "common/utils.h"
#include "config/config.h"

#include <stdlib.h>
#include <string.h>

static const char ***table;
static const char *strings[] = {
	"single",
	"double",
	"setting",
	"exit",
};

void initStringsConfig() {
	table = calloc(2, sizeof(char **));
	table[0] = calloc(4, sizeof(char *));
	table[1] = calloc(4, sizeof(char *));
	for (int i = 0; i < 4; i++) {
		table[0][i] = copyString(getConfigString(KeyChain { "strings", "en", strings[i] }, 3));
		table[1][i] = copyString(getConfigString(KeyChain { "strings", "zh", strings[i] }, 3));
	}
}

const char *getString(const char *string, int lang) {
	Assert(lang == 0 || lang == 1, "No other language");
	for (int i = 0; i < 4; i++) {
		if (strcmp(strings[i], string) == 0) {
			return table[lang][i];
		}
	}
	Error("No such string");
	Debug("Error Source: %s", string);
	return NULL;
}
