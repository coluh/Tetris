#include "../common/utils.h"
#include "config.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

#define MAX_ARRAY_LEN 10

struct ConfigModule {
	char *name;
	Table data;
	struct ConfigModule *next;
};

static char *configFile;
static ConfigModule *configs;

static void addModule(char *name, Table data);

void loadConfig(const char *config) {
	FILE *fp = fopen(config, "r");
	Assert(fp != NULL, "config file not found");

	if (configFile)
		free(configFile);
	configFile = copyString(config);

	char *k, *v;
	int line;
	while ((line = checkFormat(fp, &k, &v)) == PARSER_LINE_SKIP)
		;
	while (!feof(fp)) {
		Assert(line == PARSER_LINE_NAME, "format error in config file: name should be first");
		char *name = copyString(k);

		Table data = newTable();
		while ((line = checkFormat(fp, &k, &v)) != PARSER_LINE_NAME) {
			if (line == PARSER_LINE_SKIP) {
				if (feof(fp)) {
					break;
				}
				continue;
			}
			Assert(line != PARSER_LINE_UNKNOWN, "format error in config file: unknown line");
			addEntry(&data, k, v);
		}
		addModule(name, data);
	}

	fclose(fp);
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
	freeModule(configs);
	free(configFile);
}

static void addModule(char *name, Table data) {
	ConfigModule **p = &configs;
	for (; *p != NULL; p = &(*p)->next)
		;
	*p = calloc(1, sizeof(struct ConfigModule));
	ConfigModule *this = *p;

	this->name = name;
	this->data = data;
}

static ConfigModule *findModule(const char *name) {
	for (ConfigModule *p = configs; p != NULL; p = p->next) {
		if (strcmp(p->name, name) == 0) {
			return p;
		}
	}
	Error("ConfigModule Not Found");
	Debug("%s", name);
	return NULL;
}

const char *getConfigString(const char *module, const char *key) {
	return tableFind(findModule(module)->data, key);
}
int getConfigInt(const char *module, const char *key) {
	return toInt(tableFind(findModule(module)->data, key));
}

ArrayInt getConfigArray(const char *module, const char *key) {
	static int t[MAX_ARRAY_LEN + 1];

	const char *v = tableFind(findModule(module)->data, key);
	char *s = copyString(v);
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
	free(s);
	return newArrayInt(t, index);
}

