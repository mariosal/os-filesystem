#ifndef DIR_NODE_H_
#define DIR_NODE_H_

struct DirNode {
  char name[4096];
  const struct MetaNode* meta;
  struct DirNode* next;
};

void DirNodeInit(struct DirNode** node, const char* name, const struct MetaNode* meta);
void DirNodeReset(struct DirNode** node);

#endif  // DIR_NODE_H_
