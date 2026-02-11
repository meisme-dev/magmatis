#include "file.h"
#include <extra/colors.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <whereami.h>

static off_t file_size_get(char *filename) {
  struct stat st;

  if (stat(filename, &st) == 0) {
    return st.st_size;
  } else {
    perror("Error determining size of file");
    return -1;
  }
}

char *magmatis_resource_file_path_get(const char *filename) {
  char *path = magmatis_executable_directory_path_get();

  strcat(path, filename);

  return path;
}

char *magmatis_executable_directory_path_get() {
  int dirname_length = 0;
  int length = 0;

  length = wai_getExecutablePath(NULL, 0, NULL);
  char *path = calloc(length + 2, sizeof(char));

  wai_getExecutablePath(path, length, &dirname_length);

  path[dirname_length] = '\0';

  strcat(path, "/");

  return path;
}

char *magmatis_resource_file_read(const char *filename, size_t *size) {
  char *path = magmatis_resource_file_path_get(filename);

  FILE *file = fopen(path, "rb");

  if (!file) {
    fprintf(stderr, "%sFailed to open file: %s%s\n", RED, path, CLEAR);
    return NULL;
  }

  off_t file_size = file_size_get(path);

  if (file_size == -1) {
    fprintf(stderr, "%sFailed to get file size: %s%s\n", RED, path, CLEAR);
    fclose(file);
    free(path);
    return NULL;
  }

  char *file_contents = calloc(file_size, sizeof(char));

  if (!file_contents) {
    fprintf(stderr, "%sFailed to allocate memory for file contents: %s%s\n",
            RED, path, CLEAR);
    fclose(file);
    free(path);
    return NULL;
  }

  fread(file_contents, 1, file_size, file);

  if (ferror(file)) {
    perror("Error reading file");
  }

  fclose(file);
  free(path);

  *size = file_size;

  return file_contents;
}