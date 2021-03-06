#pragma once

#include "event_queue.h"

// High priority messages in the event queue
typedef enum {
  PEDAL_EVENT_DUMMY = 0,

  // Hopefully never happens, but in case we're getting DOS'd by a series of
  // bad sensor readings or something, we can raise this as a high priority
  // message and hope for the best.
  PEDAL_EVENT_QUEUEING_FAULT,

  NUM_PEDAL_EVENTS_CRITICAL,
} PedalEventsCritical;

// CAN related messages in the event queue
typedef enum {
  PEDAL_EVENT_CAN_FAULT = NUM_PEDAL_EVENTS_CRITICAL + 1,
  PEDAL_EVENT_CAN_RX,
  PEDAL_EVENT_CAN_TX,
  NUM_PEDAL_EVENTS_CAN
} PedalEventsCAN;

// State transition messages in the event queue
typedef enum {
  PEDAL_EVENT_INPUT_MECH_BRAKE_PRESSED = NUM_PEDAL_EVENTS_CAN + 1,
  PEDAL_EVENT_INPUT_MECH_BRAKE_RELEASED,

  PEDAL_EVENT_INPUT_BPS_FAULT,
  PEDAL_EVENT_INPUT_RETRY_POWER_STATE,
  PEDAL_EVENT_INPUT_PEDAL_FAULT,
  PEDAL_EVENT_INPUT_PEDAL_WATCHDOG_FAULT,

  PEDAL_EVENT_INPUT_PEDAL_COAST,
  PEDAL_EVENT_INPUT_PEDAL_ACCEL,
  PEDAL_EVENT_INPUT_PEDAL_BRAKE,

  PEDAL_EVENT_INPUT_DIRECTION_STATE_FORWARD,
  PEDAL_EVENT_INPUT_DIRECTION_STATE_REVERSE,
  PEDAL_EVENT_INPUT_DIRECTION_STATE_NEUTRAL,

  PEDAL_EVENT_INPUT_POWER_STATE_DRIVE,
  PEDAL_EVENT_INPUT_POWER_STATE_CHARGE,
  PEDAL_EVENT_INPUT_POWER_STATE_OFF,
  PEDAL_EVENT_INPUT_POWER_STATE_FAULT,

  PEDAL_EVENT_INPUT_MECHANICAL_BRAKE_PRESSED,
  PEDAL_EVENT_INPUT_MECHANICAL_BRAKE_RELEASED,

  PEDAL_EVENT_INPUT_SPEED_UPDATE,
  PEDAL_EVENT_INPUT_DRIVE_UPDATE_REQUESTED,

  PEDAL_EVENT_INPUT_HAZARDS_STATE_ON,
  PEDAL_EVENT_INPUT_HAZARDS_STATE_OFF,

  // Headlight States
  PEDAL_EVENT_INPUT_HEADLIGHT_STATE_OFF,
  PEDAL_EVENT_INPUT_HEADLIGHT_STATE_DRL,
  PEDAL_EVENT_INPUT_HEADLIGHT_STATE_LOWBEAM,
  PEDAL_EVENT_INPUT_HEADLIGHT_STATE_HIGHBEAM,

  NUM_PEDAL_EVENT_INPUT,
} PedalEventsInput;

// Steering Board
// Digital events raised as a result of input from the Control Stalk
typedef enum {
  PEDAL_EVENT_INPUT_CONTROL_STALK_DIGITAL_HORN_PRESSED = NUM_PEDAL_EVENT_INPUT + 1,
  PEDAL_EVENT_INPUT_CONTROL_STALK_DIGITAL_HORN_RELEASED,

  PEDAL_EVENT_INPUT_CONTROL_STALK_DIGITAL_CC_ON,
  PEDAL_EVENT_INPUT_CONTROL_STALK_DIGITAL_CC_OFF,

  PEDAL_EVENT_INPUT_CONTROL_STALK_DIGITAL_CC_SET_PRESSED,
  PEDAL_EVENT_INPUT_CONTROL_STALK_DIGITAL_CC_SET_RELEASED,

  NUM_PEDAL_EVENTS_CONTROL_STALK_DIGITAL,
} PedalEventsControlStalkDigital;

// Analog events raised as a result of input from the Control Stalk
typedef enum {
  PEDAL_EVENT_INPUT_CONTROL_STALK_ANALOG_TURN_SIGNAL_NONE =
      NUM_PEDAL_EVENTS_CONTROL_STALK_DIGITAL + 1,
  PEDAL_EVENT_INPUT_CONTROL_STALK_ANALOG_TURN_SIGNAL_RIGHT,
  PEDAL_EVENT_INPUT_CONTROL_STALK_ANALOG_TURN_SIGNAL_LEFT,

  PEDAL_EVENT_INPUT_CONTROL_STALK_ANALOG_CC_SPEED_NEUTRAL,
  PEDAL_EVENT_INPUT_CONTROL_STALK_ANALOG_CC_SPEED_MINUS,
  PEDAL_EVENT_INPUT_CONTROL_STALK_ANALOG_CC_SPEED_PLUS,

  PEDAL_EVENT_INPUT_CONTROL_STALK_ANALOG_CC_NEUTRAL,
  PEDAL_EVENT_INPUT_CONTROL_STALK_ANALOG_CC_CANCEL,
  PEDAL_EVENT_INPUT_CONTROL_STALK_ANALOG_CC_RESUME,

  PEDAL_EVENT_INPUT_CONTROL_STALK_ANALOG_CC_DISTANCE_NEUTRAL,
  PEDAL_EVENT_INPUT_CONTROL_STALK_ANALOG_CC_DISTANCE_MINUS,
  PEDAL_EVENT_INPUT_CONTROL_STALK_ANALOG_CC_DISTANCE_PLUS,

  NUM_PEDAL_EVENTS_CONTROL_STALK_ANALOG,
} PedalEventsControlStalkAnalog;

// Center Console
typedef enum {
  PEDAL_EVENT_INPUT_CENTER_CONSOLE_POWER_PRESSED = NUM_PEDAL_EVENTS_CONTROL_STALK_ANALOG + 1,

  PEDAL_EVENT_INPUT_CENTER_CONSOLE_DIRECTION_DRIVE,
  PEDAL_EVENT_INPUT_CENTER_CONSOLE_DIRECTION_NEUTRAL,
  PEDAL_EVENT_INPUT_CENTER_CONSOLE_DIRECTION_REVERSE,

  PEDAL_EVENT_INPUT_CENTER_CONSOLE_DRL,
  PEDAL_EVENT_INPUT_CENTER_CONSOLE_LOWBEAMS,

  PEDAL_EVENT_INPUT_CENTER_CONSOLE_HAZARDS_PRESSED,
  // PEDAL_EVENT_INPUT_CENTER_CONSOLE_HAZARDS_RELEASED,
} PedalEventsCenterConsole;
