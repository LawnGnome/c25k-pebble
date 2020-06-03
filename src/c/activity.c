#include "activity.h"

#include <memory.h>

#include "bitmap.h"
#include "programme.h"
#include "reward.h"

static const uint32_t MARKER_DEGREES = 2;
static const int16_t MARKER_SIZE = 16;
static const int16_t PHASE_HEIGHT = 18;
static const uint32_t TIMER_TIMEOUT_MS = 500;

#ifdef PBL_ROUND
static const int16_t PADDING_SIZE = 30;
static const int16_t RADIAL_WIDTH = 10;
#else
static const int16_t PADDING_SIZE = 5;
static const int16_t RADIAL_WIDTH = 10;
#endif

typedef enum {
  ACTIVITY_ACTIVE,
  ACTIVITY_PAUSED,
  ACTIVITY_COMPLETE,
} ActivityState;

struct ActivityWindow {
  time_t elapsed;
  time_t started_at;
  ActivityState state;
  AppTimer* timer;

  const Programme* programme;

  Window* window;
  Layer* gfx;
  TextLayer* phase;
  StatusBarLayer* status_bar;
  TextLayer* time_remaining;
  RewardWindow* reward;

#ifndef PBL_ROUND
  ActionBarLayer* action_bar;
#endif

  char phase_buffer[10];
  char time_remaining_buffer[24];

  ActivityCallbacks callbacks;
};

typedef struct {
  const GRect bounds;
  GContext* ctx;
  const time_t total_duration;
} StateIteratorData;

static GColor state_colour(ProgrammeState state) {
  switch (state) {
    case PROGRAMME_STATE_WARM_UP:
    case PROGRAMME_STATE_WARM_DOWN:
      return GColorLightGray;

    case PROGRAMME_STATE_WALK:
      return GColorDarkGray;

    case PROGRAMME_STATE_RUN:
      // The other colours in here are fine on the B&W Pebble screens, but the
      // green gets turned to white, which isn't really useful. We'll do what we
      // did with the number selector and instead use black to highlight.
      return COLOR_FALLBACK(GColorBrightGreen, GColorBlack);

    default:
      LOG_ERROR("unexpected programme state: %d", (int)state);
      return GColorRed;
  }
}

static int32_t calculate_angle(time_t at, time_t total_duration) {
  return (TRIG_MAX_RATIO * at) / total_duration;
}

static void on_state(time_t at,
                     time_t phase_duration,
                     ProgrammeState state,
                     void* userdata) {
  StateIteratorData* data = (StateIteratorData*)userdata;

  graphics_context_set_fill_color(data->ctx, state_colour(state));
  graphics_fill_radial(
      data->ctx, data->bounds, GOvalScaleModeFitCircle, RADIAL_WIDTH,
      calculate_angle(at, data->total_duration),
      calculate_angle(at + phase_duration, data->total_duration));
}

static void on_update_proc(Layer* layer, GContext* ctx) {
  ActivityWindow* activity =
      (ActivityWindow*)window_get_user_data(layer_get_window(layer));
  GRect bounds = layer_get_bounds(layer);
  uint32_t marker_angle;
  static const uint32_t marker_angle_delta = DEG_TO_TRIGANGLE(MARKER_DEGREES);
  StateIteratorData userdata = {
      .bounds = bounds,
      .ctx = ctx,
      .total_duration = programme_duration(activity->programme),
  };

  // We'll draw the circle that shows the activity in stages by iterating over
  // the phases in the programme. Each phase will draw an arc section, and if we
  // do the maths correctly, everything will just look like a fancy
  // multicoloured circle.
  programme_iterate_states(activity->programme, on_state, &userdata);

  bounds.size.w += MARKER_SIZE - RADIAL_WIDTH;
  bounds.size.h += MARKER_SIZE - RADIAL_WIDTH;
  bounds.origin.x -= (MARKER_SIZE - RADIAL_WIDTH) / 2;
  bounds.origin.y -= (MARKER_SIZE - RADIAL_WIDTH) / 2;

  // Draw the zero point in white, just to make it obvious.
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_radial(ctx, bounds, GOvalScaleModeFitCircle, MARKER_SIZE,
                       TRIG_MAX_ANGLE - marker_angle_delta, TRIG_MAX_ANGLE);
  graphics_fill_radial(ctx, bounds, GOvalScaleModeFitCircle, MARKER_SIZE,
                       DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(2));

  // Calculate and draw the marker showing our actual progression to date.
  graphics_context_set_fill_color(ctx, GColorRed);
  marker_angle = calculate_angle(activity->elapsed, userdata.total_duration);
  // This gets a bit ugly: the radial function can't handle values less than 0
  // or greater than TRIG_MAX_ANGLE, so we have to implement our own wrapping.
  // Basically, if we're going to cross 0 or TRIG_MAX_ANGLE, we have to draw two
  // radials instead, just as we did above for the zero point.
  if (marker_angle < marker_angle_delta / 2) {
    graphics_fill_radial(
        ctx, bounds, GOvalScaleModeFitCircle, MARKER_SIZE,
        (TRIG_MAX_ANGLE + marker_angle) - (marker_angle_delta / 2),
        TRIG_MAX_ANGLE);
    graphics_fill_radial(ctx, bounds, GOvalScaleModeFitCircle, MARKER_SIZE, 0,
                         marker_angle + marker_angle_delta / 2);
  } else if (marker_angle > (TRIG_MAX_ANGLE - marker_angle_delta / 2)) {
    graphics_fill_radial(ctx, bounds, GOvalScaleModeFitCircle, MARKER_SIZE,
                         marker_angle, TRIG_MAX_ANGLE);
    graphics_fill_radial(
        ctx, bounds, GOvalScaleModeFitCircle, MARKER_SIZE, 0,
        (marker_angle + marker_angle_delta / 2) - TRIG_MAX_ANGLE);
  } else {
    graphics_fill_radial(ctx, bounds, GOvalScaleModeFitCircle, MARKER_SIZE,
                         marker_angle - marker_angle_delta / 2,
                         marker_angle + marker_angle_delta / 2);
  }
}

static void update_text_labels(ActivityWindow* activity) {
  if (activity->state == ACTIVITY_ACTIVE) {
    time_t phase_remaining =
        programme_phase_remaining_at(activity->programme, activity->elapsed);

    if (phase_remaining / 60 > 9) {
      snprintf(activity->time_remaining_buffer,
               sizeof(activity->time_remaining_buffer), "%2lld:%02lld",
               phase_remaining / 60, phase_remaining % 60);
    } else {
      snprintf(activity->time_remaining_buffer,
               sizeof(activity->time_remaining_buffer), "%1lld:%02lld",
               phase_remaining / 60, phase_remaining % 60);

      if (phase_remaining == 0) {
        vibes_double_pulse();
      }
    }
    activity
        ->time_remaining_buffer[sizeof(activity->time_remaining_buffer) - 1] =
        '\0';

    strncpy(activity->phase_buffer,
            programme_state_string(
                programme_state_at(activity->programme, activity->elapsed)),
            sizeof(activity->phase_buffer));
    activity->phase_buffer[sizeof(activity->phase_buffer) - 1] = '\0';
  }
}

static void on_reward_back(void* userdata) {
  ActivityWindow* activity = (ActivityWindow*)userdata;

  window_stack_pop(true);
  (activity->callbacks.on_back)(activity->callbacks.userdata);
}

static void activity_complete(ActivityWindow* activity) {
  activity->reward = reward_window_create((RewardCallbacks){
      .on_back = on_reward_back,
      .userdata = activity,
  });

  window_stack_push(reward_window_get_window(activity->reward), true);
}

static void on_tick(void* userdata) {
  ActivityWindow* activity = (ActivityWindow*)userdata;

  activity->elapsed = time(NULL) - activity->started_at;
  if (activity->elapsed >= programme_duration(activity->programme)) {
    activity_window_set_active(activity, false);
    activity_complete(activity);
  } else {
    // We have to chain timers because the TimerService doesn't allow for
    // userdata to be provided. This is going to use more battery, but provides
    // more encapsulated code.
    activity->timer = app_timer_register(TIMER_TIMEOUT_MS, on_tick, activity);
  }

  layer_mark_dirty(activity->gfx);
  update_text_labels(activity);
}

static void on_appear(Window* window) {
  ActivityWindow* activity = (ActivityWindow*)window_get_user_data(window);

  activity->elapsed = 0;
  activity_window_set_active(activity, true);
  update_text_labels(activity);
}

static void on_disappear(Window* window) {
  ActivityWindow* activity = (ActivityWindow*)window_get_user_data(window);

  activity_window_set_active(activity, false);
}

static void on_button_back(ClickRecognizerRef ref, void* ctx) {
  ActivityWindow* activity = (ActivityWindow*)ctx;

  (activity->callbacks.on_back)(activity->callbacks.userdata);
}

static void on_button_select(ClickRecognizerRef ref, void* ctx) {
  ActivityWindow* activity = (ActivityWindow*)ctx;

  activity_window_set_active(activity,
                             activity->state == ACTIVITY_PAUSED ? true : false);
  layer_mark_dirty(activity->gfx);
  update_text_labels(activity);
}

static void on_button_up(ClickRecognizerRef ref, void* ctx) {
  ActivityWindow* activity = (ActivityWindow*)ctx;

  if (activity->state == ACTIVITY_ACTIVE) {
    time_t phase_remaining =
        programme_phase_remaining_at(activity->programme, activity->elapsed);

    if (activity->elapsed + phase_remaining >=
        programme_duration(activity->programme)) {
      activity_complete(activity);
      return;
    }

    activity->started_at -= phase_remaining;
  }
}

static void on_button_down(ClickRecognizerRef ref, void* ctx) {
  ActivityWindow* activity = (ActivityWindow*)ctx;

  if (activity->state == ACTIVITY_ACTIVE) {
    time_t phase_elapsed =
        programme_phase_elapsed_at(activity->programme, activity->elapsed);

    // If we're less than five seconds into the phase, we want to go back to the
    // start of the _previous_ phase.
    if (phase_elapsed < 5) {
      // Special case: just go back to the start if this is the first phase.
      if (activity->elapsed - activity->started_at < 5) {
        activity->started_at = time(NULL);
        return;
      }

      activity->started_at += programme_phase_elapsed_at(
                                  activity->programme, activity->elapsed - 5) +
                              5;
    }

    activity->started_at += phase_elapsed + 1;
  }
}

static void click_config_provider(void* ctx) {
  window_single_click_subscribe(BUTTON_ID_BACK, on_button_back);
  window_single_click_subscribe(BUTTON_ID_SELECT, on_button_select);
  window_single_click_subscribe(BUTTON_ID_UP, on_button_up);
  window_single_click_subscribe(BUTTON_ID_DOWN, on_button_down);
}

static void on_load(Window* window) {
  ActivityWindow* activity = (ActivityWindow*)window_get_user_data(window);
  Layer* root = window_get_root_layer(window);

#ifdef PBL_ROUND
  // To centre a circle within the round watch screen, we have to ignore the
  // status and action bars, and instead use extra padding to avoid overlaps.
  GRect bounds = layer_get_bounds(root);
#else
  GRect bounds = calculate_bounds_with_status_action_bars(root);
#endif

#ifndef PBL_ROUND
  activity->action_bar = action_bar_layer_create();
  action_bar_layer_set_context(activity->action_bar, activity);
  action_bar_layer_set_click_config_provider(activity->action_bar,
                                             click_config_provider);
  action_bar_layer_add_to_window(activity->action_bar, activity->window);
#else
  window_set_click_config_provider_with_context(
      activity->window, click_config_provider, activity);
#endif

  activity->status_bar = status_bar_layer_create();
  layer_add_child(root, status_bar_layer_get_layer(activity->status_bar));

  // By working from the outside in, we can incrementally reduce the size of the
  // bounds. First up, we'll just move in the padding size for the graphics
  // layer on which we'll draw the circular view of the activity.
  bounds.size.w -= PADDING_SIZE * 2;
  bounds.size.h -= PADDING_SIZE * 2;
  bounds.origin.x += PADDING_SIZE;
  bounds.origin.y += PADDING_SIZE;
  activity->gfx = layer_create(bounds);
  layer_set_update_proc(activity->gfx, on_update_proc);
  layer_add_child(root, activity->gfx);

  // Now we want to constrain the text layers to the inside of the circle. If we
  // were doing this properly, there'd be a square root involved somewhere. But
  // since not all Pebbles have a FPU and I don't care for pixel precision,
  // we'll just put some fudge in and it'll all be fine. Mmmm. Fudge.
  const uint16_t circle_radius = bounds.size.w - RADIAL_WIDTH * 5;
  bounds.origin.x += bounds.size.w / 2 - circle_radius / 2;
  bounds.origin.y += bounds.size.h / 2 - circle_radius / 2;
  bounds.size.w = circle_radius;
  bounds.size.h = circle_radius;

  activity->time_remaining = text_layer_create(bounds);
  text_layer_set_text_alignment(activity->time_remaining, GTextAlignmentCenter);
  text_layer_set_font(activity->time_remaining,
                      fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text(activity->time_remaining,
                      activity->time_remaining_buffer);
  layer_add_child(root, text_layer_get_layer(activity->time_remaining));

  // Finally, put the phase label below the time remaining.
  bounds.origin.y += bounds.size.h - PHASE_HEIGHT;
  bounds.size.h = PHASE_HEIGHT;
  activity->phase = text_layer_create(bounds);
  text_layer_set_text_alignment(activity->phase, GTextAlignmentCenter);
  text_layer_set_font(activity->phase,
                      fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text(activity->phase, activity->phase_buffer);
  layer_add_child(root, text_layer_get_layer(activity->phase));
}

static void on_unload(Window* window) {
  ActivityWindow* activity = (ActivityWindow*)window_get_user_data(window);

#ifndef PBL_ROUND
  action_bar_layer_destroy(activity->action_bar);
#endif

  layer_destroy(activity->gfx);
  text_layer_destroy(activity->phase);
  status_bar_layer_destroy(activity->status_bar);
  text_layer_destroy(activity->time_remaining);
}

ActivityWindow* activity_window_create(ActivityCallbacks callbacks) {
  ActivityWindow* activity = malloc(sizeof(ActivityWindow));

  activity->elapsed = 0;
  activity->state = ACTIVITY_PAUSED;
  activity->programme = NULL;
  activity->callbacks = callbacks;
  activity->reward = NULL;

  activity->window = window_create();
  window_set_user_data(activity->window, activity);

  window_set_window_handlers(activity->window, (WindowHandlers){
                                                   .load = on_load,
                                                   .unload = on_unload,
                                                   .appear = on_appear,
                                                   .disappear = on_disappear,
                                               });

  return activity;
}

void activity_window_destroy(ActivityWindow* activity) {
  if (activity->reward) {
    reward_window_destroy(activity->reward);
  }
  window_destroy(activity->window);
  free(activity);
}

Window* activity_window_get_window(ActivityWindow* activity) {
  return activity->window;
}

void activity_window_set_active(ActivityWindow* activity, bool active) {
  if (active) {
#ifndef PBL_ROUND
    action_bar_layer_set_icon(activity->action_bar, BUTTON_ID_SELECT,
                              image_pause);
    action_bar_layer_set_icon(activity->action_bar, BUTTON_ID_UP,
                              image_skip_forward);
    action_bar_layer_set_icon(activity->action_bar, BUTTON_ID_DOWN,
                              image_skip_backward);
#endif

    activity->state = ACTIVITY_ACTIVE;
    activity->started_at = time(NULL) - activity->elapsed;
    activity->timer = app_timer_register(TIMER_TIMEOUT_MS, on_tick, activity);
  } else {
#ifndef PBL_ROUND
    action_bar_layer_set_icon(activity->action_bar, BUTTON_ID_SELECT,
                              image_play);
    action_bar_layer_clear_icon(activity->action_bar, BUTTON_ID_UP);
    action_bar_layer_clear_icon(activity->action_bar, BUTTON_ID_DOWN);
#endif

    activity->state = ACTIVITY_PAUSED;
    if (activity->timer) {
      app_timer_cancel(activity->timer);
      activity->timer = NULL;
    }
  }
}

void activity_window_set_programme(ActivityWindow* activity,
                                   const Programme* programme) {
  activity->programme = programme;
}
