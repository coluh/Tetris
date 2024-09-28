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

static bool in(char c, char *string) {
	for (int i = 0; string[i] != '\0'; i++)
		if (c == string[i])
			return true;
	return false;
}
// TODO: will produce Segmentation Fault when deal with wrong config file
static void parsekv(char *buf, char **k, char **v) {
	/* int w1s, w1e, w2s, w2e; // word 1/2 start/end */
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
	while (!in(buf[i], "\n\t#[\0"))
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
		char *k;
		char *v;
		parsekv(buf, &k, &v);
		TableEntry *en = calloc(1, sizeof(struct TableEntry));
		en->key = k;
		en->value = v;
		addEntry(t, en);
	}
	return t;
}

const char *tableFind(Table *table, const char *key) {
	for (TableEntry *p = table->data; p != NULL; p = p->next) {
		if (strcmp(p->key, key) == 0)
			return p->value;
	}
	return NULL;
}
