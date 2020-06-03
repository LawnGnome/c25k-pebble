#ifndef REWARD_H_INCLUDED
#define REWARD_H_INCLUDED

#include "c25k-pebble.h"

typedef struct RewardWindow RewardWindow;

typedef struct {
  void (*on_back)(void* userdata);
  void* userdata;
} RewardCallbacks;

extern RewardWindow* reward_window_create(RewardCallbacks callbacks);
extern void reward_window_destroy(RewardWindow* reward);
extern Window* reward_window_get_window(RewardWindow* reward);

#endif