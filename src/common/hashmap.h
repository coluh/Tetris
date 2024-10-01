#ifndef __HASH_MAP__
#define __HASH_MAP__

typedef struct HashMap HashMap;

HashMap *newHashMap(int v_size);
void insertHashMap(HashMap *m, const char *key, void *value);
void *findHashMap(HashMap *m, const char *key);
void freeHashMap(HashMap *m);

#endif // __HASH_MAP__
