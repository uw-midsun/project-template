#pragma once

// Module for the mechanical brake.
// Requires ADS1015 and soft timers to be initialized.
//
// The module reads brake inputs from ADS1015, then converts those readings into a numerator value
// with EE_PEDAL_OUTPUT_DENOMINATOR as the denominator. This numerator value is called the position.
// At the same time, it raises events, INPUT_EVENT_MECHANICAL_BRAKE_PRESSED, and
// INPUT_EVENT_MECHANICAL_BRAKE_UNPRESSED. These events contain the numerator value that corresponds
// to the input reading of the ADC. We use a windowed reading such that once the brake is pressed it
// must pass a threshold to be considered as braking. Once released it must be released further than
// the initial threshold. This mechanism mitigates fluttering about the threshold.
//
// For the LSB to position conversion, the module receives peak-peak values when the brake is
// pressed and released, then correlates that data to a position value and generates a linear
// equation between the LSB input and position.

#include <stdbool.h>
#include <stdint.h>

#include "ads1015.h"
#include "soft_timer.h"
#include "status.h"

typedef struct MechBrakeCalibrationData {
  int16_t zero_value;
  int16_t hundred_value;
} MechBrakeCalibrationData;

typedef struct MechBrakeSettings {
  Ads1015Storage *ads1015;
  // Percentage value above which the brake_pressed event is raised.
  int16_t brake_pressed_threshold_percentage;
  // Percentage value below which the brake_unpressed event is raised.
  int16_t brake_unpressed_threshold_percentage;
  // Percentage tolerance for the lower and upper bound of the position.
  int16_t bounds_tolerance_percentage;
  Ads1015Channel channel;
} MechBrakeSettings;

typedef struct MechBrakeStorage {
  MechBrakeCalibrationData calibration_data;
  Ads1015Storage *ads1015;
  Ads1015Channel channel;
  // Minimun value of the position based on the tolerance value.
  int16_t lower_bound;
  // Maximum value of the position based on the tolerance value.
  int16_t upper_bound;
  // Position pressed value calculated using the brake_pressed_threshold_percentage.
  int16_t pressed_threshold_position;
  // Position unpressed value calculated using the brake_unpressed_threshold_percentage.
  int16_t unpressed_threshold_position;
  bool prev_pressed;
} MechBrakeStorage;

// Initializes the mech brake by configuring the ADS1015 channel.
// Calculates the upper and lower bound of the position.
StatusCode mech_brake_init(MechBrakeStorage *mech_brake_storage, const MechBrakeSettings *settings,
                           const MechBrakeCalibrationData *calib_data);

// Gets the current position of the mech_brake and writes that value to *position
StatusCode mech_brake_get_position(MechBrakeStorage *storage, int16_t *position);

// Returns a pointer to the global throttle storage.
// Note that this only exists because our FSMs already use their context pointers for event arbiters
MechBrakeStorage *mech_brake_global(void);
