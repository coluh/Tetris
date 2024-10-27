#ifndef __INT_MAP__
#define __INT_MAP__

#include <stdbool.h>

typedef struct IntMap IntMap;

typedef struct OptionInt {
	int data;
	bool exist;
} OptionInt;

IntMap *newIntMap();
OptionInt getIntMap(IntMap *m, int key);
void insertIntMap(IntMap *m, int key, int value);
int getIntMapSize(IntMap *m);
const int *traverseIntMap(IntMap *m, int *n);
void freeIntMap(IntMap *m);

#endif // __INT_MAP__
