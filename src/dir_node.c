#include "dir_node.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void DirNodeInit(struct DirNode** node, const char* name, struct MetaNode* meta) {
  *node = malloc(sizeof(**node));
  if (*node == NULL) {
    exit(EXIT_FAILURE);
  }
  memset((*node)->name, 0, sizeof((*node)->name));
  snprintf((*node)->name, sizeof((*node)->name), "%s", name);
  (*node)->meta = meta;
  (*node)->next = NULL;
}

void DirNodeReset(struct DirNode** node) {
  free(*node);
  *node = NULL;
}
