#ifndef TETRIS_JSON_H
#define TETRIS_JSON_H

typedef enum jsonType {
	JSONT_NUL,
	JSONT_BOO,
	JSONT_NBR,
	JSONT_STR,
	JSONT_ARR,
	JSONT_OBJ,
} jsonType;

typedef struct jsonObj jsonObj;
typedef struct jsonVal {
	jsonType type;
	int arrayLen;  // if needed
	union {
		int boolean;
		double number;
		char *string;
		struct jsonVal *array;
		jsonObj *object;
	};
} jsonVal;

jsonObj *parseJson(const char *path);
void freeJson(jsonObj *json);
void outputJson(jsonObj *json, const char *path);

const jsonVal *jsonGetVal(const jsonObj *json, const char *keychain[], int keycount);
const jsonVal *jsonGetArr(const jsonObj *json, const char *keychain[], int keycount, int *count);

#endif
