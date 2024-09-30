#include "../common/utils.h"
#include "config.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

typedef struct Config {
	char *configDir;
	ConfigModule *modules;
} Config;

static Config config;
static ConfigModule *this;

ConfigModule *getConfigModule(const char *filename) {
	char *name = malloc(strlen(filename) + 1);
	strcpy(name, filename);
	if (strstr(name, ".cfg"))
		*strstr(name, ".cfg") = '\0';
	for (ConfigModule *p = config.modules; p != NULL; p = p->next) {
		if (strcmp(p->name, name) == 0) {
			this = p;
			return this;
		}
	}
	Debug("Config Module %s Not Found", name);
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
	if(!config.configDir) {
		/*char cwd[128];*/
		/*getcwd(cwd, 128);*/
		/*Debug("Current Directory: %s\n", cwd);*/
		config.configDir = malloc(strlen(configdir) + 1);
		strcpy(config.configDir, configdir);
		Debug("config dir: %s", config.configDir);
	}
	struct dirent *entry;
	while ((entry = readdir(dp))) {
		const char *filename = entry->d_name;
		if (filename[0] == '.')
			continue;
		if (!strstr(filename, ".cfg")) {
			continue;
		} else {
			Debug("find config file: %s%s", CSI_GREEN, filename);
		}
		if (getConfigModule(filename)) {
			Debug("Module %s already load", filename);
			continue;// TODO: reload config
		}
		char path[512];
		snprintf(path, sizeof(path), "%s/%s", configdir, filename);
		Table *table = readConfig(path);
		if (!table) {
			Debug("Skip %s%s", CSI_GREEN, path);
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

static void addModule(const char *filename, Table *data) {
	ConfigModule **p = &config.modules;
	for (; *p != NULL; p = &(*p)->next)
		;
	*p = calloc(1, sizeof(struct ConfigModule));
	ConfigModule *this = *p;

	char *name = malloc(strlen(filename) + 1);
	strcpy(name, filename);
	*strstr(name, ".cfg") = '\0';
	this->name = name;

	this->data = data;
	this->getString = getString;
	this->setString = setString;
	this->getInt = getInt;
	this->setInt = setInt;
	this->writeToFile = writeToFile;
}


