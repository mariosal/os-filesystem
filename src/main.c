#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "meta_list.h"

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
  for (int i = 1; i < argc; ++i) {
    if (Equals(argv[i], "c")) {
    } else if (Equals(argv[i], "a")) {
    } else if (Equals(argv[i], "x")) {
    } else if (Equals(argv[i], "j")) {
      compress = true;
    } else if (Equals(argv[i], "m")) {
    } else if (Equals(argv[i], "q")) {
    } else if (Equals(argv[i], "p")) {
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

  struct MetaList* list;
  MetaListInit(&list);

  printf("archive: %s\n", archive);
  MetaListInsertHead(list, append, append_len, compress);
  MetaListPrintName(list);
  MetaListPrintMeta(list);
  MetaListReset(&list);

  for (size_t i = 0; i < append_len; ++i) {
    free(append[i]);
  }
  free(append);

  return EXIT_SUCCESS;
}
