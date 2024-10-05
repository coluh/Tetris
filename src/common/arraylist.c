#include "arraylist.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>
#include <strings.h>

struct ArrayList {
	void *data;
	int unit_size;
	int len;
	int cap;
};

#define INIT_CAP 4;

#define LIST_NO(l, i) ((l)->data + (i) * (l)->unit_size)

List *newList(int unit_size) {
	List *l = calloc(1, sizeof(struct ArrayList));
	l->unit_size = unit_size;

	l->len = 0;
	l->cap = INIT_CAP;
	l->data = calloc(l->cap, l->unit_size);
	return l;
}

void listAdd(List *l, void *e) {
	memcpy(LIST_NO(l, l->len), e, l->unit_size);
	l->len++;

	if (l->len == l->cap) {
		l->cap *= 2;
		l->data = realloc(l->data, l->cap * l->unit_size);
		memset(LIST_NO(l, l->len), 0, (l->cap - l->len) * l->unit_size);
	}
}

void listDelete(List *l, int i) {
	Assert(i < l->len, "List delete out of range");
	memmove(LIST_NO(l, i), LIST_NO(l, i + 1), (l->len - i - 1) * l->unit_size);
	l->len--;
}

const void *listGet(List *l, int i) {
	return LIST_NO(l, i);
}

int listLength(List *list) {
	return list->len;
}

void freeList(List *l) {
	free(l->data);
	free(l);
}
