#include "pebble.h"

#include <memory.h>

char* strdup(const char* str) {
  const size_t len = strlen(str);
  char* out = malloc(len + 1);

  memcpy(out, str, len + 1);

  return out;
}
