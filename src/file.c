#include "file.h"
#include <stdlib.h>
#include <string.h>

void FileCreate(const char* path) {
  struct File file;
  long header_size = sizeof(file.cont_pos) + sizeof(file.meta_pos) + sizeof(file.dir_pos);
  file.cont_pos = file.meta_pos = header_size;
  file.dir_pos = header_size + sizeof(((struct MetaList*)NULL)->size)
                 + sizeof(((struct MetaNode*)NULL)->mode)
                 + sizeof(((struct MetaNode*)NULL)->uid)
                 + sizeof(((struct MetaNode*)NULL)->gid)
                 + sizeof(((struct MetaNode*)NULL)->size)
                 + sizeof(((struct MetaNode*)NULL)->time)
                 + sizeof(((struct MetaNode*)NULL)->compress)
                 + sizeof(((struct MetaNode*)NULL)->id)
                 + sizeof(((struct MetaNode*)NULL)->link)
                 + sizeof(((struct MetaNode*)NULL)->cont_pos);

  file.f = fopen(path, "wb");
  fwrite(&file.cont_pos, 1, sizeof(file.cont_pos), file.f);
  fwrite(&file.meta_pos, 1, sizeof(file.meta_pos), file.f);
  fwrite(&file.dir_pos, 1, sizeof(file.dir_pos), file.f);
  int size = 0;
  fwrite(&size, 1, sizeof(size), file.f);
  struct MetaNode it;
  memset(&it, 0, sizeof(it));
  fwrite(&it.mode, 1, sizeof(it.mode), file.f);
  fwrite(&it.uid, 1, sizeof(it.uid), file.f);
  fwrite(&it.gid, 1, sizeof(it.gid), file.f);
  fwrite(&it.size, 1, sizeof(it.size), file.f);
  fwrite(it.time, 1, sizeof(it.time), file.f);
  fwrite(&it.compress, 1, sizeof(it.compress), file.f);
  fwrite(&it.id, 1, sizeof(it.id), file.f);
  fwrite(it.link, 1, sizeof(it.link), file.f);
  fwrite(&it.cont_pos, 1, sizeof(it.cont_pos), file.f);
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
        tmp->next = NULL;

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

static void FileExtractHelper(struct File* file, struct DirList* list, const char* path) {
  char* newpath = malloc(sizeof(*newpath));
  newpath[0] = '\0';
  size_t newpath_cap = 1;
  struct DirNode* it = list->head;
  while (it != NULL) {
    if (strcmp(it->name, ".") != 0 && strcmp(it->name, "..") != 0) {
      while (strlen(path) + 1 + strlen(it->name) > newpath_cap - 1) {
        newpath_cap *= 2;
        newpath = realloc(newpath, newpath_cap);
      }
      snprintf(newpath, newpath_cap, "%s/%s", path, it->name);

      char buf[1024];
      if (S_ISREG(it->meta->mode)) {
        FILE* out = fopen(newpath, "wb");
        fseek(file->f, it->meta->cont_pos, SEEK_SET);
        char* buf = malloc(it->meta->size);
        fread(buf, 1, it->meta->size, file->f);
        fwrite(buf, 1, it->meta->size, out);
        free(buf);
        fclose(out);
      } else if (S_ISDIR(it->meta->mode)) {
        snprintf(buf, sizeof(buf), "mkdir -p %s", newpath);
        system(buf);
        FileExtractHelper(file, it->meta->dir, newpath);
      } else if (S_ISLNK(it->meta->mode)) {
        snprintf(buf, sizeof(buf), "ln -s %s %s", it->meta->link, newpath);
        system(buf);
      }
    }
    it = it->next;
  }
  free(newpath);
}

void FileExtract(struct File* file, const char* path) {
  FileExtractHelper(file, file->meta->head->dir, path);
}

static void FileSaveHelper(struct File* file, struct DirList* list, const char* path) {
  char* newpath = malloc(sizeof(*newpath));
  newpath[0] = '\0';
  size_t newpath_cap = 1;
  struct DirNode* it = list->head;
  while (it != NULL) {
    if (strcmp(it->name, ".") != 0 && strcmp(it->name, "..") != 0) {
      while (strlen(path) + 1 + strlen(it->name) > newpath_cap - 1) {
        newpath_cap *= 2;
        newpath = realloc(newpath, newpath_cap);
      }
      snprintf(newpath, newpath_cap, "%s/%s", path, it->name);

      if (S_ISREG(it->meta->mode)) {
        FILE* in = fopen(newpath, "rb");
        fseek(in, 0, SEEK_END);
        long in_size = ftell(in);
        fseek(in, 0, SEEK_SET);
        char* buf = malloc(in_size);
        fread(buf, 1, in_size, in);
        it->meta->cont_pos = ftell(file->f);
        fwrite(buf, 1, in_size, file->f);
        free(buf);
        fclose(in);
      } else if (S_ISDIR(it->meta->mode)) {
        FileSaveHelper(file, it->meta->dir, newpath);
      }
    }
    it = it->next;
  }
  free(newpath);
}

void FileSave(struct File* file) {
  fseek(file->f, file->cont_pos, SEEK_SET);
  FileSaveHelper(file, file->meta->head->dir, ".");

  file->meta_pos = ftell(file->f);
  fwrite(&file->meta->size, 1, sizeof(file->meta->size), file->f);

  struct MetaNode* it = file->meta->head;
  while (it != NULL) {
    fwrite(&it->mode, 1, sizeof(it->mode), file->f);
    fwrite(&it->uid, 1, sizeof(it->uid), file->f);
    fwrite(&it->gid, 1, sizeof(it->gid), file->f);
    fwrite(&it->size, 1, sizeof(it->size), file->f);
    fwrite(it->time, 1, sizeof(it->time), file->f);
    fwrite(&it->compress, 1, sizeof(it->compress), file->f);
    fwrite(&it->id, 1, sizeof(it->id), file->f);
    fwrite(it->link, 1, sizeof(it->link), file->f);
    fwrite(&it->cont_pos, 1, sizeof(it->cont_pos), file->f);
    it = it->next;
  }

  file->dir_pos = ftell(file->f);
  it = file->meta->head;
  while (it != NULL) {
    if (it == file->meta->head || S_ISDIR(it->mode)) {
      fwrite(&it->dir->size, 1, sizeof(it->dir->size), file->f);
      struct DirNode* dit = it->dir->head;
      while (dit != NULL) {
        fwrite(dit->name, 1, sizeof(dit->name), file->f);
        fwrite(&dit->meta->id, 1, sizeof(dit->meta->id), file->f);
        dit = dit->next;
      }
    }
    it = it->next;
  }

  fseek(file->f, 0, SEEK_SET);
  fwrite(&file->cont_pos, 1, sizeof(file->cont_pos), file->f);
  fwrite(&file->meta_pos, 1, sizeof(file->meta_pos), file->f);
  fwrite(&file->dir_pos, 1, sizeof(file->dir_pos), file->f);
}

void FileClose(struct File** file) {
  if (*file != NULL) {
    fclose((*file)->f);
    MetaListReset(&((*file)->meta));
    free(*file);
    *file = NULL;
  }
}
