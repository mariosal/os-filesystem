#include "dir_node.h"
#include <stdio.h>
#include <stdlib.h>

void DirNodeInit(struct DirNode** node, const char* name, const struct MetaNode* meta) {
  *node = malloc(sizeof(**node));
  if (*node == NULL) {
    exit(EXIT_FAILURE);
  }
  snprintf((*node)->name, sizeof((*node)->name), "%s", name);
  (*node)->meta = meta;
  (*node)->next = NULL;
}

void DirNodeReset(struct DirNode** node) {
  free(*node);
  *node = NULL;
}
