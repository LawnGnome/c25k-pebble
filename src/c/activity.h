#ifndef ACTIVITY_H_DEFINED
#define ACTIVITY_H_DEFINED

#include "c25k-pebble.h"

typedef struct ActivityWindow ActivityWindow;

typedef struct {
  void (*on_back)(void* userdata);
  void* userdata;
} ActivityCallbacks;

extern ActivityWindow* activity_window_create(int8_t week,
                                              int8_t day,
                                              ActivityCallbacks callbacks);
extern void activity_window_destroy(ActivityWindow* activity);
extern Window* activity_window_get_window(ActivityWindow* activity);

#endif