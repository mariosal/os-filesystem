#include "meta_node.h"
#include <stdlib.h>
#include <string.h>

void MetaNodeInit(struct MetaNode** node, const struct stat* info,
                  bool compress, int id) {
  *node = malloc(sizeof(**node));
  if (*node == NULL) {
    exit(EXIT_FAILURE);
  }
  if (info != NULL) {
    (*node)->mode = info->st_mode;
    (*node)->uid = info->st_uid;
    (*node)->gid = info->st_gid;
    (*node)->size = info->st_size;
    (*node)->time[0] = info->st_atim;
    (*node)->time[1] = info->st_mtim;
  } else {
    (*node)->mode = 0;
    (*node)->uid = 0;
    (*node)->gid = 0;
    (*node)->size = 0;
    memset((*node)->time, 0, sizeof((*node)->time));
  }
  (*node)->compress = compress;
  (*node)->id = id;
  memset((*node)->link, 0, sizeof((*node)->link));
  DirListInit(&(*node)->dir);
  (*node)->cont_pos = 0;
  (*node)->next = NULL;
}

void MetaNodeReset(struct MetaNode** node) {
  if (*node != NULL) {
    DirListReset(&(*node)->dir);
    free(*node);
    *node = NULL;
  }
}
