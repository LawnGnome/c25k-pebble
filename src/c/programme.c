#include "programme.h"

typedef struct {
  time_t duration;
  ProgrammeState state;
} ProgrammePhase;

struct Programme {
  const ProgrammePhase* phases;
};

static const ProgrammePhase week1[] = {
    {300, PROGRAMME_STATE_WARM_UP},   {60, PROGRAMME_STATE_RUN},
    {90, PROGRAMME_STATE_WALK},       {60, PROGRAMME_STATE_RUN},
    {90, PROGRAMME_STATE_WALK},       {60, PROGRAMME_STATE_RUN},
    {90, PROGRAMME_STATE_WALK},       {60, PROGRAMME_STATE_RUN},
    {90, PROGRAMME_STATE_WALK},       {60, PROGRAMME_STATE_RUN},
    {90, PROGRAMME_STATE_WALK},       {60, PROGRAMME_STATE_RUN},
    {90, PROGRAMME_STATE_WALK},       {60, PROGRAMME_STATE_RUN},
    {90, PROGRAMME_STATE_WALK},       {60, PROGRAMME_STATE_RUN},
    {300, PROGRAMME_STATE_WARM_DOWN}, {0, PROGRAMME_STATE_END},
};

static const struct {
  uint8_t week;
  uint8_t day;
  const Programme programme;
} programmes[] = {
    {
        .week = 1,
        .day = 1,
        .programme = {week1},
    },
    {
        .week = 1,
        .day = 2,
        .programme = {week1},
    },
    {
        .week = 1,
        .day = 3,
        .programme = {week1},
    },
};

const Programme* programme_get(int8_t week, int8_t day) {
  size_t i;

  for (i = 0; i < sizeof(programmes) / sizeof(programmes[0]); i++) {
    if (week == programmes[i].week && day == programmes[i].day) {
      return &programmes[i].programme;
    }
  }

  return NULL;
}

void programme_iterate_states(const Programme* programme,
                              ProgrammeStateIterator on_state,
                              void* userdata) {
  time_t at = 0;
  size_t i;

  for (i = 0; programme->phases[i].state != PROGRAMME_STATE_END; i++) {
    (on_state)(at, programme->phases[i].duration, programme->phases[i].state,
               userdata);
    at += programme->phases[i].duration;
  }
}

time_t programme_state_duration(const Programme* programme) {
  time_t duration = 0;
  size_t i;

  for (i = 0; programme->phases[i].state != PROGRAMME_STATE_END; i++) {
    duration += programme->phases[i].duration;
  }

  return duration;
}

ProgrammeState programme_state_at(const Programme* programme, time_t at) {
  time_t duration = 0;
  size_t i;
  ProgrammeState current_state = programme->phases[0].state;

  for (i = 0; programme->phases[i].state != PROGRAMME_STATE_END; i++) {
    duration += programme->phases[i].duration;

    if (duration >= at) {
      return current_state;
    }
    current_state = programme->phases[i].state;
  }

  return current_state;
}
