#include "../common/utils.h"
#include "config.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

typedef struct Config {
	const char *configDir;
	ConfigModule *modules;
} Config;

static Config config;
static ConfigModule *this;

ConfigModule *getConfigModule(const char *name) {
	for (ConfigModule *p = config.modules; p != NULL; p = p->next) {
		if (strcmp(p->name, name) == 0) {
			this = p;
			return this;
		}
	}
	Error("Config Module Not Found");
	return NULL;
}

static void addModule(const char *name, Table *data);

void loadConfig(const char *configDir) {
	char configdir[256];
	// TODO: Error Handling
	strcpy(configdir, configDir);
	if (configdir[strlen(configdir)-1] == '/')
		configdir[strlen(configdir)-1] = '\0';

	DIR *dp = opendir(configdir);
	Assert(dp != NULL, "configdir not found");
	struct dirent *entry;
	while ((entry = readdir(dp))) {
		const char *filename = entry->d_name;
		if (filename[0] == '.')
			continue;
		char path[512];
		snprintf(path, sizeof(path), "%s/%s", configdir, filename);
		Table *table = readConfig(path);
		if (!table) {
			Warning("Skip a file in dir");
			Debug("that file is %s, %s", filename, path);
			continue;
		}

		addModule(filename, table);
	}
}

const char * getString(const char * key) {
	return tableFind(this->data, key);
}
void setString(const char *key, const char *value) {
	Error("Not_Implement");
}
int getInt(const char *key) {
	return atoi(tableFind(this->data, key));
}
void setInt(const char *key, int value) {
	Error("Not_Implement");
}
void writeToFile() {
	Error("Not_Implement");
}

static void addModule(const char *name, Table *data) {
	ConfigModule **p = &config.modules;
	for (; *p != NULL; p = &(*p)->next)
		;
	*p = calloc(1, sizeof(struct ConfigModule));
	ConfigModule *this = *p;
	if (strstr(name, ".cfg")) {
		*strstr(name, ".cfg") = '\0';
	}
	this->name = name;
	this->data = data;
	this->getString = getString;
	this->setString = setString;
	this->getInt = getInt;
	this->setInt = setInt;
	this->writeToFile = writeToFile;
}


