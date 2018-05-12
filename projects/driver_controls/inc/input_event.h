#pragma once
// Shared events for FSMs
#include "event_queue.h"

// ID definitions for the driver input events.
typedef enum {
  INPUT_EVENT_DRIVE_WATCHDOG_FAULT = 0,
  INPUT_EVENT_CAN_RX,
  INPUT_EVENT_CAN_TX,
  INPUT_EVENT_CAN_FAULT,
  INPUT_EVENT_BPS_FAULT,
  // Mechanical brake must take precedence over power so pressing the brake then the power button
  // is handled properly
  INPUT_EVENT_MECHANICAL_BRAKE_PRESSED,
  INPUT_EVENT_DRIVE_UPDATE_REQUESTED,
  INPUT_EVENT_MECHANICAL_BRAKE_RELEASED,
  INPUT_EVENT_POWER,
  INPUT_EVENT_PEDAL_BRAKE,
  INPUT_EVENT_PEDAL_COAST,
  INPUT_EVENT_PEDAL_ACCEL,
  INPUT_EVENT_PEDAL_FAULT,
  INPUT_EVENT_CRUISE_CONTROL,
  INPUT_EVENT_CRUISE_CONTROL_INC,
  INPUT_EVENT_CRUISE_CONTROL_DEC,
  INPUT_EVENT_DIRECTION_SELECTOR_DRIVE,
  INPUT_EVENT_DIRECTION_SELECTOR_REVERSE,
  INPUT_EVENT_TURN_SIGNAL_NONE,
  INPUT_EVENT_TURN_SIGNAL_LEFT,
  INPUT_EVENT_TURN_SIGNAL_RIGHT,
  INPUT_EVENT_HAZARD_LIGHT,
  INPUT_EVENT_HORN,
  INPUT_EVENT_PUSH_TO_TALK,
  INPUT_EVENT_PEDAL_CALIBRATION_START,
  INPUT_EVENT_PEDAL_CALIBRATION_FULL_BRAKE,
  INPUT_EVENT_PEDAL_CALIBRATION_FULL_THROTTLE,
  INPUT_EVENT_PEDAL_CALIBRATION_ENTER_VALIDATION,
  INPUT_EVENT_PEDAL_CALIBRATION_CONTINUE_VALIDATION,
  NUM_INPUT_EVENTS
} InputEvent;
