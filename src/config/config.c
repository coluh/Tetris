#include "config.h"

#include "json.h"
#include "../common/utils.h"

static jsonObj *config;

void loadConfig(const char *path) {
	config = parseJson(path);
}

void freeConfig() {
	freeJson(config);
}

int getConfigInt(const char *keychain[], int keycount) {
	const jsonVal *v = jsonGetVal(config, keychain, keycount);
	return (int)v->number;
}

const char *getConfigString(const char *keychain[], int keycount) {
	const jsonVal *v = jsonGetVal(config, keychain, keycount);
	return v->string;
}

const ArrayInt getConfigArrayInt(const char *keychain[], int keycount) {
	static int buf[32];
	const jsonVal *v = jsonGetVal(config, keychain, keycount);
	Assert(v->type == JSONT_ARR, "Not an array");
	for (int i = 0; i < v->arrayLen; i++) {
		buf[i] = (int)v->array[i].number;
	}
	return newArrayInt(buf, v->arrayLen);
}

const ArrayString getConfigArrayString(const char *keychain[], int keycount) {
	static char *buf[32];
	const jsonVal *v = jsonGetVal(config, keychain, keycount);
	Assert(v->type == JSONT_ARR, "Not an array");
	for (int i = 0; i < v->arrayLen; i++) {
		buf[i] = v->array[i].string;
	}
	return (ArrayString) {
		.data = (char**)buf,
		.length = v->arrayLen,
	};
}




