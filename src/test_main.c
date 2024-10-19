
#include "config/config.h"
#include "common/utils.h"
#include "common/hashmap.h"
#include "common/intmap.h"
#include "common/arraylist.h"

#include <signal.h>
#include <string.h>
#include "common/errhandle.h"

void test_map();
void test_arraylist();

int main() {
	signal(SIGSEGV, segv_handler);
	loadConfig("./config/game.cfg");
	ArrayInt p = getConfigArray("Color", "MenuColor");
	Debug("Array: %d %d %d %d", p.data[0], p.data[1], p.data[2], p.data[3]);
	test_map();
	test_arraylist();
	return 0;
}

struct TESTNODE {
	int id;
	char name[10];
	float v;
};

void test_arraylist() {
	List *list = newList(sizeof(struct TESTNODE));
	const char *s[8] = { "zhao", "qian", "sun", "li", "Tom", "Chou", "Joe", "Van" };
	for (int i = 0; i < 8; i++) {
		struct TESTNODE t = { .id = i+1, .v = 1.0f / (i+1) + i+1 };
		strcpy(t.name, s[i]);
		listAdd(list, &t);
	}
	listDelete(list, 0);
	listDelete(list, 3);
	listDelete(list, 5);
	listDelete(list, 0);
	listDelete(list, 2);
	// should left "sun", "li", "Joe"
	for (int i = 0; i < listLength(list); i++) {
		struct TESTNODE *t = (struct TESTNODE *)listGet(list, i);
		Debug("%d: %s: %.2f", t->id, t->name, t->v);
	}
	freeList(list);
}

void test_map() {
	HashMap *map = newHashMap(sizeof(int));
	insertHashMap(map, "cat", (int []){3});
	insertHashMap(map, "dog", (int []){4});
	insertHashMap(map, "coffee", (int []){5});
	Debug("cat: %d\tdog: %d\tcoffee: %d", *(int*)findHashMap(map, "cat"), *(int*)findHashMap(map, "dog"), *(int*)findHashMap(map, "coffee"));

	IntMap *imap = newIntMap();
	intmapSet(imap, 3, 5);
	intmapSet(imap, 444, -7);
	intmapSet(imap, 3, 6);
	OptionInt a = intmapGet(imap, 3);
	Debug("intmap 3: %d, 444: %d", a.data, intmapGet(imap, 444).data);
	OptionInt b = intmapGet(imap, 6);
	if (!b.exist)
		Debug("imap 6: not exist");
}
