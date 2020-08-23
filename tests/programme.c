#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>

#include <cmocka.h>

// Reuse a trick from the Pebble SDK to prevent the main SDK from being
// included, since we don't need it.
#define C25K_PEBBLE_H_INCLUDED
#include "../src/c/programme.c"

static void test_w2d3(void** state) {
  // user 'selects' week 2, day 3 through UI
  const int8_t week = 2;
  const int8_t day = 3;

  // Call programme_get when user submits 'selection' (see on_selector_submit(...))
  const Programme *prog = programme_get(week, day);
  assert_non_null(prog);
}

static void test_sanity(void** state) {
  size_t i;

  (void)state;

  for (i = 0; i < sizeof(programmes) / sizeof(programmes[0]); i++) {
    size_t j;

    assert_int_not_equal(0, programmes[i].week);
    assert_int_not_equal(0, programmes[i].day);

    for (j = 0; j < 100; j++) {
      if (programmes[i].programme.phases[j].state == PROGRAMME_STATE_END) {
        goto success;
      }
    }

    fail_msg("never got a PROGRAMME_STATE_END phase");

  success:;
  }
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_sanity),
      cmocka_unit_test(test_w2d3),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
