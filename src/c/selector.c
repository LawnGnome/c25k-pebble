#include "selector.h"

#include <memory.h>

#include "bitmap.h"
#include "number.h"

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
  ActionBarLayer* action_bar;
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

  if (selector->active == SELECTOR_WEEK) {
    number_layer_set_active(selector->week, true);
    number_layer_set_active(selector->day, false);
    action_bar_layer_set_icon_animated(selector->action_bar, BUTTON_ID_SELECT,
                                       image_arrow_right, true);
  } else {
    number_layer_set_active(selector->week, false);
    number_layer_set_active(selector->day, true);
    action_bar_layer_set_icon_animated(selector->action_bar, BUTTON_ID_SELECT,
                                       image_tick, true);
  }
}

static void on_button_back(ClickRecognizerRef ref, void* ctx) {
  SelectorWindow* selector = (SelectorWindow*)ctx;

  if (selector->active == SELECTOR_DAY) {
    selector_window_set_active(selector, SELECTOR_WEEK);
  } else {
    (selector->callbacks.on_cancel)(selector->callbacks.userdata);
  }
}

static void on_button_down(ClickRecognizerRef ref, void* ctx) {
  SelectorWindow* selector = (SelectorWindow*)ctx;

  number_layer_decrement_value(selector_window_get_active(selector));
}

static void on_button_select(ClickRecognizerRef ref, void* ctx) {
  SelectorWindow* selector = (SelectorWindow*)ctx;

  if (selector->active == SELECTOR_WEEK) {
    selector_window_set_active(selector, SELECTOR_DAY);
  } else {
    (selector->callbacks.on_submit)(number_layer_get_value(selector->week),
                                    number_layer_get_value(selector->day),
                                    selector->callbacks.userdata);
  }
}

static void on_button_up(ClickRecognizerRef ref, void* ctx) {
  SelectorWindow* selector = (SelectorWindow*)ctx;

  number_layer_increment_value(selector_window_get_active(selector));
}

static void click_config_provider(void* ctx) {
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
  GRect bounds = calculate_bounds_with_status_action_bars(root);
  GSize number_layer_size;

  selector->action_bar = action_bar_layer_create();
  action_bar_layer_set_icon(selector->action_bar, BUTTON_ID_UP, image_arrow_up);
  action_bar_layer_set_icon(selector->action_bar, BUTTON_ID_DOWN,
                            image_arrow_down);
  action_bar_layer_set_icon(selector->action_bar, BUTTON_ID_SELECT,
                            image_arrow_right);
  action_bar_layer_set_context(selector->action_bar, selector);
  action_bar_layer_set_click_config_provider(selector->action_bar,
                                             click_config_provider);
  action_bar_layer_add_to_window(selector->action_bar, window);

  selector->status_bar = status_bar_layer_create();
  layer_add_child(root, status_bar_layer_get_layer(selector->status_bar));

  // Adjust the bounds we use to lay out the number layers for the fixed
  // elements.
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

  action_bar_layer_destroy(selector->action_bar);
  status_bar_layer_destroy(selector->status_bar);
  number_layer_destroy(selector->week);
  number_layer_destroy(selector->day);
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

  window_set_window_handlers(selector->window, (WindowHandlers){
                                                   .load = on_load,
                                                   .unload = on_unload,
                                                   .appear = on_appear,
                                                   .disappear = on_disappear,
                                               });

  return selector;
}

void selector_window_destroy(SelectorWindow* selector) {
  window_destroy(selector->window);
  free(selector);
}

Window* selector_window_get_window(SelectorWindow* selector) {
  return selector->window;
}
