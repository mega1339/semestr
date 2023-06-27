#pragma once

#include <string.h>
#include <stdlib.h>

#ifndef SAFE_FREE
#define SAFE_FREE(p) safe_memory_free((void**)&(p));
#endif

const char *copy_string(const char *str);
void safe_memory_free(void **block);
