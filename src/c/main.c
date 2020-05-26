#include "c25k-pebble.h"
#include "activity.h"
#include "bitmap.h"
#include "selector.h"

static uint32_t PERSIST_KEY_WEEK = 1;
static uint32_t PERSIST_KEY_DAY = 2;

static ActivityWindow* activity;
static SelectorWindow* selector;

static void on_cancel(void* userdata) {
  window_stack_pop(true);
}

static void on_selector_submit(int8_t week, int8_t day, void* userdata) {
  persist_write_int(PERSIST_KEY_WEEK, week);
  persist_write_int(PERSIST_KEY_DAY, day);
  activity_window_set_programme(activity, programme_get(week, day));
  window_stack_push(activity_window_get_window(activity), true);
}

int main(void) {
  int32_t week, day;

  bitmap_init();

  week = (int8_t)persist_read_int(PERSIST_KEY_WEEK);
  day = (int8_t)persist_read_int(PERSIST_KEY_DAY);
  selector = selector_window_create(
      (SelectorCallbacks){
          .on_cancel = on_cancel,
          .on_submit = on_selector_submit,
      },
      week ? week : 1, day ? day : 1);

  activity = activity_window_create((ActivityCallbacks){
      .on_back = on_cancel,
  });

  window_stack_push(selector_window_get_window(selector), true);

  app_event_loop();

  activity_window_destroy(activity);
  selector_window_destroy(selector);
  bitmap_deinit();
}
