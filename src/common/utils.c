#include "utils.h"

#include <stdlib.h>
#include <time.h>

void shuffle(int *array, int n) {
	if (n <= 1)
		return;

	srand(time(0) + clock());
	Debug("srand with %d", (int)time(0) + (int)clock());

	for (int i = n - 1; i > 0; i--) {
		int j = rand() % (i + 1);
		int temp = array[i];
		array[i] = array[j];
		array[j] = temp;
	}
}

int toInt(const char *string) {
	const char *p = string;
	int k = 1;
	if (*p == '-') {
		k = -1;
		p++;
	} else if (*p == '+') {
		p++;
	}
	int d = 0;
	while (*p != '\0') {
		int i = *p - '0';
		if (i < 0 || i > 9)
			Error("atoi fail");
		d *= 10;
		d += i;
		p++;
	}
	d *= k;
	return d;
}

void printError(const char *msg, const char *file, int line, const char *function) {
	fprintf(stderr, "%s: line %d[%s]: \x1B[1;37;41mError:\x1B[0m %s\n", file, line, function, msg);
	fflush(stderr);
}

