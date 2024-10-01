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
			free(name);
			return this;
		}
	}
	Debug("Config Module %s Not Found", name);
	free(name);
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

	/*char cwd[128];*/
	/*getcwd(cwd, 128);*/
	/*Debug("Current Directory: %s\n", cwd);*/
	config.configDir = malloc(strlen(configdir) + 1);
	strcpy(config.configDir, configdir);
	Debug("config dir: %s", config.configDir);

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
	closedir(dp);
}

static void freeModule(ConfigModule *m) {
	if (m == NULL)
		return;
	freeModule(m->next);
	free(m->name);
	freeTable(m->data);
	free(m);
}
void freeConfig() {
	free(config.configDir);
	freeModule(config.modules);
}

const char * getString(const char * key) {
	return tableFind(this->data, key);
}
void setString(const char *key, const char *value) {
	Error("Not_Implement");
}
int getInt(const char *key) {
	return toInt(tableFind(this->data, key));
}
void setInt(const char *key, int value) {
	Error("Not_Implement");
}
const int *getIntArray(const char *key) {
	static int t[MAX_ARRAY_LEN + 1];
	const char *v = tableFind(this->data, key);
	char *s = malloc(strlen(v) + 1);
	strcpy(s, v);
	char *word = &s[0];
	int index = 0;
	for (int i = 0; s[i] != '\0'; i++) {
		if (s[i] == ',') {
			s[i] = '\0';
			t[index++] = atoi(word);
			word = &s[i+1];
		}
	}
	if(*word != '\0')
		t[index++] = atoi(word);
	t[index] = 0;
	free(s);
	return t;
}
void setIntArray(const char *key, int *value) {
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
	this->getInt = getInt;
	this->getIntArray = getIntArray;
	this->setString = setString;
	this->setInt = setInt;
	this->setIntArray = setIntArray;
	this->writeToFile = writeToFile;
}


