#pragma once
#include <stdlib.h>

char *magmatis_resource_file_read(char *filename, size_t *size);

#ifndef USE_LONG_NAMES
#define resource_file_read magmatis_resource_file_read
#endif