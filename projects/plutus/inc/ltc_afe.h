#pragma once
// driver for LTC6804-1 AFE chip
// assumes that:
// - a 16 channel analog MUX is attached to the GPIO outputs
// - GPIO2, GPIO3, GPIO4, GPIO5 are used as AUX channel select outputs
// - GPIO1 is used as a thermistor input
// requires GPIO, Interrupts and Soft Timers to be initialized

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "gpio.h"
#include "spi.h"
#include "status.h"

#define LTC6804_CELLS_PER_DEVICE 12

// - 12-bit, 16-bit and 24-bit values are little endian
// - commands and PEC are big endian
#define SWAP_UINT16(x) (uint16_t)(((uint16_t)(x) >> 8) | ((uint16_t)(x) << 8))

#if defined(__GNUC__)
#define _PACKED __attribute__((packed))
#else
#define _PACKED
#endif

// select the ADC mode (trade-off between speed or minimizing noise)
// see p.50 for conversion times and p.23 for noise
typedef enum {
  LTC_AFE_ADC_MODE_27KHZ = 0,
  LTC_AFE_ADC_MODE_7KHZ,
  LTC_AFE_ADC_MODE_26HZ,
  LTC_AFE_ADC_MODE_14KHZ,
  LTC_AFE_ADC_MODE_3KHZ,
  LTC_AFE_ADC_MODE_2KHZ,
  NUM_LTC_AFE_ADC_MODE
} LtcAfeAdcMode;

typedef struct LtcAfeSettings {
  GPIOAddress cs;
  GPIOAddress mosi;
  GPIOAddress miso;
  GPIOAddress sclk;

  const SPIPort spi_port;
  uint32_t spi_baudrate;

  LtcAfeAdcMode adc_mode;
} LtcAfeSettings;

// initialize the LTC6804
StatusCode ltc_afe_init(const LtcAfeSettings *afe);

// read all voltages
// result is an array of size LTC6804_CELLS_PER_DEVICE *
// LTC_AFE_DEVICES_IN_CHAIN
// len should be SIZEOF_ARRAY(result)
StatusCode ltc_afe_read_all_voltage(const LtcAfeSettings *afe, uint16_t *result, size_t len);

// read all auxiliary registers
// result should be an array of size LTC6804_CELLS_PER_DEVICE *
// LTC_AFE_DEVICES_IN_CHAIN
// len should be SIZEOF_ARRAY(result)
StatusCode ltc_afe_read_all_aux(const LtcAfeSettings *afe, uint16_t *result, size_t len);

// mark cells for discharging (takes effect after config is re-written)
StatusCode ltc_afe_toggle_discharge_cells(const LtcAfeSettings *afe, uint16_t cell, bool discharge);
