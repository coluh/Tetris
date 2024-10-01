#include "parser.h"

#include "../common/utils.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*
 * static char *getlt(FILE *fp) {
 *         static char buf[4]; // store a UTF-8 char
 *         uint8_t c = fgetc(fp);
 *         if (c >> 7 == 0) {
 *                 buf[0] = c;
 *                 buf[1] = '\0';
 *         } else
 *         return buf;
 * }
 */

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

static bool in(char c, const char *cs, int cs_len) {
	for (int i = 0; i < cs_len; i++)
		if (c == cs[i])
			return true;
	return false;
}
// TODO: will produce Segmentation Fault when deal with wrong config file
static void parsekv(char *buf, char **k, char **v) {
	int i = 0;;
	while (buf[i] == ' ' || buf[i] == '\t')
		i++;

	char *word1 = &buf[i];
	while (buf[i] > ' ' && buf[i] < 127)
		i++;
	buf[i] = '\0';

	while (buf[i] != '=')
		i++;
	i++;
	while (buf[i] == ' ' || buf[i] == '\t')
		i++;

	char *word2 = &buf[i];
	while (!in(buf[i], "\0\t\n #[", 6))
		i++;
	buf[i] = '\0';

	*k = malloc(strlen(word1) + 1);
	strcpy(*k, word1);
	*v = malloc(strlen(word2) + 1);
	strcpy(*v, word2);
}

static void addEntry(Table *t, TableEntry *e) {
	TableEntry **p = &t->data;
	for (; *p != NULL; p = &(*p)->next)
		;
	(*p)= e;
}

Table *readConfig(const char *path) {
	FILE *fp = fopen(path, "r");
	if (fp == NULL) return NULL;
	Table *t = calloc(1, sizeof(struct Table));
	char buf[128];
	while (!feof(fp)) {
		readline(fp, buf, sizeof(buf));
		if (buf[0] == '\0' || buf[0] == '#' || buf[0] == '[' || buf[0] == '\n')
			continue;
		if (feof(fp))
			break;
		char *k;
		char *v;
		parsekv(buf, &k, &v);
		TableEntry *en = calloc(1, sizeof(struct TableEntry));
		en->key = k;
		en->value = v;
		addEntry(t, en);
	}
	fclose(fp);
	if (t->data == NULL) {
		Debug("config file %s%s%s is empty", CSI_GREEN, path, CSI_END);
		free(t);
		return NULL;
	}
	return t;
}

static void freeTableEntry(TableEntry *e) {
	if (e == NULL)
		return;
	freeTableEntry(e->next);
	free(e->key);
	free(e->value);
}
void freeTable(Table *t) {
	free(t->data);
	free(t);
}

const char *tableFind(Table *table, const char *key) {
	for (TableEntry *p = table->data; p != NULL; p = p->next) {
		if (strcmp(p->key, key) == 0)
			return p->value;
	}
	return NULL;
}
