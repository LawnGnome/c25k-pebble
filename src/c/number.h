#ifndef NUMBER_H_INCLUDED
#define NUMBER_H_INCLUDED

#include "c25k-pebble.h"

typedef struct _number_layer_t NumberLayer;

extern NumberLayer* number_layer_create(GRect frame,
                                        const char* label,
                                        int8_t min,
                                        int8_t max,
                                        int8_t initial);
extern void number_layer_destroy(NumberLayer* layer);
extern Layer* number_layer_get_layer(NumberLayer* layer);

extern void number_layer_increment_value(NumberLayer* layer);
extern void number_layer_decrement_value(NumberLayer* layer);
extern int8_t number_layer_get_value(const NumberLayer* layer);

extern void number_layer_set_active(NumberLayer* layer, bool active);

#endif