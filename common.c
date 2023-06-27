#include "common.h"

const char *copy_string(const char *str) {
  size_t str_len = strlen(str);
  char *const new_str = malloc(str_len + 1);
  memcpy_s(new_str, str_len + 1, str, str_len + 1);
  return new_str;
}

void safe_memory_free(void **block) {
  if (block != NULL && *block != NULL) {
    free(*block);
    *block = NULL;
  }
}
