#include "bitmap.h"

GBitmap* image_arrow_down;
GBitmap* image_arrow_right;
GBitmap* image_arrow_up;
GBitmap* image_tick;

typedef struct {
  uint32_t resource;
  GBitmap** bitmap;
} BitmapMapping;

static const BitmapMapping mappings[] = {
    {RESOURCE_ID_IMAGE_ARROW_DOWN, &image_arrow_down},
    {RESOURCE_ID_IMAGE_ARROW_RIGHT, &image_arrow_right},
    {RESOURCE_ID_IMAGE_ARROW_UP, &image_arrow_up},
    {RESOURCE_ID_IMAGE_TICK, &image_tick},
};

void bitmap_init(void) {
  size_t i;

  for (i = 0; i < sizeof(mappings) / sizeof(mappings[0]); i++) {
    *mappings[i].bitmap = gbitmap_create_with_resource(mappings[i].resource);
  }
}

void bitmap_deinit(void) {
  size_t i;

  for (i = 0; i < sizeof(mappings) / sizeof(mappings[0]); i++) {
    gbitmap_destroy(*mappings[i].bitmap);
  }
}
