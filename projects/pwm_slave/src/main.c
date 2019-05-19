#include <stdint.h>

#include "pwm_input.h"
#include "delay.h"
#include "gpio.h"
#include "gpio_it.h"
#include "interrupt.h"
#include "interrupt_def.h"
#include "log.h"
#include "pwm.h"
#include "soft_timer.h"

// To run this test, connect the input and output pins

#define TEST_OUTPUT_PWM_TIMER PWM_TIMER_1
#define TEST_OUTPUT_PWM_ALTFN GPIO_ALTFN_2
#define TEST_OUTPUT_PWM_PERIOD_US 1000
#define TEST_OUTPUT_PWM_ADDR \
  { .port = GPIO_PORT_A, .pin = 8, }

#define TEST_INPUT_PWM_TIMER PWM_TIMER_3
#define TEST_INPUT_PWM_ALTFN GPIO_ALTFN_1
#define TEST_INPUT_PWM_CHANNEL PWM_CHANNEL_1
#define TEST_INPUT_PWM_ADDR \
  { .port = GPIO_PORT_A, .pin = 6, }

int main(void) {
  interrupt_init();
  soft_timer_init();
  gpio_init();

  PwmInputReading reading = { 0 };

  PwmTimer input_timer = TEST_INPUT_PWM_TIMER;

  GpioAddress input = TEST_INPUT_PWM_ADDR;

  GpioSettings input_settings = {
    .direction = GPIO_DIR_IN,
    .alt_function = TEST_INPUT_PWM_ALTFN,
  };

  gpio_init_pin(&input, &input_settings);
  pwm_input_init(input_timer, TEST_INPUT_PWM_CHANNEL);

  while (true) {
    pwm_input_get_reading(TEST_INPUT_PWM_TIMER, &reading);
    LOG_DEBUG("DC: %d | Period: %d\n", (int)reading.dc, (int)reading.period_us);
    delay_ms(500);
  }

  return 0;
}
