#ifndef PROGRAMME_H_INCLUDED
#define PROGRAMME_H_INCLUDED

#include "c25k-pebble.h"

typedef enum {
  PROGRAMME_STATE_WARM_UP,
  PROGRAMME_STATE_RUN,
  PROGRAMME_STATE_WALK,
  PROGRAMME_STATE_WARM_DOWN,
  PROGRAMME_STATE_END,
} ProgrammeState;

typedef struct Programme Programme;

typedef void (*ProgrammeStateIterator)(time_t at,
                                       time_t duration,
                                       ProgrammeState state,
                                       void* userdata);

extern time_t programme_duration(const Programme* programme);
extern const Programme* programme_get(int8_t week, int8_t day);
extern void programme_iterate_states(const Programme* programme,
                                     ProgrammeStateIterator on_state,
                                     void* userdata);
extern time_t programme_phase_elapsed_at(const Programme* programme, time_t at);
extern time_t programme_phase_remaining_at(const Programme* programme,
                                           time_t at);
extern ProgrammeState programme_state_at(const Programme* programme, time_t at);
extern const char* programme_state_string(ProgrammeState state);

#endif