#include "adc.h"
#include "gpio.h"
#include "stm32f0xx.h"  
#include "unity.h"
#include <stdio.h>

static GPIOAddress address = { 0, 0 };

void setup_test() {
  GPIOSettings settings = { GPIO_DIR_IN, GPIO_STATE_LOW, GPIO_RES_NONE, GPIO_ALTFN_ANALOG };
  
  gpio_init();
  gpio_init_pin(&address, &settings);
}

void test_single() {
  adc_init(ADC_MODE_SINGLE);
  adc_init_pin(&address, ADC_SAMPLE_RATE_1);
  uint16_t reading;

  // Ensure that converted values are within the required range
  for (uint8_t i = 0; i < 8; i++) {
    reading = adc_read(&address, 3000);
    TEST_ASSERT_TRUE((reading >= 0) && (reading <= 3000));
  }
  
  // Read directly from the register. Should stay constant despite changes with the input
  // (Analog input should be connected for this test)
  
  /*
  for (uint32_t i = 0; i < 2000; i++) {
    printf("Single Read #%d = %d\n", i, reading, 3000*ADC1->DR/4095);
  }
  */

  // Disable ADC for continuous test
  ADC_StopOfConversion(ADC1);
  while (ADC_GetFlagStatus(ADC1, ADC_FLAG_ADSTP)) {}

  ADC1->CR |= ADC_FLAG_ADDIS;
  while (ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN)) {}
}

void test_continuous() {
  adc_init(ADC_MODE_CONTINUOUS);
  adc_init_pin(&address, ADC_SAMPLE_RATE_1);
  uint16_t reading;

  // Ensure that converted values are within the required range
  for (uint8_t i = 0; i < 8; i++) {
    reading = adc_read(&address, 3000);
    TEST_ASSERT_TRUE((reading >= 0) && (reading <= 4096));
  }

  
  // Read directly from the register. Value should change with the connected input
  // (Analog input should be connected for this test)
  /*
  for (uint32_t i = 0; i < 2000; i++) {
    printf("Continuous Read #%d =  %d - %d\n", i, reading, 3000*ADC1->DR/4095);
  }
  */
}

void teardown_test(void) { }

