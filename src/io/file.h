#pragma once
#include <stdlib.h>

char *magmatis_executable_directory_path_get();
char *magmatis_resource_file_path_get(const char *filename);
char *magmatis_resource_file_read(const char *filename, size_t *size);

#ifndef USE_LONG_NAMES
#define resource_file_read magmatis_resource_file_read
#define executable_directory_path_get magmatis_executable_directory_path_get
#define resource_file_path_get magmatis_resource_file_path_get
#endif