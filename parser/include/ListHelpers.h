#ifndef LISTHELPERS_H
#define LISTHELPERS_H

#include "LinkedListAPI.h"

void append_list_to_list(List *list, List *listToAdd);
void free_list_keep_data(List *list);
List* find_elements(List * list, bool (*customCompare)(const void* first,const void* second), char* (*printFunction)(void* toBePrinted),void (*deleteFunction)(void* toBeDeleted),int (*compareFunction)(const void* first,const void* second), const void* searchRecord);

#endif

