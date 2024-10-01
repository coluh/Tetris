#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "parser.h"

#include <strings.h>

#define MAX_ARRAY_LEN 10

/* 
 * getConfig()
 *     .getModule("strings")
 *         .getString("exit");
 *         .setString("exit", "Quit");
 *     .getModule("rules")
 *         .getInt("LockDelay");
 *         .setInt("LockDelay", 500);
 *     .getModule("keymap")
 *         .writeToFile();
 *     .load("./config/");
 * */
typedef struct ConfigModule {
	char *name;
	Table *data;
	const char * (*getString)(const char *);
	int (*getInt)(const char *);
	const int *(*getIntArray)(const char *);
	void (*setString)(const char *, const char *);
	void (*setInt)(const char *, int);
	void (*setIntArray)(const char *, int *);
	void (*writeToFile)(void);
	struct ConfigModule *next;
} ConfigModule;
ConfigModule *getConfigModule(const char *);
void loadConfig(const char *configDir);
void freeConfig();

#endif // __CONFIG_H__
