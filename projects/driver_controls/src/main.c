
#include <stdint.h>
#include <stdio.h>

#include <stdbool.h>
#include "adc.h"
#include "ads1015.h"
#include "ads1015_def.h"
#include "delay.h"
#include "event_arbiter.h"
#include "event_queue.h"
#include "gpio_it.h"
#include "i2c.h"
#include "input_event.h"
#include "interrupt.h"
#include "log.h"
#include "magnetic_brake_event_generator.h"
#include "soft_timer.h"
#include "status.h"
#include "unity.h"

static MagneticCalibrationData data;

static MagneticBrakeSettings brake_settings = {
  .percentage_threshold = 500,
  .zero_value = 513,
  .hundred_value = 624,
  .min_allowed_range = 0,
  .max_allowed_range = (1 << 12),
};

static void prv_callback_channel(Ads1015Channel channel, void *context) {
  Ads1015Storage *storage = context;

  ads1015_read_raw(storage, channel, &data.reading);

  int16_t percentage = percentage_converter(&data, &brake_settings);
  int16_t reading = data.reading;

  printf("%d %d\n", data.reading, percentage);

  data.percentage = percentage;
}

int main(void) {
  
  gpio_init();
  interrupt_init();
  gpio_it_init();
  soft_timer_init();

  I2CSettings i2c_settings = {
    .speed = I2C_SPEED_FAST,
    .scl = { .port = GPIO_PORT_B, .pin = 8 },
    .sda = { .port = GPIO_PORT_B, .pin = 9 },
  };

  i2c_init(I2C_PORT_1, &i2c_settings);

  GPIOAddress ready_pin = { .port = GPIO_PORT_A, .pin = 10 };

  ads1015_init(data.mech_brake_storage, I2C_PORT_1, ADS1015_ADDRESS_GND, &ready_pin);

  LOG_DEBUG("Brake sensor is calibrating, Please ensure the brake is not being pressed, wait \n "
         "for response to continue");
  delay_s(5);
  LOG_DEBUG("Beginning sampling\n");
  //magnetic_brake_calibration(data.percentage, brake_settings.min_allowed_range, &brake_settings.zero_value,ADS1015_CHANNEL_2, &brake_settings);
  LOG_DEBUG("Completed sampling\n");
  LOG_DEBUG("Initial calibration complete, Please press and hold the brake \n"
         "wait for response to continue");
  delay_s(5);
  LOG_DEBUG("Beginning sampling\n");
  //magnetic_brake_calibration(data.percentage, brake_settings.max_allowed_range,&brake_settings.hundred_value,ADS1015_CHANNEL_2, &brake_settings);
  LOG_DEBUG("Completed sampling\n");

  //ads1015_configure_channel(&storage, ADS1015_CHANNEL_2, true, prv_callback_channel, &storage);

  while (true) { 
  }

  return 0;
}
