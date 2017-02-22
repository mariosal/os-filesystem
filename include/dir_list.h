#ifndef DIR_LIST_H_
#define DIR_LIST_H_

#include "dir_node.h"
#include <stdbool.h>

struct DirList {
  int size;
  struct DirNode* head;
  struct DirNode* tail;
};

void DirListInit(struct DirList** list);
void DirListReset(struct DirList** list);
void DirListInsert(struct DirList* list, const char* name, const struct MetaNode* meta);
void DirListPrintName(struct DirList* list, int lvl);
void DirListPrintMeta(struct DirList* list);
bool DirListQuery(struct DirList* list, const char* path);

#endif  // META_LIST_H_
