#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "parser.h"
#include "../common/utils.h"

#include <strings.h>

typedef struct ConfigModule ConfigModule;
void loadConfig(const char *config);
void freeConfig();
const char *getConfigString(const char *module, const char *key);
const int getConfigInt(const char *module, const char *key);
const ArrayInt getConfigArray(const char *module, const char *key);

#endif // __CONFIG_H__
