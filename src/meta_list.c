#include "meta_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <dirent.h>

void MetaListInit(struct MetaList** list) {
  *list = malloc(sizeof(**list));
  if (*list == NULL) {
    exit(EXIT_FAILURE);
  }
  (*list)->size = 0;
  MetaNodeInit(&(*list)->head, NULL, 0, 0);
  (*list)->tail = (*list)->head;
}

void MetaListReset(struct MetaList** list) {
  if (*list != NULL) {
    struct MetaNode* it = (*list)->head;
    while (it != NULL) {
      struct MetaNode* tmp = it;
      it = it->next;
      MetaNodeReset(&tmp);
    }
    free(*list);
    *list = NULL;
  }
}

void MetaListInsertHead(struct MetaList* list, char** append, size_t append_len,
                        bool compress) {
  for (size_t i = 0; i < append_len; ++i) {
    DirListInsert(list->head->dir, append[i],
                  MetaListInsert(list, append[i], compress, list->head));
  }
}

const struct MetaNode* MetaListInsert(struct MetaList* list, const char* path,
                                      bool compress,
                                      const struct MetaNode* parent) {
  struct stat info;
  if (lstat(path, &info) != 0) {
    return NULL;
  }

  struct MetaNode* tmp;
  int cur_id = list->size + 1;
  MetaNodeInit(&tmp, &info, compress, cur_id);
  if (list->head == NULL) {
    list->head = tmp;
  } else {
    list->tail->next = tmp;
  }
  list->tail = tmp;

  char* newpath = malloc(sizeof(*newpath));
  newpath[0] = '\0';
  size_t newpath_cap = 1;
  if (S_ISDIR(info.st_mode)) {
    DIR* dir = opendir(path);
    struct dirent* child;
    while ((child = readdir(dir)) != NULL) {
      if (strcmp(child->d_name, ".") == 0) {
        DirListInsert(tmp->dir, child->d_name, tmp);
      } else if (strcmp(child->d_name, "..") == 0) {
        DirListInsert(tmp->dir, child->d_name, parent);
      } else {
        while (strlen(path) + 1 + strlen(child->d_name) > newpath_cap - 1) {
          newpath_cap *= 2;
          newpath = realloc(newpath, newpath_cap);
        }
        snprintf(newpath, newpath_cap, "%s/%s", path, child->d_name);
        DirListInsert(tmp->dir, child->d_name,
                      MetaListInsert(list, newpath, compress, tmp));
      }
    }
    closedir(dir);
  } else if (S_ISLNK(info.st_mode)) {
    readlink(path, tmp->link, sizeof(tmp->link) - 1);
  }
  free(newpath);
  return tmp;
}

void MetaListPrintName(struct MetaList* list) {
  DirListPrintName(list->head->dir, 0);
}

void MetaListPrintMeta(struct MetaList* list) {
  DirListPrintMeta(list->head->dir);
}

void MetaListQuery(struct MetaList* list, char** append, size_t append_len) {
  for (size_t i = 0; i < append_len; ++i) {
    //printf("%s: %d\n", append[i], DirListQuery(list->head->dir, append[i]));
  }
}
