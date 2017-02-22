#ifndef META_LIST_H_
#define META_LIST_H_

#include "meta_node.h"

struct MetaList {
  int size;
  struct MetaNode* head;
  struct MetaNode* tail;
};

void MetaListInit(struct MetaList** list);
void MetaListReset(struct MetaList** list);
void MetaListInsertHead(struct MetaList* list, char** append, size_t append_len,
                        bool compress);
struct MetaNode* MetaListInsert(struct MetaList* list, const char* path,
                                bool compress,
                                struct MetaNode* parent);
void MetaListPrintName(struct MetaList* list);
void MetaListPrintMeta(struct MetaList* list);
void MetaListQuery(struct MetaList* list, char** append, size_t append_len);


#endif  // META_LIST_H_
