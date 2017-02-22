#include "file.h"
#include <stdlib.h>
#include <string.h>

void FileCreate(const char* path) {
  struct File file;
  long header_size = sizeof(file.cont_pos) + sizeof(file.meta_pos);
  file.cont_pos = file.meta_pos = header_size;

  char* buf = malloc(header_size);
  memcpy(buf, &file.cont_pos, sizeof(file.cont_pos));
  memcpy(buf + sizeof(file.cont_pos), &file.meta_pos, sizeof(file.meta_pos));

  file.f = fopen(path, "wb");
  fwrite(buf, sizeof(char), header_size, file.f);
  fclose(file.f);
}

void FileOpen(struct File** file, const char* path) {
  *file = malloc(sizeof(**file));
  if (*file == NULL) {
    exit(EXIT_FAILURE);
  }
}

void FileSave(struct File* file) {
}

void FileClose(struct File** file) {
  if (*file != NULL) {
    fclose((*file)->f);
    MetaListReset(&(*file)->meta);
    free(*file);
    *file = NULL;
  }
}
