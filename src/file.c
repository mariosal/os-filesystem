#include "file.h"
#include <stdlib.h>
#include <string.h>

void FileCreate(const char* path) {
  struct File file;
  long header_size = sizeof(file.cont_pos) + sizeof(file.meta_pos) + sizeof(file.dir_pos);
  file.cont_pos = file.meta_pos = header_size;
  file.dir_pos = header_size + sizeof(((struct MetaList*)NULL)->size);

  file.f = fopen(path, "wb");
  printf("%ld\n", file.cont_pos);
  fwrite(&file.cont_pos, 1, sizeof(file.cont_pos), file.f);
  fwrite(&file.meta_pos, 1, sizeof(file.meta_pos), file.f);
  fwrite(&file.dir_pos, 1, sizeof(file.dir_pos), file.f);
  int size = 0;
  fwrite(&size, 1, sizeof(size), file.f);
  fwrite(&size, 1, sizeof(size), file.f);
  fclose(file.f);
}

void FileOpen(struct File** file, const char* path) {
  *file = malloc(sizeof(**file));
  if (*file == NULL) {
    exit(EXIT_FAILURE);
  }
  long header_size = sizeof((*file)->cont_pos) + sizeof((*file)->meta_pos);

  (*file)->f = fopen(path, "r+b");
  fread(&(*file)->cont_pos, 1, sizeof((*file)->cont_pos), (*file)->f);
  fread(&(*file)->meta_pos, 1, sizeof((*file)->meta_pos), (*file)->f);
  fread(&(*file)->dir_pos, 1, sizeof((*file)->dir_pos), (*file)->f);
  fseek((*file)->f, 0, SEEK_END);
  long file_size = ftell((*file)->f);

  fseek((*file)->f, (*file)->meta_pos, SEEK_SET);

  (*file)->meta = malloc(sizeof(*(*file)->meta));
  (*file)->meta->size = 0;
  (*file)->meta->tail = (*file)->meta->head = NULL;

  fread(&(*file)->meta->size, 1, sizeof((*file)->meta->size), (*file)->f);
  struct MetaNode** map = malloc(sizeof(struct MetaNode*) * ((*file)->meta->size + 1));

  for (int i = 0; i <= (*file)->meta->size; ++i) {
    struct MetaNode* tmp = malloc(sizeof(*tmp));
    fread(&tmp->mode, 1, sizeof(tmp->mode), (*file)->f);
    fread(&tmp->uid, 1, sizeof(tmp->uid), (*file)->f);
    fread(&tmp->gid, 1, sizeof(tmp->gid), (*file)->f);
    fread(&tmp->size, 1, sizeof(tmp->size), (*file)->f);
    fread(tmp->time, 1, sizeof(tmp->time), (*file)->f);
    fread(&tmp->compress, 1, sizeof(tmp->compress), (*file)->f);
    fread(&tmp->id, 1, sizeof(tmp->id), (*file)->f);
    fread(tmp->link, 1, sizeof(tmp->link), (*file)->f);
    fread(&tmp->cont_pos, 1, sizeof(tmp->cont_pos), (*file)->f);
    DirListInit(&tmp->dir);
    tmp->next = NULL;

    if ((*file)->meta->head == NULL) {
      (*file)->meta->head = tmp;
    } else {
      (*file)->meta->tail->next = tmp;
    }
    (*file)->meta->tail = tmp;
    map[i] = tmp;
  }

  fseek((*file)->f, (*file)->dir_pos, SEEK_SET);

  struct MetaNode* it = (*file)->meta->head;
  while (it != NULL) {
    if (it == (*file)->meta->head || S_ISDIR(it->mode)) {
      int dir_size;
      fread(&dir_size, 1, sizeof(dir_size), (*file)->f);
      for (int i = 0; i < dir_size; ++i) {
        struct DirNode* tmp = malloc(sizeof(*tmp));
        fread(tmp->name, 1, sizeof(tmp->name), (*file)->f);
        int meta_id;
        fread(&meta_id, 1, sizeof(meta_id), (*file)->f);
        tmp->meta = map[meta_id];

        if (it->dir->head == NULL) {
          it->dir->head = tmp;
        } else {
          it->dir->tail->next = tmp;
        }
        it->dir->tail = tmp;
      }
    }
    it = it->next;
  }

  free(map);
}

void FileExtract(struct file* file) {
}

void FileSave(struct File* file) {
}

void FileClose(struct File** file) {
  if (*file != NULL) {
    fclose((*file)->f);
    MetaListReset(&((*file)->meta));
    free(*file);
    *file = NULL;
  }
}
