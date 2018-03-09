#include <stdint.h>
#include <stdio.h>
#include "ads1015.h"
#include "ads1015_def.h"
#include "delay.h"
#include "gpio_it.h"
#include "i2c.h"
#include "interrupt.h"
#include "log.h"
#include "soft_timer.h"
#include "unity.h"

static Ads1015Storage s_storage;

// This function is registered as the callback for channels.
static void prv_callback(Ads1015Channel channel, void *context) {
  bool *callback_called = context;
  (*callback_called) = true;
}

static bool prv_channel_reading_valid(int16_t reading) {
  return (reading < (ADS1015_CURRENT_FSR / 2)) && (reading >= 0);
}

void setup_test(void) {
  gpio_init();
  interrupt_init();
  gpio_it_init();
  soft_timer_init();
  I2CSettings i2c_settings = {
    .speed = I2C_SPEED_FAST,                    //
    .scl = { .port = GPIO_PORT_B, .pin = 10 },  //
    .sda = { .port = GPIO_PORT_B, .pin = 11 },  //
  };
  i2c_init(TEST_ADS1015_I2C_PORT, &i2c_settings);
  GPIOAddress ready_pin = {
    .port = GPIO_PORT_B,  //
    .pin = 2,             //
  };
  ads1015_init(&s_storage, TEST_ADS1015_I2C_PORT, TEST_ADS1015_ADDR, &ready_pin);
}

void teardown_test(void) {}

void test_ads1015_init_invalid_input(void) {
  GPIOAddress ready_pin = {
    .port = GPIO_PORT_A,  //
    .pin = 0,             //
  };
  // Tests a basic use of the function
  TEST_ASSERT_EQUAL(STATUS_CODE_OK,
                    ads1015_init(&s_storage, TEST_ADS1015_I2C_PORT, TEST_ADS1015_ADDR, &ready_pin));
  // Tests for storage being a null pointer.
  TEST_ASSERT_EQUAL(STATUS_CODE_INVALID_ARGS,
                    ads1015_init(NULL, TEST_ADS1015_I2C_PORT, TEST_ADS1015_ADDR, &ready_pin));
  // Tests for ready pin being a null pointer.
  TEST_ASSERT_EQUAL(STATUS_CODE_INVALID_ARGS,
                    ads1015_init(&s_storage, TEST_ADS1015_I2C_PORT, TEST_ADS1015_ADDR, NULL));
}

void test_ads1015_config_channel_invalid_input(void) {
  // Tests a basic use of the function
  TEST_ASSERT_EQUAL(STATUS_CODE_OK, ads1015_configure_channel(&s_storage, ADS1015_CHANNEL_0, true,
                                                              prv_callback, &s_storage));
  // Tests disabling a channel.
  TEST_ASSERT_EQUAL(STATUS_CODE_OK, ads1015_configure_channel(&s_storage, ADS1015_CHANNEL_0, false,
                                                              prv_callback, &s_storage));
  // Tests enabling a channel with no callback (context has no effect).
  TEST_ASSERT_EQUAL(STATUS_CODE_OK, ads1015_configure_channel(&s_storage, ADS1015_CHANNEL_0, true,
                                                              NULL, &s_storage));
  // Tests for out of bound channel.
  TEST_ASSERT_EQUAL(
      STATUS_CODE_INVALID_ARGS,
      ads1015_configure_channel(&s_storage, NUM_ADS1015_CHANNELS, true, prv_callback, &s_storage));
  // Tests for s_storage being a null pointer.
  TEST_ASSERT_EQUAL(
      STATUS_CODE_INVALID_ARGS,
      ads1015_configure_channel(NULL, ADS1015_CHANNEL_1, true, prv_callback, &s_storage));
}

void test_ads1015_read_invalid_input(void) {
  int16_t reading = 0;

  ads1015_configure_channel(&s_storage, ADS1015_CHANNEL_0, true, NULL, &s_storage);
  ads1015_configure_channel(&s_storage, ADS1015_CHANNEL_1, true, NULL, &s_storage);
  ads1015_configure_channel(&s_storage, ADS1015_CHANNEL_2, true, NULL, &s_storage);
  ads1015_configure_channel(&s_storage, ADS1015_CHANNEL_3, true, NULL, &s_storage);
  // Tests a correct use of the function.
  TEST_ASSERT_EQUAL(STATUS_CODE_OK,
                    ads1015_read_converted(&s_storage, ADS1015_CHANNEL_0, &reading));
  TEST_ASSERT_EQUAL(STATUS_CODE_OK, ads1015_read_raw(&s_storage, ADS1015_CHANNEL_2, &reading));
  // Tests for s_storage being null.
  TEST_ASSERT_EQUAL(STATUS_CODE_INVALID_ARGS,
                    ads1015_read_converted(NULL, ADS1015_CHANNEL_2, &reading));
  TEST_ASSERT_EQUAL(STATUS_CODE_INVALID_ARGS, ads1015_read_raw(NULL, ADS1015_CHANNEL_2, &reading));
  // Tests for reading being null.
  TEST_ASSERT_EQUAL(STATUS_CODE_INVALID_ARGS,
                    ads1015_read_converted(&s_storage, ADS1015_CHANNEL_2, NULL));
  TEST_ASSERT_EQUAL(STATUS_CODE_INVALID_ARGS,
                    ads1015_read_raw(&s_storage, ADS1015_CHANNEL_2, NULL));

  // Tests for out of bound channel.
  TEST_ASSERT_EQUAL(STATUS_CODE_INVALID_ARGS,
                    ads1015_read_converted(&s_storage, NUM_ADS1015_CHANNELS, &reading));
  TEST_ASSERT_EQUAL(STATUS_CODE_INVALID_ARGS,
                    ads1015_read_raw(&s_storage, NUM_ADS1015_CHANNELS, &reading));
}

// This test checks if the callbacks are called properly for enabled channels.
void test_ads1015_channel_callback(void) {
  bool callback_called_0 = false;
  bool callback_called_1 = false;
  bool callback_called_2 = false;
  bool callback_called_3 = false;
  ads1015_configure_channel(&s_storage, ADS1015_CHANNEL_0, true, prv_callback, &callback_called_0);
  ads1015_configure_channel(&s_storage, ADS1015_CHANNEL_1, false, prv_callback, &callback_called_1);
  ads1015_configure_channel(&s_storage, ADS1015_CHANNEL_2, true, prv_callback, &callback_called_2);
  ads1015_configure_channel(&s_storage, ADS1015_CHANNEL_3, false, prv_callback, &callback_called_3);
  delay_ms(50);
  TEST_ASSERT_EQUAL(true, callback_called_0);
  TEST_ASSERT_EQUAL(false, callback_called_1);
  TEST_ASSERT_EQUAL(true, callback_called_2);
  TEST_ASSERT_EQUAL(false, callback_called_3);
}

// Tests enabling a channel after disabling.
void test_ads1015_disable_enable_channel(void) {
  int16_t reading = ADS1015_READ_UNSUCCESSFUL;

  ads1015_configure_channel(&s_storage, ADS1015_CHANNEL_1, false, NULL, &s_storage);
  ads1015_configure_channel(&s_storage, ADS1015_CHANNEL_1, true, NULL, &s_storage);
  delay_ms(50);
  ads1015_read_converted(&s_storage, ADS1015_CHANNEL_1, &reading);
  TEST_ASSERT_EQUAL(STATUS_CODE_OK,
                    ads1015_read_converted(&s_storage, ADS1015_CHANNEL_1, &reading));
}

// Tests enabling and disabling a channel right after the other.
void test_ads1015_enable_disable_channel(void) {
  int16_t reading = ADS1015_READ_UNSUCCESSFUL;

  ads1015_configure_channel(&s_storage, ADS1015_CHANNEL_1, true, NULL, &s_storage);
  ads1015_configure_channel(&s_storage, ADS1015_CHANNEL_1, false, NULL, &s_storage);
  delay_ms(50);
  TEST_ASSERT_EQUAL(STATUS_CODE_INVALID_ARGS,
                    ads1015_read_converted(&s_storage, ADS1015_CHANNEL_1, &reading));
}

// Tests disabling an already disabled channel and enabling after.
void test_ads1015_disable_already_disabled_channel(void) {
  int16_t reading = ADS1015_READ_UNSUCCESSFUL;

  ads1015_configure_channel(&s_storage, ADS1015_CHANNEL_2, false, NULL, &s_storage);
  delay_ms(50);
  TEST_ASSERT_EQUAL(STATUS_CODE_INVALID_ARGS,
                    ads1015_read_converted(&s_storage, ADS1015_CHANNEL_2, &reading));
  ads1015_configure_channel(&s_storage, ADS1015_CHANNEL_2, true, NULL, &s_storage);
  delay_ms(50);
  ads1015_read_converted(&s_storage, ADS1015_CHANNEL_2, &reading);
  TEST_ASSERT_TRUE(prv_channel_reading_valid(reading));
}

// Tests a case where every channel is enabled.
void test_ads1015_all_channels_enabled(void) {
  int16_t reading = ADS1015_READ_UNSUCCESSFUL;

  ads1015_configure_channel(&s_storage, ADS1015_CHANNEL_0, true, NULL, &s_storage);
  ads1015_configure_channel(&s_storage, ADS1015_CHANNEL_1, true, NULL, &s_storage);
  ads1015_configure_channel(&s_storage, ADS1015_CHANNEL_2, true, NULL, &s_storage);
  ads1015_configure_channel(&s_storage, ADS1015_CHANNEL_3, true, NULL, &s_storage);
  delay_ms(50);
  for (Ads1015Channel channel = 0; channel < NUM_ADS1015_CHANNELS; channel++) {
    ads1015_read_converted(&s_storage, channel, &reading);
    TEST_ASSERT_TRUE(prv_channel_reading_valid(reading));
    TEST_ASSERT_EQUAL(STATUS_CODE_OK, ads1015_read_converted(&s_storage, channel, &reading));
  }
}

// Tests if all channels are disabled if only ads1015_init is called.
void test_ads1015_start_with_all_channels_disabled(void) {
  int16_t reading = ADS1015_READ_UNSUCCESSFUL;

  for (Ads1015Channel channel = 0; channel < NUM_ADS1015_CHANNELS; channel++) {
    TEST_ASSERT_EQUAL(STATUS_CODE_INVALID_ARGS,
                      ads1015_read_converted(&s_storage, channel, &reading));
  }
}
