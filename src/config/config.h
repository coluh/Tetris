#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "parser.h"
#include "../common/utils.h"

#include <strings.h>

typedef struct ConfigModule ConfigModule;
void loadConfig(const char *config);
void freeConfig();
const char *getConfigString(const char *module, const char *key);
int getConfigInt(const char *module, const char *key);
ArrayInt getConfigArray(const char *module, const char *key);

// TODO BETTER CONFIG, like string array

#endif // __CONFIG_H__
