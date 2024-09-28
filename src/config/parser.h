#ifndef __PARSER_H__
#define __PARSER_H__

typedef struct TableEntry {
	char *key;
	char *value;
	struct TableEntry *next;
} TableEntry;

typedef struct Table {
	TableEntry *data;
	int len;
} Table;

Table *readConfig(const char *path);
const char *tableFind(Table *table, const char *key);

#endif // __PARSER_H__
