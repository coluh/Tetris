#include "hashmap.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>

#define ITEMS_COUNT 128

typedef struct HashMapNode {
	char *k;
	void *v;
	struct HashMapNode *next;
} HashMapNode;
struct HashMap {
	HashMapNode items[ITEMS_COUNT]; // map by char
	int v_size;
};

HashMap *newHashMap(int v_size) {
	HashMap *m = calloc(1, sizeof(struct HashMap));
	m->v_size = v_size;
	return m;
}
static void insertInList(HashMapNode *head, const char *key, void *value, int v_size) {
	HashMapNode *this;
	if (head->k == NULL) {
		this = head;
	} else {
		HashMapNode **p = &head;
		for (; *p != NULL; p = &(*p)->next)
			;
		(*p) = calloc(1, sizeof(struct HashMapNode));
		this = *p;
	}
	this->k = malloc(strlen(key) + 1);
	strcpy(this->k, key);
	this->v = malloc(v_size);
	memcpy(this->v, value, v_size);
}
void insertHashMap(HashMap *m, const char *key, void *value) {
	int i = (int)key[0];
	Assert(i >= 0 && i < ITEMS_COUNT, "unknown key");
	insertInList(&m->items[(int)key[0]], key, value, m->v_size);
}
void *findHashMap(HashMap *m, const char *key) {
	int i = (int)key[0];
	Assert(i >= 0 && i < ITEMS_COUNT, "unknown key");
	for (HashMapNode *n = &m->items[(int)key[0]]; n != NULL; n = n->next) {
		if (strcmp(n->k, key) == 0) {
			return n->v;
		}
	}
	return NULL;
}
static void freeHashMapNode(HashMapNode *n) {
	if (n == NULL)
		return;
	freeHashMapNode(n->next);
	free(n->k);
	free(n->v);
}
void freeHashMap(HashMap *m) {
	for (int i = 0; i < ITEMS_COUNT; i++) {
		if (m->items[i].next) {
			freeHashMapNode(m->items[i].next);
		}
	}
}
