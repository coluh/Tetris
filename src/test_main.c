
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

	const jsonVal *arr = jsonGetArr(jsonGetArr(json, KeyChain { "keymap" }, 1, NULL)[0].object, KeyChain { "Hold" }, 1, NULL);
	printf("Keys: %s %s\n", arr[0].string, arr[1].string);

	// outputJson(json, "");

	freeJson(json);
}
