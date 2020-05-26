#include "number.h"

#include <memory.h>
#include <string.h>

static const GColor ACTIVE_BG = COLOR_FALLBACK(GColorGreen, GColorBlack);
static const GColor ACTIVE_FG = COLOR_FALLBACK(GColorBlack, GColorWhite);
static const GColor INACTIVE_BG = COLOR_FALLBACK(GColorLightGray, GColorWhite);
static const GColor INACTIVE_FG =
    COLOR_FALLBACK(GColorDarkGray, GColorDarkGray);
static const int16_t FONT_SIZE_LABEL = 14;
static const int16_t FONT_SIZE_VALUE = 28;
static const int16_t VERTICAL_PADDING_LABEL = 6;
static const int16_t VERTICAL_PADDING_VALUE = 8;

struct _number_layer_t {
  int8_t min;
  int8_t max;
  int8_t value;

  char* label_str;
  char value_str[5];

  // Internal layers.
  Layer* root;
  TextLayer* label;
  TextLayer* current;
};

static void number_layer_update_current_value(NumberLayer* layer) {
  snprintf(layer->value_str, sizeof(layer->value_str), "%d", layer->value);
  layer_mark_dirty(text_layer_get_layer(layer->current));
}

NumberLayer* number_layer_create(GRect frame,
                                 const char* label,
                                 int8_t min,
                                 int8_t max,
                                 int8_t initial) {
  NumberLayer* layer = malloc(sizeof(NumberLayer));
  static const int16_t total_height = FONT_SIZE_LABEL + FONT_SIZE_VALUE +
                                      VERTICAL_PADDING_LABEL +
                                      VERTICAL_PADDING_VALUE * 2;
  const int16_t y_offset =
      total_height < frame.size.h ? (frame.size.h - total_height) / 2 : 0;

  layer->min = min;
  layer->max = max;
  layer->value = initial;

  memset(layer->value_str, 0, sizeof(layer->value_str));
  layer->label_str = strdup(label);

  layer->root = layer_create(frame);

  layer->label = text_layer_create((GRect){
      .origin = {0, y_offset},
      .size = {frame.size.w, FONT_SIZE_LABEL + VERTICAL_PADDING_LABEL},
  });
  text_layer_set_background_color(layer->label, GColorClear);
  text_layer_set_text_color(layer->label, GColorDarkGray);
  text_layer_set_font(layer->label,
                      fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_alignment(layer->label, GTextAlignmentCenter);
  text_layer_set_text(layer->label, layer->label_str);
  layer_add_child(layer->root, text_layer_get_layer(layer->label));

  layer->current = text_layer_create((GRect){
      .origin = {0, y_offset + FONT_SIZE_LABEL + VERTICAL_PADDING_LABEL},
      .size = {frame.size.w, FONT_SIZE_VALUE + VERTICAL_PADDING_VALUE * 2},
  });
  text_layer_set_font(layer->current,
                      fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(layer->current, GTextAlignmentCenter);
  text_layer_set_text(layer->current, layer->value_str);
  number_layer_set_active(layer, false);
  number_layer_update_current_value(layer);
  layer_add_child(layer->root, text_layer_get_layer(layer->current));

  return layer;
}

void number_layer_destroy(NumberLayer* layer) {
  text_layer_destroy(layer->current);
  text_layer_destroy(layer->label);
  layer_destroy(layer->root);
  free(layer->label_str);
  free(layer);
}

Layer* number_layer_get_layer(NumberLayer* layer) {
  return layer->root;
}

void number_layer_increment_value(NumberLayer* layer) {
  if (++layer->value > layer->max) {
    layer->value = layer->min;
  }
  number_layer_update_current_value(layer);
}

void number_layer_decrement_value(NumberLayer* layer) {
  if (--layer->value < layer->min) {
    layer->value = layer->max;
  }
  number_layer_update_current_value(layer);
}

int8_t number_layer_get_value(const NumberLayer* layer) {
  return layer->value;
}

void number_layer_set_active(NumberLayer* layer, bool active) {
  text_layer_set_background_color(layer->current,
                                  active ? ACTIVE_BG : INACTIVE_BG);
  text_layer_set_text_color(layer->current, active ? ACTIVE_FG : INACTIVE_FG);
}
