#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"

static bool Equals(const char* a, const char* b) {
  if (a[0] == '-') {
    return Equals(a + 1, b);
  }
  if (b[0] == '-') {
    return Equals(a, b + 1);
  }
  if (tolower(a[0]) == tolower(b[0])) {
    return a[0] == '\0' || Equals(a + 1, b + 1);
  }
  return false;
}

int main(int argc, char** argv) {
  setbuf(stdout, NULL);

  char archive[4096];
  char** append = NULL;
  size_t append_len = 0;
  bool compress = false;
  bool create = false;
  bool append_cmd = false;
  bool meta_print = false;
  bool query = false;
  bool name_print = false;
  bool extract = false;
  for (int i = 1; i < argc; ++i) {
    if (Equals(argv[i], "c")) {
      create = true;
    } else if (Equals(argv[i], "a")) {
      append_cmd = true;
    } else if (Equals(argv[i], "x")) {
      extract = true;
    } else if (Equals(argv[i], "j")) {
      compress = true;
    } else if (Equals(argv[i], "m")) {
      meta_print = true;
    } else if (Equals(argv[i], "q")) {
      query = true;
    } else if (Equals(argv[i], "p")) {
      name_print = true;
    } else {
      sscanf(argv[i], "%s", archive);
      ++i;
      append_len = argc - i;
      append = malloc(sizeof(*append) * append_len);
      for (size_t j = 0; j < append_len; ++j) {
        append[j] = malloc(sizeof(*append[j]) * 4096);
        sscanf(argv[i], "%s", append[j]);
        ++i;
      }
    }
  }

  if (create) {
    FileCreate(archive);
  }
  struct File* file;
  FileOpen(&file, archive);
  if (create || append_cmd) {
    MetaListInsertHead(file->meta, append, append_len, compress);
  }
  if (meta_print) {
    MetaListPrintMeta(file->meta);
  }
  if (name_print) {
    MetaListPrintName(file->meta);
  }
  if (query) {
    MetaListQuery(file->meta, append, append_len);
  }
  if (extract) {
    FileExtract(file, "output");
  }

  for (size_t i = 0; i < append_len; ++i) {
    free(append[i]);
  }
  free(append);

  if (create || append_cmd) {
    FileSave(file);
  }
  FileClose(&file);

  return EXIT_SUCCESS;
}
