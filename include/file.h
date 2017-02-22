#ifndef FILE_H_
#define FILE_H_

#include "meta_list.h"
#include <stdio.h>

struct File {
  FILE* f;
  long cont_pos;
  long meta_pos;
  long dir_pos;
  struct MetaList* meta;
};

void FileCreate(const char* path);
void FileOpen(struct File** file, const char* path);
void FileSave(struct File* file);
void FileClose(struct File** file);

#endif  // FILE_H_
