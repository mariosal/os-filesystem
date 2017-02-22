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
  DirNodeInit(&(*list)->head, NULL, NULL);
  (*list)->tail = (*list)->head;
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

void DirListInsert(struct DirList* list, const char* name, const struct MetaNode* meta) {
  struct DirNode* tmp;
  DirNodeInit(&tmp, name, meta);
  if (list->head == NULL) {
    list->head = tmp;
  } else {
    list->tail->next = tmp;
  }
  list->tail = tmp;
}

void DirListPrintName(struct DirList* list, int lvl) {
  struct DirNode* it = list->head->next;
  while (it != NULL) {
    for (int i = 0; i < lvl; ++i) {
      printf("  ");
    }
    printf("%s\n", it->name);
    if (strcmp(it->name, ".") != 0 && strcmp(it->name, "..") != 0) {
      DirListPrintName(it->meta->dir, lvl + 1);
    }
    it = it->next;
  }
}

void DirListPrintMeta(struct DirList* list) {
  struct DirNode* it = list->head->next;
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
  if (list->head->next != NULL) {
    printf("\n");
  }
}

void DirListQuery(struct DirList* list, const char* path) {
  struct DirNode* it = list->head->next;
  char buf[100];
  while (it != NULL) {
    DirListQuery(it->meta->dir, path);
    it = it->next;
  }
}
