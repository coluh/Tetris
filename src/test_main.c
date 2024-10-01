
#include "config/config.h"
#include "common/utils.h"
#include "common/hashmap.h"

#include <stdio.h>

#include <signal.h>
#include "common/errhandle.h"

void test_map();

int main() {
	signal(SIGSEGV, segv_handler);
	loadConfig("./config");
	Debug("Value: %s", getConfigModule("rules")->getString("LockDelay"));
	Debug("Value: %s", getConfigModule("strings")->getString("double_player"));
	Debug("Array Value: %d", getConfigModule("layout")->getIntArray("MenuColor")[0]);
	const int *p = getConfigModule("layout")->getIntArray("FontColorActive");
	Debug("Array: %d %d %d %d", p[0], p[1], p[2], p[3]);
	test_map();
	return 0;
}

void test_map() {
	HashMap *map = newHashMap(sizeof(int));
	insertHashMap(map, "cat", (int []){3});
	insertHashMap(map, "dog", (int []){4});
	insertHashMap(map, "coffee", (int []){5});
	Debug("cat: %d\tdog: %d\tcoffee: %d", *(int*)findHashMap(map, "cat"), *(int*)findHashMap(map, "dog"), *(int*)findHashMap(map, "coffee"));
	int *p = NULL;
	*p = 1;
}
