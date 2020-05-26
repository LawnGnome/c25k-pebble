#ifndef ACTIVITY_H_DEFINED
#define ACTIVITY_H_DEFINED

#include "c25k-pebble.h"
#include "programme.h"

typedef struct ActivityWindow ActivityWindow;

typedef struct {
  void (*on_back)(void* userdata);
  void* userdata;
} ActivityCallbacks;

extern ActivityWindow* activity_window_create(ActivityCallbacks callbacks);
extern void activity_window_destroy(ActivityWindow* activity);
extern Window* activity_window_get_window(ActivityWindow* activity);
extern void activity_window_set_active(ActivityWindow* activity, bool active);
extern void activity_window_set_programme(ActivityWindow* activity,
                                          const Programme* programme);

#endif