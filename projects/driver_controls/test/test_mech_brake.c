
#include <stdio.h>

#include <stdbool.h>
#include "adc.h"
#include "ads1015.h"
#include "ads1015_def.h"
#include "dc_cfg.h"
#include "delay.h"
#include "event_arbiter.h"
#include "event_queue.h"
#include "exported_enums.h"
#include "gpio_it.h"
#include "i2c.h"
#include "input_event.h"
#include "interrupt.h"
#include "log.h"
#include "mech_brake.h"
#include "soft_timer.h"
#include "status.h"
#include "test_helpers.h"
#include "unity.h"

static MechBrakeStorage s_mech_brake_storage;
static Ads1015Storage s_ads1015_storage;
static MechBrakeCalibrationData s_calibration_data;

#define TEST_MECH_BRAKE_TOLERANCE 2
#define TEST_MECH_BRAKE_ZERO_VALUE 0
#define TEST_MECH_BRAKE_HUNDRED_VALUE 1 << 12

// Readings used as fake inputs.
static int16_t s_mocked_reading;

const MechBrakeSettings brake_settings = {
  .brake_pressed_threshold = 50,
  .bounds_tolerance = 2,
  .channel = ADS1015_CHANNEL_2,
  .ads1015 = &s_ads1015_storage,
};

// Mocks ads1015_read_raw
StatusCode TEST_MOCK(ads1015_read_raw)(Ads1015Storage *storage, Ads1015Channel channel,
                                       int16_t *reading) {
  *reading = s_mocked_reading;
  return STATUS_CODE_OK;
}

// Initializes the calibration data from static data
static void prv_set_calibration_data(MechBrakeCalibrationData *data) {
  data->zero_value = TEST_MECH_BRAKE_ZERO_VALUE;
  data->hundred_value = TEST_MECH_BRAKE_HUNDRED_VALUE;
}

void setup_test() {
  gpio_init();
  interrupt_init();
  gpio_it_init();
  soft_timer_init();
  const I2CSettings i2c_settings = {
    .speed = I2C_SPEED_FAST,
    .scl = DC_CFG_I2C_BUS_SCL,
    .sda = DC_CFG_I2C_BUS_SDA,
  };
  i2c_init(DC_CFG_I2C_BUS_PORT, &i2c_settings);
  event_queue_init();
  GPIOAddress ready_pin = DC_CFG_PEDAL_ADC_RDY_PIN;
  ads1015_init(&s_ads1015_storage, DC_CFG_I2C_BUS_PORT, DC_CFG_PEDAL_ADC_ADDR, &ready_pin);
  prv_set_calibration_data(&s_calibration_data);
  mech_brake_init(&s_mech_brake_storage, &brake_settings, &s_calibration_data);
}

void teardown_test(void) {}

void test_mech_brake_init_invalid_args(void) {
  // Test with valid arguments.
  TEST_ASSERT_EQUAL(STATUS_CODE_OK,
                    mech_brake_init(&s_mech_brake_storage, &brake_settings, &s_calibration_data));
  // Check for null pointers on each parameter.
  TEST_ASSERT_EQUAL(STATUS_CODE_INVALID_ARGS,
                    mech_brake_init(NULL, &brake_settings, &s_calibration_data));
  TEST_ASSERT_EQUAL(STATUS_CODE_INVALID_ARGS,
                    mech_brake_init(&s_mech_brake_storage, NULL, &s_calibration_data));
  TEST_ASSERT_EQUAL(STATUS_CODE_INVALID_ARGS,
                    mech_brake_init(&s_mech_brake_storage, &brake_settings, NULL));
}

void test_mech_brake_get_percentage_invalid_args(void) {
  int16_t percentage;
  // Check for null pointers.
  TEST_ASSERT_EQUAL(STATUS_CODE_INVALID_ARGS, mech_brake_get_position(NULL, &percentage));
  TEST_ASSERT_EQUAL(STATUS_CODE_INVALID_ARGS, mech_brake_get_position(&s_mech_brake_storage, NULL));
}

void test_mech_brake_percentage_in_released_zone(void) {
  int16_t position = 0;
  // The brake_pressed_threshold is 50%, a reading of 400 is 9%.
  s_mocked_reading = 400;
  Event e;

  TEST_ASSERT_EQUAL(STATUS_CODE_OK, mech_brake_get_position(&s_mech_brake_storage, &position));
  delay_ms(5);
  TEST_ASSERT_OK(event_process(&e));
  TEST_ASSERT_EQUAL(INPUT_EVENT_MECHANICAL_BRAKE_RELEASED, e.id);
}

void test_mech_brake_percentage_in_pressed_zone(void) {
  int16_t position = 0;
  // The brake_pressed_threshold is 50%,a reading of 400 is 73%.
  s_mocked_reading = 3000;
  Event e;

  TEST_ASSERT_EQUAL(STATUS_CODE_OK, mech_brake_get_position(&s_mech_brake_storage, &position));
  delay_ms(5);
  TEST_ASSERT_OK(event_process(&e));
  TEST_ASSERT_EQUAL(INPUT_EVENT_MECHANICAL_BRAKE_PRESSED, e.id);
}

void test_mech_brake_percentage_below_bounds(void) {
  int16_t position = 0;

  // The bounds_tolerance is 2%, so the lower_bound for the position is -81.
  s_mocked_reading = -1000;
  Event e;

  TEST_ASSERT_EQUAL(STATUS_CODE_OUT_OF_RANGE,
                    mech_brake_get_position(&s_mech_brake_storage, &position));
  delay_ms(5);
}

void test_mech_brake_percentage_above_bounds(void) {
  int16_t position = 0;
  // The bounds_tolerance is 2%, so the upper_bound for the position is 4177.
  s_mocked_reading = EE_DRIVE_OUTPUT_DENOMINATOR + 1000;
  Event e;

  TEST_ASSERT_EQUAL(STATUS_CODE_OUT_OF_RANGE,
                    mech_brake_get_position(&s_mech_brake_storage, &position));
  delay_ms(5);
}

void test_mech_brake_percentage_within_lower_tolerance(void) {
  int16_t position = 0;
  // This is just within the lower bound of -81.
  s_mocked_reading = -80;
  Event e;

  TEST_ASSERT_EQUAL(STATUS_CODE_OK, mech_brake_get_position(&s_mech_brake_storage, &position));
  delay_ms(5);
  TEST_ASSERT_OK(event_process(&e));
  TEST_ASSERT_EQUAL(INPUT_EVENT_MECHANICAL_BRAKE_RELEASED, e.id);
}

void test_mech_brake_percentage_within_upper_tolerance(void) {
  int16_t position = 0;
  // This is just within the upper bound of 4177.
  s_mocked_reading = 4176;
  Event e;

  TEST_ASSERT_EQUAL(STATUS_CODE_OK, mech_brake_get_position(&s_mech_brake_storage, &position));
  delay_ms(5);
  TEST_ASSERT_OK(event_process(&e));
  TEST_ASSERT_EQUAL(INPUT_EVENT_MECHANICAL_BRAKE_PRESSED, e.id);
}
