#ifndef SELECTOR_H_INCLUDED
#define SELECTOR_H_INCLUDED

#include "c25k-pebble.h"

typedef struct _selector_t SelectorWindow;

typedef struct {
  void (*on_cancel)(void* userdata);
  void (*on_submit)(int8_t week, int8_t day, void* userdata);
  void* userdata;
} SelectorCallbacks;

extern SelectorWindow* selector_window_create(SelectorCallbacks callbacks,
                                              int8_t week,
                                              int8_t day);
extern void selector_window_destroy(SelectorWindow* selector);
extern Window* selector_window_get_window(SelectorWindow* selector);

#endif