#include "selector.h"
#include "number.h"

#include <memory.h>

static const uint16_t REPEAT_INTERVAL_MS = 250;

static const int16_t PADDING_SIZE = 10;

typedef enum {
  SELECTOR_WEEK,
  SELECTOR_DAY,
} selector_control_t;

typedef struct {
  int8_t week;
  int8_t day;
} selector_state_t;

struct _selector_t {
  Window* window;
  StatusBarLayer* status_bar;
  NumberLayer* week;
  NumberLayer* day;

  selector_state_t initial;
  selector_control_t active;

  SelectorCallbacks callbacks;
};

static inline SelectorWindow* selector_retrieve(const Window* window) {
  return (SelectorWindow*)window_get_user_data(window);
}

static NumberLayer* selector_window_get_active(SelectorWindow* selector) {
  switch (selector->active) {
    case SELECTOR_WEEK:
      return selector->week;

    case SELECTOR_DAY:
      return selector->day;

    default:
      LOG_ERROR("unexpected selector active control: %d",
                (int)selector->active);
      return selector->week;
  }
}

static void selector_window_set_active(SelectorWindow* selector,
                                       selector_control_t control) {
  selector->active = control;
  number_layer_set_active(selector->week, selector->active == SELECTOR_WEEK);
  number_layer_set_active(selector->day, selector->active == SELECTOR_DAY);
}

static void on_button_back(ClickRecognizerRef ref, void* ctx) {
  SelectorWindow* selector = selector_retrieve(ctx);

  if (selector->active == SELECTOR_DAY) {
    selector_window_set_active(selector, SELECTOR_WEEK);
  } else {
    (selector->callbacks.on_cancel)(selector->callbacks.userdata);
  }
}

static void on_button_down(ClickRecognizerRef ref, void* ctx) {
  SelectorWindow* selector = selector_retrieve(ctx);

  number_layer_decrement_value(selector_window_get_active(selector));
}

static void on_button_select(ClickRecognizerRef ref, void* ctx) {
  SelectorWindow* selector = selector_retrieve(ctx);

  if (selector->active == SELECTOR_WEEK) {
    selector_window_set_active(selector, SELECTOR_DAY);
  } else {
    (selector->callbacks.on_submit)(number_layer_get_value(selector->week),
                                    number_layer_get_value(selector->day),
                                    selector->callbacks.userdata);
  }
}

static void on_button_up(ClickRecognizerRef ref, void* ctx) {
  SelectorWindow* selector = selector_retrieve(ctx);

  number_layer_increment_value(selector_window_get_active(selector));
}

static void click_config_provider(void* ctx) {
  Window* window = (Window*)ctx;

  LOG_DEBUG("%s: window: %p; selector: %p", __func__, window,
            window_get_user_data(window));

  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, REPEAT_INTERVAL_MS,
                                          on_button_down);
  window_single_repeating_click_subscribe(BUTTON_ID_UP, REPEAT_INTERVAL_MS,
                                          on_button_up);
  window_single_click_subscribe(BUTTON_ID_BACK, on_button_back);
  window_single_click_subscribe(BUTTON_ID_SELECT, on_button_select);
}

static void on_appear(Window* window) {
  SelectorWindow* selector = selector_retrieve(window);

  selector_window_set_active(selector, SELECTOR_WEEK);
}

static void on_disappear(Window* window) {}

static void on_load(Window* window) {
  SelectorWindow* selector = selector_retrieve(window);
  Layer* root = window_get_root_layer(window);
  GRect bounds = layer_get_unobstructed_bounds(root);
  GSize number_layer_size;

  LOG_DEBUG("%s: window: %p; selector: %p", __func__, window, selector);
  LOG_DEBUG("%s: unobstructed bounds: origin: %d,%d; size: %d,%d", __func__,
            bounds.origin.x, bounds.origin.y, bounds.size.w, bounds.size.h);

  selector->status_bar = status_bar_layer_create();
  layer_add_child(root, status_bar_layer_get_layer(selector->status_bar));
  bounds.size.h -= STATUS_BAR_LAYER_HEIGHT;
  bounds.origin.y = STATUS_BAR_LAYER_HEIGHT;
  number_layer_size = (GSize){
      .w = (bounds.size.w / 2) - (PADDING_SIZE * 1.5),
      .h = (bounds.size.h) - (PADDING_SIZE * 2),
  };

  selector->week = number_layer_create(
      (GRect){
          .origin = {bounds.origin.x + PADDING_SIZE,
                     bounds.origin.y + PADDING_SIZE},
          .size = number_layer_size,
      },
      "Week", 1, 9, selector->initial.week);
  layer_add_child(root, number_layer_get_layer(selector->week));

  LOG_DEBUG("%s: number_layer_size: %d,%d", __func__, number_layer_size.w,
            number_layer_size.h);
  selector->day = number_layer_create(
      (GRect){
          .origin = {(bounds.size.w / 2) + (0.5 * PADDING_SIZE),
                     bounds.origin.y + PADDING_SIZE},
          .size = number_layer_size,
      },
      "Day", 1, 3, selector->initial.day);
  layer_add_child(root, number_layer_get_layer(selector->day));
}

static void on_unload(Window* window) {
  SelectorWindow* selector = selector_retrieve(window_get_user_data(window));

  status_bar_layer_destroy(selector->status_bar);
  number_layer_destroy(selector->week);
  number_layer_destroy(selector->day);

  LOG_DEBUG("on_unload: %p", window);
}

SelectorWindow* selector_window_create(SelectorCallbacks callbacks,
                                       int8_t week,
                                       int8_t day) {
  SelectorWindow* selector = malloc(sizeof(SelectorWindow));

  selector->callbacks = callbacks;
  selector->initial.week = week;
  selector->initial.day = day;

  selector->window = window_create();
  window_set_user_data(selector->window, selector);
  LOG_DEBUG("%s, window: %p; selector: %p", __func__, selector->window,
            selector);

  window_set_click_config_provider(selector->window, click_config_provider);

  window_set_window_handlers(selector->window, (WindowHandlers){
                                                   .load = on_load,
                                                   .unload = on_unload,
                                                   .appear = on_appear,
                                                   .disappear = on_disappear,
                                               });

  return selector;
}

void selector_window_destroy(SelectorWindow* selector) {
  LOG_DEBUG("%s, window: %p; selector: %p", __func__, selector->window,
            selector);

  window_destroy(selector->window);
  free(selector);
}

Window* selector_window_get_window(SelectorWindow* selector) {
  LOG_DEBUG("%s, window: %p; selector: %p", __func__, selector->window,
            selector);

  return selector->window;
}
