#include "intmap.h"

#include <stdlib.h>
#include <string.h>

#define ITEMS_COUNT 128

typedef struct IntMapNode {
	int k;
	int v;
	struct IntMapNode *next;
} IntMapNode;
struct IntMap {
	IntMapNode *head;
};

IntMap *newIntMap() {
	return (IntMap *)calloc(1, sizeof(struct IntMap));
}

int getIntMapSize(IntMap *m) {
	int n = 0;
	for (IntMapNode *p = m->head; p != NULL; p = p->next) {
		n++;
	}
	return n;
}

const int *traverseIntMap(IntMap *m, int *n) {
	static int keys[256];
	int index = 0;
	for (IntMapNode *p = m->head; p != NULL; p = p->next) {
		keys[index] = p->k;
		index++;
	}
	if (n)
		*n = index;
	return keys;
}

#define Some(value) ((OptionInt){.data = value, .exist = true})
#define None() ((OptionInt){.exist = false})

OptionInt getIntMap(IntMap *m, int key) {
	for (IntMapNode *p = m->head; p != NULL; p = p->next) {
		if (p->k == key) {
			return Some(p->v);
		}
	}
	return None();
}

void insertIntMap(IntMap *m, int key, int value) {
	IntMapNode **p = &m->head;
	for (; *p != NULL; p = &(*p)->next) {
		if ((*p)->k == key) {
			(*p)->v = value;
			return;
		}
	}
	(*p) = calloc(1, sizeof(struct IntMapNode));
	IntMapNode *this = *p;
	this->k = key;
	this->v = value;
}

static void freeIntMapNode(IntMapNode *n) {
	if (!n)
		return;
	freeIntMapNode(n->next);
	free(n);
}
void freeIntMap(IntMap *m) {
	freeIntMapNode(m->head);
	free(m);
}
