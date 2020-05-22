#ifndef BITMAP_H_INCLUDED
#define BITMAP_H_INCLUDED

#include <pebble.h>

extern GBitmap* image_arrow_down;
extern GBitmap* image_arrow_right;
extern GBitmap* image_arrow_up;
extern GBitmap* image_tick;

extern void bitmap_init(void);
extern void bitmap_deinit(void);

#endif