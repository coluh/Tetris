#include "utils.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

int getrand(int min, int max) {
	static int seed = 0;
	if (!seed) {
		seed = time(0) + clock();
		srand(seed);
	}
	return min + rand() % (max - min);
}

float getrandf(float min, float max, float acc) {
	float k = 1.0 / acc;
	int r = rand() % (int)((max - min) * k);
	r += min * k;
	return (float)r * acc;
}

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

void ArrayIntCopy(ArrayInt *dst, const ArrayInt *src) {
	/*if (dst->data) free(dst->data);*/
	dst->data = malloc(src->length * sizeof(int));
	for (int i = 0; i < src->length; i++)
		dst->data[i] = src->data[i];
	dst->length = src->length;
}

char *copyString(const char *other) {
	char *n = malloc(strlen(other) + 1);
	strcpy(n, other);
	return n;
}

const char *concat(const char *m, const char *n) {
	static char buf[64];
	memset(buf, 0, 64);
	strcpy(buf, m);
	strcat(buf, n);
	return buf;
}

void printError(const char *msg, const char *file, int line, const char *function) {
	fprintf(stderr, "%s: line %d[%s]: \x1B[1;37;41mError:\x1B[0m %s\n", file, line, function, msg);
	fflush(stderr);
}

void arrayStringAdd(ArrayString *a, const char *str) {
	if (a->data == NULL) {
		a->data = calloc(4, sizeof(char*));
		a->length = 0;
		a->capacity = 4;
	}
	if (a->length == a->capacity) {
		a->data = realloc(a->data, a->capacity*2 * sizeof(char*));
		a->capacity *= 2;
	}

	a->data[a->length] = strdup(str);
	a->length++;
}

void freeArrayString(ArrayString a) {
	for (int i = 0; i < a.length; i++)
		free(a.data[i]);
	free(a.data);
}

