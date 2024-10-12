#include "parser.h"

#include "../common/utils.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

static void readline(FILE *fp, char *buf, int n) {
	if (feof(fp)) {
		buf[0] = '\0';
		return;
	}
	char ch;
	int i = 0;
	while ((ch = fgetc(fp)) != '\n') {
		if (feof(fp))
			break;
		buf[i] = ch;
		i++;
		if (i == n) {
			Error("line too long in config file");
		}
	}
	buf[i] = '\0';
}

static void stripAll(char *buf) {
	int index = 0;
	for (char *p = buf; *p != '\0'; p++) {
		if (*p == ' ' || *p == '\t') {
			continue;
		} else {
			buf[index] = *p;
			index++;
		}
	}
	buf[index] = '\0';
}

int checkFormat(FILE *fp, char **k, char **v) {
	static char kb[128];
	static char vb[128];
	char buf[128];
	readline(fp, buf, sizeof(buf));
	stripAll(buf);

	if (buf[0] == '#' || buf[0] == '\0') {
		return PARSER_LINE_SKIP;
	}

	char *p1 = strchr(buf, '[');
	char *p2 = strchr(buf, ']');
	if (p1 && p2 && p2 > p1) {
		int len = p2 - p1 - 1;
		memcpy(kb, p1+1, len);
		kb[len] = '\0';
		*k = kb;
		return PARSER_LINE_NAME;
	}

	if (strchr(buf, '=')) {
		char *e = strchr(buf, '=');
		char *o = strchr(buf, '#');
		if (o == NULL)
			o = buf + strlen(buf);
		memcpy(kb, buf, e-buf);
		kb[e-buf] = '\0';
		memcpy(vb, e+1, o-e-1);
		vb[o-e-1] = '\0';
		*k = kb;
		*v = vb;
		return PARSER_LINE_DATA;
	}

	Error("Error when parsing config file");
	Debug("Unknown format: %s", buf);
	return PARSER_LINE_UNKNOWN;
}

void addEntry(Table *t, char *k, char *v) {

	TableEntry **p = t;
	for (; *p != NULL; p = &(*p)->next) {
		;
	}

	*p = calloc(1, sizeof(struct TableEntry));
	TableEntry *this = *p;
	this->key = copyString(k);
	this->value = copyString(v);
}

Table newTable() {
	return NULL;
}

static void freeTableEntry(TableEntry *e) {
	if (e == NULL)
		return;
	freeTableEntry(e->next);

	free(e->key);
	free(e->value);
}
void freeTable(Table t) {
	freeTableEntry(t);
	free(t);
}

const char *tableFind(Table table, const char *key) {
	for (TableEntry *p = table; p != NULL; p = p->next) {
		if (strcmp(p->key, key) == 0)
			return p->value;
	}
	Error("KEY Not Found");
	return NULL;
}
