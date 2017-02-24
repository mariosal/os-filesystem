#ifndef META_NODE_H_
#define META_NODE_H_

#include "dir_list.h"
#include <stdbool.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct MetaNode {
  mode_t mode;
  uid_t uid;
  gid_t gid;
  off_t size;
  struct timespec time[2];
  bool compress;
  off_t com_size;
  int id;
  char link[4096];
  long cont_pos;
  struct DirList* dir;
  struct MetaNode* next;
};

void MetaNodeInit(struct MetaNode** node, const struct stat* info,
                  bool compress, int id);
void MetaNodeReset(struct MetaNode** node);

#endif  // META_NODE_H_
