#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdio.h>

#define PARSER_LINE_SKIP    0
#define PARSER_LINE_NAME    1
#define PARSER_LINE_DATA    2
#define PARSER_LINE_UNKNOWN 3

typedef struct TableEntry {
	char *key;
	char *value;
	struct TableEntry *next;
} TableEntry, *Table;

int checkFormat(FILE *fp, char **k, char **v);
void addEntry(Table *t, char *k, char *v);
Table newTable();
void freeTable(Table t);
const char *tableFind(Table table, const char *key);

#endif // __PARSER_H__
