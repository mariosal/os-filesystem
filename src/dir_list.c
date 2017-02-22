#include "dir_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "meta_node.h"

void DirListInit(struct DirList** list) {
  *list = malloc(sizeof(**list));
  if (*list == NULL) {
    exit(EXIT_FAILURE);
  }
  (*list)->size = 0;
  (*list)->tail = (*list)->head = NULL;
}

void DirListReset(struct DirList** list) {
  if (*list != NULL) {
    struct DirNode* it = (*list)->head;
    while (it != NULL) {
      struct DirNode* tmp = it;
      it = it->next;
      DirNodeReset(&tmp);
    }
    free(*list);
    *list = NULL;
  }
}

void DirListInsert(struct DirList* list, const char* name, struct MetaNode* meta) {
  struct DirNode* tmp;
  DirNodeInit(&tmp, name, meta);
  if (list->head == NULL) {
    list->head = tmp;
  } else {
    list->tail->next = tmp;
  }
  list->tail = tmp;
  ++list->size;
}

void DirListPrintName(struct DirList* list, int lvl) {
  struct DirNode* it = list->head;
  while (it != NULL) {
    if (strcmp(it->name, ".") != 0 && strcmp(it->name, "..") != 0) {
      for (int i = 0; i < lvl; ++i) {
        printf("  ");
      }
      printf("%s\n", it->name);
      DirListPrintName(it->meta->dir, lvl + 1);
    }
    it = it->next;
  }
}

void DirListPrintMeta(struct DirList* list) {
  struct DirNode* it = list->head;
  char buf[100];
  while (it != NULL) {
    if (it->meta->id != 0) {
      strftime(buf, sizeof(buf), "%D %T", localtime(&(it->meta->time[0]).tv_sec));
      printf("Perm: %o User: %d Group: %d Size: %d Modified: %s Name: %s",
             it->meta->mode & 0777, it->meta->uid, it->meta->gid, it->meta->size,
             buf, it->name);
      if (it->meta->link[0] != '\0') {
        printf(" -> %s", it->meta->link);
      }
      printf("\n");
      if (strcmp(it->name, ".") != 0 && strcmp(it->name, "..") != 0) {
        DirListPrintMeta(it->meta->dir);
      }
    }
    it = it->next;
  }
  if (list->head != NULL) {
    printf("\n");
  }
}

bool DirListQuery(struct DirList* list, const char* path) {
  if (path[0] == '\0') {
    return true;
  }
  struct DirNode* it = list->head;
  int len = strlen(path);
  char* first = malloc(strlen(path) + 1);
  int f_len = 0;
  char* second = malloc(strlen(path) + 1);
  int s_len = 0;
  int i;
  for (i = 0; i < len; ++i) {
    if (path[i] == '/') {
      break;
    }
    first[i] = path[i];
    ++f_len;
  }
  first[f_len] = 0;
  for (++i; i < len; ++i) {
    second[s_len] = path[i];
    ++s_len;
  }
  second[s_len] = 0;
  bool found = false;
  while (it != NULL) {
    if (strcmp(it->name, first) == 0) {
      found = DirListQuery(it->meta->dir, second);
    }
    it = it->next;
  }
  free(first);
  free(second);
  return found;
}
