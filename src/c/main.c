#include "c25k-pebble.h"
#include "activity.h"
#include "bitmap.h"
#include "selector.h"

static ActivityWindow* activity;
static SelectorWindow* selector;

static void on_cancel(void* userdata) {
  window_stack_pop(true);
}

static void on_selector_submit(int8_t week, int8_t day, void* userdata) {
  activity_window_set_programme(activity, programme_get(week, day));
  window_stack_push(activity_window_get_window(activity), true);
}

int main(void) {
  SelectorWindow* selector;

  bitmap_init();

  // TODO: pull initial state from a cache.
  selector = selector_window_create(
      (SelectorCallbacks){
          .on_cancel = on_cancel,
          .on_submit = on_selector_submit,
      },
      1, 1);

  activity = activity_window_create((ActivityCallbacks){
      .on_back = on_cancel,
  });

  window_stack_push(selector_window_get_window(selector), true);

  app_event_loop();

  activity_window_destroy(activity);
  selector_window_destroy(selector);
  bitmap_deinit();
}
