#ifndef C25K_PEBBLE_H_INCLUDED
#define C25K_PEBBLE_H_INCLUDED

#include <pebble.h>

#define LOG_ERROR(M, ...) \
  app_log(APP_LOG_LEVEL_ERROR, __FILE__, __LINE__, (M), __VA_ARGS__)

#define LOG_DEBUG(M, ...) \
  app_log(APP_LOG_LEVEL_DEBUG, __FILE__, __LINE__, (M), __VA_ARGS__)

extern GRect calculate_bounds_with_status_action_bars(const Layer* root);
extern char* strdup(const char* str);

#endif