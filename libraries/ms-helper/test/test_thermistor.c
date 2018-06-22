#include <inttypes.h>
#include "adc.h"
#include "gpio.h"
#include "interrupt.h"
#include "log.h"
#include "test_helpers.h"
#include "thermistor.h"
#include "unity.h"

#define THERMISTOR_TEMPERATURE_TOLERANCE 100

// The tests cases return different test values depending on the inputted adc channel inputs
StatusCode TEST_MOCK(adc_read_converted)(ADCChannel adc_channel, uint16_t *reading) {
  switch (adc_channel) {
    case (ADC_CHANNEL_0):
      *reading = 2900;
      break;
    case (ADC_CHANNEL_1):
      *reading = 1926;
      break;
    case (ADC_CHANNEL_2):
      *reading = 2193;
      break;
    case (ADC_CHANNEL_3):
      *reading = 267;
      break;
    case (ADC_CHANNEL_4):
      *reading = 1074;
      break;
    case (ADC_CHANNEL_5):
      *reading = 807;
      break;
    case (ADC_CHANNEL_6):
      *reading = 2733;
      break;
    case (ADC_CHANNEL_7):
      *reading = 0;
      break;
    case (ADC_CHANNEL_REF):
      *reading = 3000;
      break;
    default:

      break;
  }
  return STATUS_CODE_OK;
}

StatusCode TEST_MOCK(adc_get_channel)(GPIOAddress address, ADCChannel *adc_channel) {
  *adc_channel = address.pin;
  return STATUS_CODE_OK;
}

StatusCode TEST_MOCK(adc_set_channel)(ADCChannel adc_channel, bool new_state) {
  return STATUS_CODE_OK;
}

void setup_test(void) {}

void teardown_test(void) {}

// Tests thermistor calculations with thermistor proceeding its sibling resistor
// Tests the thermistor at a standard temperature(10 degrees)
void test_thermistor_normal(void) {
  GPIOAddress gpio_addr = {
    .port = GPIO_PORT_A,
    .pin = 1,
  };
  ThermistorStorage storage;
  TEST_ASSERT_OK(thermistor_init(&storage, gpio_addr, PROCEEDING));

  uint32_t reading = 0;
  TEST_ASSERT_OK(thermistor_get_temp(&storage, &reading));
  TEST_ASSERT_UINT32_WITHIN(THERMISTOR_TEMPERATURE_TOLERANCE, 10000, reading);
  LOG_DEBUG("Temperature: %" PRIuLEAST32 "\n", reading / 1000);
}

// Tests the lower bound of the resistance-temperature lookup table (0 deg)
void test_thermistor_min_temp(void) {
  GPIOAddress gpio_addr = {
    .port = GPIO_PORT_A,
    .pin = 2,
  };
  ThermistorStorage storage;

  uint32_t reading = 0;
  TEST_ASSERT_OK(thermistor_init(&storage, gpio_addr, PROCEEDING));
  TEST_ASSERT_OK(thermistor_get_temp(&storage, &reading));
  LOG_DEBUG("Temperature: %" PRIuLEAST32 "\n", reading / 1000);
  TEST_ASSERT_UINT32_WITHIN(THERMISTOR_TEMPERATURE_TOLERANCE, 0, reading);
}

// Tests the upper bound of the resistance-temperature lookup table (100 deg)
void test_thermistor_max_temp(void) {
  GPIOAddress gpio_addr = {
    .port = GPIO_PORT_A,
    .pin = 3,
  };
  ThermistorStorage storage;

  uint32_t reading = 0;
  TEST_ASSERT_OK(thermistor_init(&storage, gpio_addr, PROCEEDING));
  TEST_ASSERT_OK(thermistor_get_temp(&storage, &reading));
  LOG_DEBUG("Temperature: %" PRIuLEAST32 "\n", reading / 1000);
  TEST_ASSERT_UINT32_WITHIN(THERMISTOR_TEMPERATURE_TOLERANCE, 100000, reading);
}

// Tests thermistor calculations with thermistor preceeding its sibling resistor
void test_thermistor_normal_alt(void) {
  GPIOAddress gpio_addr = {
    .port = GPIO_PORT_A,
    .pin = 4,
  };
  ThermistorStorage storage;
  TEST_ASSERT_OK(thermistor_init(&storage, gpio_addr, PRECEEDING));

  uint32_t reading = 0;
  TEST_ASSERT_OK(thermistor_get_temp(&storage, &reading));
  TEST_ASSERT_UINT32_WITHIN(THERMISTOR_TEMPERATURE_TOLERANCE, 10000, reading);
  LOG_DEBUG("Temperature: %" PRIuLEAST32 "\n", reading / 1000);
}

void test_thermistor_min_temp_alt(void) {
  GPIOAddress gpio_addr = {
    .port = GPIO_PORT_A,
    .pin = 5,
  };
  ThermistorStorage storage;

  uint32_t reading = 0;
  TEST_ASSERT_OK(thermistor_init(&storage, gpio_addr, PRECEEDING));
  TEST_ASSERT_OK(thermistor_get_temp(&storage, &reading));
  LOG_DEBUG("Temperature: %" PRIuLEAST32 "\n", reading / 1000);
  TEST_ASSERT_UINT32_WITHIN(THERMISTOR_TEMPERATURE_TOLERANCE, 0, reading);
}

void test_thermistor_max_temp_alt(void) {
  GPIOAddress gpio_addr = {
    .port = GPIO_PORT_A,
    .pin = 6,
  };
  ThermistorStorage storage;

  uint32_t reading = 0;
  TEST_ASSERT_OK(thermistor_init(&storage, gpio_addr, PRECEEDING));
  TEST_ASSERT_OK(thermistor_get_temp(&storage, &reading));
  LOG_DEBUG("Temperature: %" PRIuLEAST32 "\n", reading / 1000);
  TEST_ASSERT_UINT32_WITHIN(THERMISTOR_TEMPERATURE_TOLERANCE, 100000, reading);
}

// Tests for a temperature greater the lookup tables ranges
void test_thermistor_exceed_range(void) {
  GPIOAddress gpio_addr = {
    .port = GPIO_PORT_A,
    .pin = 0,
  };
  ThermistorStorage storage;

  uint32_t reading = 0;
  TEST_ASSERT_OK(thermistor_init(&storage, gpio_addr, PROCEEDING));
  TEST_ASSERT_NOT_OK(thermistor_get_temp(&storage, &reading));
  LOG_DEBUG("Temperature: %" PRIuLEAST32 "\n", reading / 1000);
}

// Tests for a temperature smaller than the lookup tables ranges
void test_thermistor_under_range(void) {
  GPIOAddress gpio_addr = {
    .port = GPIO_PORT_A,
    .pin = 7,
  };
  ThermistorStorage storage;

  uint32_t reading = 0;
  TEST_ASSERT_OK(thermistor_init(&storage, gpio_addr, PROCEEDING));
  // Adds one millivolt as to not trigger the NULL read case
  reading++;
  TEST_ASSERT_NOT_OK(thermistor_get_temp(&storage, &reading));
  LOG_DEBUG("Temperature: %" PRIuLEAST32 "\n", reading / 1000);
}

// Tests for NULL node voltage reading
void test_zero_node_voltage(void) {
  GPIOAddress gpio_addr = {
    .port = GPIO_PORT_A,
    .pin = 7,
  };
  ThermistorStorage storage;

  uint32_t reading = 0;
  TEST_ASSERT_OK(thermistor_init(&storage, gpio_addr, PROCEEDING));
  TEST_ASSERT_NOT_OK(thermistor_get_temp(&storage, &reading));
  LOG_DEBUG("Temperature: %" PRIuLEAST32 "\n", reading / 1000);
}
