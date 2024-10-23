
#include "config/config.h"
#include "common/utils.h"
#include "common/arraylist.h"
#include "config/json.h"

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include "common/errhandle.h"

void test_arraylist();
void tests();

int main() {

	tests();

	signal(SIGSEGV, segv_handler);
	loadConfig("./config/game.cfg");
	ArrayInt p = getConfigArray("Color", "MenuColor");
	Debug("Array: %d %d %d %d", p.data[0], p.data[1], p.data[2], p.data[3]);
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


jsonObj *parseJson(const char *path);

void tests() {

	jsonObj *json = parseJson("./config/config.json");

	printf("Window Width: %d\n", jsonGetInt(json, (const char *[]){ "layout", "window", "width" }, 3));
	printf("Font path: %s\n", jsonGetStr(json, (const char *[]){ "layout", "font", "path" }, 3));
	printf("Mode name: %s\n", jsonGetStr(json, (const char *[]){ "strings", "zh", "double" }, 3));

	int c[4];
	jsonGetArrI(json, (const char *[]){ "color", "font", "inactive" }, 3, c);
	printf("FontInact: (%d, %d, %d, %d)\n", c[0], c[1], c[2], c[3]);

	outputJson(json, "");
}
