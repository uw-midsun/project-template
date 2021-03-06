#include "status.h"
#include "test_helpers.h"
#include "unity.h"

#include "interrupt.h"
#include "lights_blinker.h"
#include "lights_events.h"
#include "lights_strobe.h"

#define TEST_LIGHTS_STROBE_DURATION 300

static LightsStrobeStorage s_lights_strobe = { 0 };

void setup_test(void) {
  interrupt_init();
  soft_timer_init();
  event_queue_init();
  lights_strobe_init(&s_lights_strobe, TEST_LIGHTS_STROBE_DURATION);
}

void teardown_test(void) {}

// We turn strobe on, make sure it blinks, and turn it off again.
void test_lights_strobe_turn_on_off() {
  // Turning strobe on.
  const Event e1 = {
    .id = LIGHTS_EVENT_STROBE_ON,  //
    .data = 0                      //
  };
  TEST_ASSERT_OK(lights_strobe_process_event(&s_lights_strobe, &e1));
  Event event = { 0 };
  TEST_ASSERT_OK(event_process(&event));
  TEST_ASSERT_EQUAL(event.id, LIGHTS_EVENT_GPIO_ON);
  TEST_ASSERT_EQUAL(event.data, LIGHTS_EVENT_GPIO_PERIPHERAL_STROBE);

  // Waiting for one blink.
  while (event_process(&event) != STATUS_CODE_OK) {
  }
  // Asserting correct event raised by blinker.
  TEST_ASSERT_EQUAL(event.id, LIGHTS_EVENT_GPIO_OFF);
  TEST_ASSERT_EQUAL(event.data, LIGHTS_EVENT_GPIO_PERIPHERAL_STROBE);

  // Turning strobe off.
  const Event e2 = {
    .id = LIGHTS_EVENT_STROBE_OFF,  //
    .data = 0                       //
  };
  TEST_ASSERT_OK(lights_strobe_process_event(&s_lights_strobe, &e2));
  TEST_ASSERT_OK(event_process(&event));
  TEST_ASSERT_EQUAL(event.id, LIGHTS_EVENT_GPIO_OFF);
  TEST_ASSERT_EQUAL(event.data, LIGHTS_EVENT_GPIO_PERIPHERAL_STROBE);
}

// Do nothing when event is not LIGHTS_EVENT_STROBE.
void test_lights_strobe_non_strobe_event() {
  // Left signal event.
  const Event e1 = {
    .id = LIGHTS_EVENT_SIGNAL_ON,          //
    .data = LIGHTS_EVENT_SIGNAL_MODE_LEFT  //
  };
  // Do nothing.
  TEST_ASSERT_OK(lights_strobe_process_event(&s_lights_strobe, &e1));
  // Storage for event.
  Event e = { 0 };
  TEST_ASSERT_EQUAL(event_process(&e), STATUS_CODE_EMPTY);
}
