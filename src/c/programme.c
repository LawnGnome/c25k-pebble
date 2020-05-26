#include "programme.h"

typedef struct {
  time_t duration;
  ProgrammeState state;
} ProgrammePhase;

struct Programme {
  const ProgrammePhase* phases;
};

static const ProgrammePhase week1[] = {
    {300, PROGRAMME_STATE_WARM_UP}, {60, PROGRAMME_STATE_RUN},
    {90, PROGRAMME_STATE_WALK},     {60, PROGRAMME_STATE_RUN},
    {90, PROGRAMME_STATE_WALK},     {60, PROGRAMME_STATE_RUN},
    {90, PROGRAMME_STATE_WALK},     {60, PROGRAMME_STATE_RUN},
    {90, PROGRAMME_STATE_WALK},     {60, PROGRAMME_STATE_RUN},
    {90, PROGRAMME_STATE_WALK},     {60, PROGRAMME_STATE_RUN},
    {90, PROGRAMME_STATE_WALK},     {60, PROGRAMME_STATE_RUN},
    {90, PROGRAMME_STATE_WALK},     {60, PROGRAMME_STATE_RUN},
    {90, PROGRAMME_STATE_WALK},     {300, PROGRAMME_STATE_WARM_DOWN},
    {0, PROGRAMME_STATE_END},
};

static const ProgrammePhase week2[] = {
    {300, PROGRAMME_STATE_WARM_UP}, {90, PROGRAMME_STATE_RUN},
    {120, PROGRAMME_STATE_WALK},    {90, PROGRAMME_STATE_RUN},
    {120, PROGRAMME_STATE_WALK},    {90, PROGRAMME_STATE_RUN},
    {120, PROGRAMME_STATE_WALK},    {90, PROGRAMME_STATE_RUN},
    {120, PROGRAMME_STATE_WALK},    {90, PROGRAMME_STATE_RUN},
    {120, PROGRAMME_STATE_WALK},    {90, PROGRAMME_STATE_RUN},
    {120, PROGRAMME_STATE_WALK},    {300, PROGRAMME_STATE_WARM_DOWN},
    {0, PROGRAMME_STATE_END},
};

static const ProgrammePhase week3[] = {
    {300, PROGRAMME_STATE_WARM_UP}, {90, PROGRAMME_STATE_RUN},
    {90, PROGRAMME_STATE_WALK},     {180, PROGRAMME_STATE_RUN},
    {180, PROGRAMME_STATE_WALK},    {90, PROGRAMME_STATE_RUN},
    {90, PROGRAMME_STATE_WALK},     {180, PROGRAMME_STATE_RUN},
    {180, PROGRAMME_STATE_WALK},    {300, PROGRAMME_STATE_WARM_DOWN},
    {0, PROGRAMME_STATE_END},
};

static const ProgrammePhase week4[] = {
    {300, PROGRAMME_STATE_WARM_UP},   {180, PROGRAMME_STATE_RUN},
    {90, PROGRAMME_STATE_WALK},       {300, PROGRAMME_STATE_RUN},
    {150, PROGRAMME_STATE_WALK},      {180, PROGRAMME_STATE_RUN},
    {90, PROGRAMME_STATE_WALK},       {300, PROGRAMME_STATE_RUN},
    {300, PROGRAMME_STATE_WARM_DOWN}, {0, PROGRAMME_STATE_END},
};

static const ProgrammePhase week7[] = {
    {300, PROGRAMME_STATE_WARM_UP},
    {1500, PROGRAMME_STATE_RUN},
    {300, PROGRAMME_STATE_WARM_DOWN},
    {0, PROGRAMME_STATE_END},
};

static const ProgrammePhase week8[] = {
    {300, PROGRAMME_STATE_WARM_UP},
    {1680, PROGRAMME_STATE_RUN},
    {300, PROGRAMME_STATE_WARM_DOWN},
    {0, PROGRAMME_STATE_END},
};

static const ProgrammePhase week9[] = {
    {300, PROGRAMME_STATE_WARM_UP},
    {1800, PROGRAMME_STATE_RUN},
    {300, PROGRAMME_STATE_WARM_DOWN},
    {0, PROGRAMME_STATE_END},
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
    {
        .week = 2,
        .day = 1,
        .programme = {week2},
    },
    {
        .week = 2,
        .day = 2,
        .programme = {week2},
    },
    {
        .week = 3,
        .day = 3,
        .programme = {week2},
    },
    {
        .week = 3,
        .day = 1,
        .programme = {week3},
    },
    {
        .week = 3,
        .day = 2,
        .programme = {week3},
    },
    {
        .week = 3,
        .day = 3,
        .programme = {week3},
    },
    {
        .week = 4,
        .day = 1,
        .programme = {week4},
    },
    {
        .week = 4,
        .day = 2,
        .programme = {week4},
    },
    {
        .week = 4,
        .day = 3,
        .programme = {week4},
    },
    {
        .week = 5,
        .day = 1,
        .programme =
            {
                (const ProgrammePhase[]){
                    {300, PROGRAMME_STATE_WARM_UP},
                    {300, PROGRAMME_STATE_RUN},
                    {180, PROGRAMME_STATE_WALK},
                    {300, PROGRAMME_STATE_RUN},
                    {180, PROGRAMME_STATE_WALK},
                    {300, PROGRAMME_STATE_RUN},
                    {300, PROGRAMME_STATE_WARM_DOWN},
                    {0, PROGRAMME_STATE_END},

                },
            },
    },
    {
        .week = 5,
        .day = 2,
        .programme =
            {
                (const ProgrammePhase[]){
                    {300, PROGRAMME_STATE_WARM_UP},
                    {480, PROGRAMME_STATE_RUN},
                    {300, PROGRAMME_STATE_WALK},
                    {480, PROGRAMME_STATE_RUN},
                    {300, PROGRAMME_STATE_WARM_DOWN},
                    {0, PROGRAMME_STATE_END},

                },
            },
    },
    {
        .week = 5,
        .day = 3,
        .programme =
            {
                (const ProgrammePhase[]){
                    {300, PROGRAMME_STATE_WARM_UP},
                    {1200, PROGRAMME_STATE_RUN},
                    {300, PROGRAMME_STATE_WARM_DOWN},
                    {0, PROGRAMME_STATE_END},

                },
            },
    },
    {
        .week = 6,
        .day = 1,
        .programme =
            {
                (const ProgrammePhase[]){
                    {300, PROGRAMME_STATE_WARM_UP},
                    {300, PROGRAMME_STATE_RUN},
                    {180, PROGRAMME_STATE_WALK},
                    {480, PROGRAMME_STATE_RUN},
                    {180, PROGRAMME_STATE_WALK},
                    {300, PROGRAMME_STATE_RUN},
                    {300, PROGRAMME_STATE_WARM_DOWN},
                    {0, PROGRAMME_STATE_END},

                },
            },
    },
    {
        .week = 6,
        .day = 2,
        .programme =
            {
                (const ProgrammePhase[]){
                    {300, PROGRAMME_STATE_WARM_UP},
                    {600, PROGRAMME_STATE_RUN},
                    {180, PROGRAMME_STATE_WALK},
                    {600, PROGRAMME_STATE_RUN},
                    {300, PROGRAMME_STATE_WARM_DOWN},
                    {0, PROGRAMME_STATE_END},

                },
            },
    },
    {
        .week = 6,
        .day = 3,
        // Not a typo; it's the same programme as week 7.
        .programme = {week7},
    },
    {
        .week = 7,
        .day = 1,
        .programme = {week7},
    },
    {
        .week = 7,
        .day = 2,
        .programme = {week7},
    },
    {
        .week = 7,
        .day = 3,
        .programme = {week7},
    },
    {
        .week = 8,
        .day = 1,
        .programme = {week8},
    },
    {
        .week = 8,
        .day = 2,
        .programme = {week8},
    },
    {
        .week = 8,
        .day = 3,
        .programme = {week8},
    },
    {
        .week = 9,
        .day = 1,
        .programme = {week9},
    },
    {
        .week = 9,
        .day = 2,
        .programme = {week9},
    },
    {
        .week = 9,
        .day = 3,
        .programme = {week9},
    },
};

time_t programme_duration(const Programme* programme) {
  time_t duration = 0;
  size_t i;

  for (i = 0; programme->phases[i].state != PROGRAMME_STATE_END; i++) {
    duration += programme->phases[i].duration;
  }

  return duration;
}

const Programme* programme_get(int8_t week, int8_t day) {
  size_t i;

  // A linear search is obviously nowhere near theoretically optimal, but even a
  // Cortex-M3 should be able to do this just fine with 27 entries.
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

time_t programme_phase_remaining_at(const Programme* programme, time_t at) {
  time_t duration = 0;
  size_t i;

  for (i = 0; programme->phases[i].state != PROGRAMME_STATE_END; i++) {
    duration += programme->phases[i].duration;

    if (duration >= at) {
      return duration - at;
    }
  }

  return 0;
}

ProgrammeState programme_state_at(const Programme* programme, time_t at) {
  time_t duration = 0;
  size_t i;

  for (i = 0; programme->phases[i].state != PROGRAMME_STATE_END; i++) {
    duration += programme->phases[i].duration;

    if (duration >= at) {
      return programme->phases[i].state;
    }
  }

  return PROGRAMME_STATE_END;
}

const char* programme_state_string(ProgrammeState state) {
  switch (state) {
    case PROGRAMME_STATE_WARM_UP:
      return "Warmup";

    case PROGRAMME_STATE_WALK:
      return "Walk";

    case PROGRAMME_STATE_RUN:
      return "Run!";

    case PROGRAMME_STATE_WARM_DOWN:
      return "Warmdown";

    default:
      return "¯\\_(ツ)_/¯";
  }
}
