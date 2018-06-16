#pragma once

// LTC 2484 calibration module

#include <assert.h>
#include "current_sense.h"

// Must be less than 128, or overflow may occur
#define CURRENT_CALIBRATION_SAMPLES 10

// Window for moving offset average
#define CURRENT_CALIBRATION_OFFSET_WINDOW 10

static_assert(CURRENT_CALIBRATION_SAMPLES < 128, "Sample limit too large. May cause overflow");

typedef struct {
  LtcAdcStorage *adc_storage;
  LtcAdcSettings *settings;
  int32_t voltage;
  volatile uint8_t samples;

  int32_t buffer[CURRENT_CALIBRATION_OFFSET_WINDOW];
  uint8_t index;
  uint8_t num_chip_resets;
} CurrentCalibrationStorage;

// Initialize current calibration
StatusCode current_calibration_init(CurrentCalibrationStorage *storage, LtcAdcStorage *adc_storage,
                                    LtcAdcSettings *adc_settings);

// Samples adc readings at specified current(milliamps) in order to obtain data for two-point
// calibration. Function will block until completion. For optimal results, make
// sure the points are as far apart as possible
StatusCode current_calibration_sample_point(CurrentCalibrationStorage *storage,
                                            CurrentSenseValue *point, int32_t current);

// The zero point for the chip seems to change on chip reset. The offset will be updated according
// the moving average kept by CurrentCalibrationStorage
StatusCode current_calibration_zero_reset(CurrentCalibrationStorage *storage,
                                          CurrentSenseValue *zero_point);
