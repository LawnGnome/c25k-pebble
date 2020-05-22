#include "activity.h"

#include <memory.h>

#include "programme.h"

struct ActivityWindow {
  time_t elapsed;
  bool active;
  const Programme* programme;

  Window* window;
  ActionBarLayer* action_bar;
  StatusBarLayer* status_bar;

  ActivityCallbacks callbacks;
};

ActivityWindow* activity_window_create(int8_t week,
                                       int8_t day,
                                       ActivityCallbacks callbacks) {
  ActivityWindow* activity = malloc(sizeof(ActivityWindow));
  Layer* root;

  activity->elapsed = 0;
  activity->active = false;
  activity->programme = programme_get(week, day);
  activity->callbacks = callbacks;

  activity->window = window_create();
  root = window_get_root_layer(activity->window);

  activity->action_bar = action_bar_layer_create();
  // TODO: build stuff.
  action_bar_layer_add_to_window(activity->action_bar, activity->window);

  activity->status_bar = status_bar_layer_create();
  layer_add_child(root, activity->status_bar);

  return activity;
}

void activity_window_destroy(ActivityWindow* activity) {
  action_bar_layer_destroy(activity->action_bar);
  status_bar_layer_destroy(activity->status_bar);
  window_destroy(activity->window);
  free(activity);
}

Window* activity_window_get_window(ActivityWindow* activity) {
  return activity->window;
}