#ifndef __ARRAYLIST_H__
#define __ARRAYLIST_H__

typedef struct ArrayList List;

List *newList(int unit_size);
void listAdd(List *list, void *entry);
// delete list[index]
void listDelete(List *list, int index);
const void *listGet(List *list, int index);
int listLength(List *list);
void freeList(List *list);

#endif // __ARRAYLIST_H__
