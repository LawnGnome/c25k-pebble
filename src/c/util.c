#include "c25k-pebble.h"

#include <memory.h>

GRect calculate_bounds_with_status_action_bars(const Layer* root) {
  GRect bounds = layer_get_unobstructed_bounds(root);

  bounds.origin.y += STATUS_BAR_LAYER_HEIGHT;
  bounds.size.h -= STATUS_BAR_LAYER_HEIGHT;
  bounds.size.w -= ACTION_BAR_WIDTH;

  return bounds;
}

char* strdup(const char* str) {
  const size_t len = strlen(str);
  char* out = malloc(len + 1);

  memcpy(out, str, len + 1);

  return out;
}
