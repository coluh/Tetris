#include "json.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct jsonPair {
	char *key;
	jsonVal *value;
	struct jsonPair *next;
} jsonPair;

struct jsonObj {
	jsonPair *pairs;
};

static void addPair(jsonObj *obj, char *key, jsonVal *value) {
	jsonPair **p = &obj->pairs;
	for (; (*p) != NULL; p = &(*p)->next)
		;
	(*p) = calloc(1, sizeof(struct jsonPair));
	(*p)->key = key;
	(*p)->value = value;
}

void freeJson(jsonObj *json);

static void freeValue(jsonVal *v) {
	if (v == NULL) return;
	if (v->type == JSONT_STR) {
		free(v->string);
	} else if (v->type == JSONT_OBJ) {
		freeJson(v->object);
	} else if (v->type == JSONT_ARR) {
		for (int i = 0; i < v->arrayLen; i++) {
			freeValue(&v->array[i]);
		}
		free(v->array);
	}
}

void freeJson(jsonObj *json) {
	if (json == NULL) return;
	for (jsonPair *p = json->pairs; p != NULL; ) {
		jsonPair *next = p->next;
		free(p->key);
		freeValue(p->value);
		free(p->value);
		free(p);
		p = next;
	}
}

typedef struct jsonValArray {
	jsonVal *arr;
	int len;
	int cap;
} jsonValArray;

static jsonValArray *newValueArray() {
	jsonValArray *a = calloc(1, sizeof(jsonValArray));
	a->cap = 8;
	a->arr = calloc(a->cap, sizeof(jsonVal));
	a->len = 0;
	return a;
}

static void addtoValueArray(jsonValArray *a, jsonVal *v) {
	if (a->len == a->cap) {
		a->cap *= 2;
		a->arr = realloc(a->arr, a->cap * sizeof(jsonVal));
		memset(&a->arr[a->len], 0, (a->len) * sizeof(jsonVal));
	}

	a->arr[a->len] = *v;
	a->len++;
}

char fgetcb(FILE *fp) {
	char c;
	while (1) {
		c = fgetc(fp);
		if (!strchr(" \t\n", c))
			break;
	}
	return c;
}

int mpow(int a, int n) {
	int s = 1;
	for (int i = 0; i < n; i++)
		s *= a;
	return s;
}

double fgetNumber(FILE *fp) {
	double n = 0;
	int dot = 0;
	int bits = 1;
	while (1) {
		char c = fgetc(fp);
		if (!strchr("1234567890.", c)) {
			ungetc(c, fp);
			break;
		}
		if (c == '.') {
			dot = 1;
			continue;
		}
		if (!dot) {
			n *= 10;
			n += c - '0';
		} else {
			double a = (double)mpow(10, bits);
			n += a;
			bits++;
		}
	}
	return n;
}

char *dump(const char *str) {
	char *n = malloc(strlen(str) + 1);
	strcpy(n, str);
	return n;
}

const char *fgetString(FILE *fp) {
	static char buf[256];
	memset(buf, 0, 256);
	fgetcb(fp); // first '"'
	int i = 0;
	while (1) {
		char c = fgetc(fp);
		if (c == '\"') {
			break;
		}
		if (c == '\\') {
			char n = fgetc(fp);
			if (n == 'b')
				buf[i++] = 0x8;
			else if (n == 't')
				buf[i++] = 0x9;
			else if (n == 'n')
				buf[i++] = 0xA;
			else if (n == 'f')
				buf[i++] = 0xC;
			else if (n == 'r')
				buf[i++] = 0xD;
			else
				buf[i++] = n;
		} else {
			buf[i++] = c;
		}
	}
	return buf;
}

const char *fgetLiteral(FILE *fp) {
	static char buf[8];
	memset(buf, 0, 8);
	fscanf(fp, "%s", buf);
	return buf;
}

jsonType lookType(FILE *fp) {

	char c = fgetcb(fp);
	ungetc(c, fp);
	if (c == 't' || c == 'f')
		return JSONT_BOO;
	if (c == 'n')
		return JSONT_NUL;
	if (strchr("0123456789.", c))
		return JSONT_NBR;
	if (c == '\"')
		return JSONT_STR;
	if (c == '[')
		return JSONT_ARR;
	if (c == '{')
		return JSONT_OBJ;
	return JSONT_NUL;
}

jsonVal *newValue(jsonType type, void *data) {
	jsonVal *v = calloc(1, sizeof(struct jsonVal));
	v->type = type;
	switch (v->type) {
	case JSONT_NUL:
		break;
	case JSONT_BOO:
		v->boolean = *(int*)data;
		break;
	case JSONT_NBR:
		v->number = *(double*)data;
		break;
	case JSONT_STR:
		v->string = data;
		break;
	case JSONT_ARR:
		fprintf(stderr, "newValue: Error");
		break;
	case JSONT_OBJ:
		v->object = data;
		break;
	}
	return v;
}

jsonVal *fgetValue(FILE *fp);

jsonObj *fgetObject(FILE *fp) {
	jsonObj *j = calloc(1, sizeof(jsonObj));
	while (1) {
		char c = fgetcb(fp); // '{' first time, ',' normal time, '}' last time
		if (c == '}')
			break;
		if (c == ',') {
			char n = fgetcb(fp);
			if (n == '}')
				break;
			else
				ungetc(n, fp);
		}
		char *key = dump(fgetString(fp));
		fgetcb(fp); // get colon ':'
		jsonVal *value = fgetValue(fp);
		addPair(j, key, value);
	}
	return j;
}

jsonVal *fgetValue(FILE *fp) {
	jsonType t = lookType(fp);

	const char *literal;
	double number;
	char *string;
	jsonObj *j;
	jsonValArray *a;

	switch (t) {
	case JSONT_NUL:
	case JSONT_BOO:
		literal = fgetLiteral(fp);
		if (strcmp(literal, "true") == 0 || strcmp(literal, "false") == 0) {
			int boolean = literal[0] == 't' ? 1 : 0;
			return newValue(JSONT_BOO, &boolean);
		} else if (strcmp(literal, "null") == 0) {
			return newValue(JSONT_NUL, NULL);
		}
	case JSONT_NBR:
		ungetc(fgetcb(fp), fp);
		number = fgetNumber(fp);
		return newValue(JSONT_NBR, &number);
	case JSONT_STR:
		string = dump(fgetString(fp));
		return newValue(JSONT_STR, string);
	case JSONT_OBJ:
		j = fgetObject(fp);
		return newValue(JSONT_OBJ, j);
	case JSONT_ARR:
		a = newValueArray();
		while (1) {
			char c = fgetcb(fp); // can be '[', ',' or ']'
			if (c == ']') {
				break;
			}
			if (c == ',') {
				char n = fgetcb(fp);
				if (n == ']')
					break;
				else
					ungetc(n, fp);
			}
			addtoValueArray(a, fgetValue(fp));
		}
		jsonVal *n = calloc(1, sizeof(jsonVal));
		n->type = JSONT_ARR;
		n->arrayLen = a->len;
		n->array = a->arr;
		free(a);
		return n;
	}
}

jsonObj *parseJson(const char *path) {

	FILE *fp = fopen(path, "r");
	if (fp == NULL) {
		fprintf(stderr, "json not exist: %s", path);
		return NULL;
	}

	return fgetObject(fp);
}

static void printObject(FILE *fp, jsonObj *obj, int indent);

static void printValue(FILE *fp, jsonVal *val, int indent) {
	double nf;
	int n;
	switch (val->type) {
	case JSONT_NUL:
		fprintf(fp, "null");
		break;
	case JSONT_BOO:
		fprintf(fp, "%s", val->number == 0 ? "false" : "true");
		break;
	case JSONT_NBR:
		nf = val->number;
		n = (int)nf;
		if (n == nf) {
			fprintf(fp, "%d", n);
		} else {
			fprintf(fp, "%.2f", nf);
		}
		break;
	case JSONT_STR:
		fprintf(fp, "\"%s\"", val->string);
		break;
	case JSONT_ARR:
		fprintf(fp, "[");
		if (val->array->type != JSONT_OBJ) {
			fprintf(fp, " ");
			for (int i = 0; i < val->arrayLen; i++) {
				printValue(fp, &val->array[i], indent + 1);
				if (i != val->arrayLen-1)
					fprintf(fp, ", ");
				else
					fprintf(fp, " ");
			}
		} else {
			fprintf(fp, "\n");
			for (int i = 0; i < val->arrayLen; i++) {
				for (int ii = 0; ii < indent + 2; ii++)
					fprintf(fp, "\t");
				printValue(fp, &val->array[i], indent + 1);
				fprintf(fp, ",\n");
			}
			for (int ii = 0; ii < indent + 1; ii++)
				fprintf(fp, "\t");
		}
		fprintf(fp, "]");
		break;
	case JSONT_OBJ:
		printObject(fp, val->object, indent + 1);
		break;
	}
}

static void printObject(FILE *fp, jsonObj *obj, int indent) {
	fprintf(fp, "{\n");
	int n;
	for (jsonPair *p = obj->pairs; p != NULL; p = p->next) {
		for (int ii = 0; ii < indent + 1; ii++)
			fprintf(fp, "\t");
		fprintf(fp, "\"%s\": ", p->key);
		printValue(fp, p->value, indent);
		fprintf(fp, ",\n");
	}
	for (int ii = 0; ii < indent; ii++)
		fprintf(fp, "\t");
	fprintf(fp, "}");
}

void outputJson(jsonObj *json, const char *path) {

	FILE *fp = stdout;

	printObject(fp, json, 0);
	fprintf(fp, "\n");
}

const jsonVal *jsonGetVal(const jsonObj *json, const char *keychain[], int keycount) {
	const jsonObj *current = json;
	jsonVal *t = NULL;
	for (int i = 0; i < keycount; i++) {
		const char *k = keychain[i];
		jsonPair *p = current->pairs;
		for (; p != NULL; p = p->next) {
			if (strcmp(p->key, k) == 0) {
				t = p->value;
				break;
			}
		}
		if (p == NULL) {
			fprintf(stderr, "jsonGet %s: key not found\n", keychain[i]);
			return NULL;
		}
		if (i < keycount - 1) {
			if (t->type != JSONT_OBJ) {
				fprintf(stderr, "jsonGet %s: Not an object\n", keychain[i]);
			}
			current = t->object;
		}
	}
	return t;
}

const jsonVal *jsonGetArr(const jsonObj *json, const char *keychain[], int keycount, int *count) {
	const jsonVal *v = jsonGetVal(json, keychain, keycount);
	if (count)
		*count = v->arrayLen;
	return v->array;
}


