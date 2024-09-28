#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <strings.h>

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

#include "parser.h"

typedef struct ConfigModule {
	const char *name;
	Table *data;
	const char * (*getString)(const char *);
	void (*setString)(const char *, const char *);
	int (*getInt)(const char *);
	void (*setInt)(const char *, int);
	void (*writeToFile)(void);
	struct ConfigModule *next;
} ConfigModule;
ConfigModule *getConfigModule(const char *);
void loadConfig(const char *configDir);

#endif // __CONFIG_H__
