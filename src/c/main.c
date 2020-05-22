#include "c25k-pebble.h"
#include "selector.h"

static void on_cancel(void* userdata) {
  window_stack_pop(true);
}

static void on_submit(int8_t week, int8_t day, void* userdata) {
  LOG_DEBUG("%s: %d %d", __func__, week, day);
}

int main(void) {
  // TODO: pull initial state from a cache.
  SelectorWindow* selector = selector_window_create(
      (SelectorCallbacks){
          .on_cancel = on_cancel,
          .on_submit = on_submit,
      },
      1, 1);

  window_stack_push(selector_window_get_window(selector), true);

  app_event_loop();

  selector_window_destroy(selector);
}
