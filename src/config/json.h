#ifndef TETRIS_JSON_H
#define TETRIS_JSON_H

typedef struct jsonObj jsonObj;
typedef struct jsonVal jsonVal;

jsonObj *parseJson(const char *path);
void outputJson(jsonObj *json, const char *path);

// jsonVal *jsonGetVal(jsonObj *json, const char *keychain[], int keycount);

int jsonGetInt(jsonObj *json, const char *keychain[], int keycount);
double jsonGetNbr(jsonObj *json, const char *keychain[], int keycount);
const char *jsonGetStr(jsonObj *json, const char *keychain[], int keycount);

// send your pointer here, I'll fill it
int jsonGetArrI(jsonObj *json, const char *keychain[], int keycount, int p[]);

#endif
