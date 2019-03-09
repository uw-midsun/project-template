#pragma once
// Shared events for FSMs
#include "event_queue.h"

// ID definitions for the console input events.
typedef enum {
  INPUT_EVENT_STEERING_WATCHDOG_FAULT = 0,
  INPUT_EVENT_STEERING_CAN_RX,
  INPUT_EVENT_STEERING_CAN_TX,
  INPUT_EVENT_STEERING_CAN_FAULT,
  INPUT_EVENT_STEERING_POWER_STATE_OFF,
  INPUT_EVENT_STEERING_POWER_STATE_FAULT,
  INPUT_EVENT_STEERING_UPDATE_REQUESTED,

  // Control Stalk analog events
  INPUT_EVENT_CONTROL_STALK_ANALOG_DISTANCE_NEUTRAL,
  INPUT_EVENT_CONTROL_STALK_ANALOG_DISTANCE_MINUS,
  INPUT_EVENT_CONTROL_STALK_ANALOG_DISTANCE_PLUS,
  INPUT_EVENT_CONTROL_STALK_ANALOG_CC_SPEED_NEUTRAL,
  INPUT_EVENT_CONTROL_STALK_ANALOG_CC_SPEED_MINUS,
  INPUT_EVENT_CONTROL_STALK_ANALOG_CC_SPEED_PLUS,
  // ambiguous - depends on digital state
  INPUT_EVENT_CONTROL_STALK_ANALOG_CC_DIGITAL,
  INPUT_EVENT_CONTROL_STALK_ANALOG_CC_CANCEL,
  INPUT_EVENT_CONTROL_STALK_ANALOG_CC_RESUME,
  INPUT_EVENT_CONTROL_STALK_ANALOG_TURN_SIGNAL_NONE,
  INPUT_EVENT_CONTROL_STALK_ANALOG_TURN_SIGNAL_RIGHT,
  INPUT_EVENT_CONTROL_STALK_ANALOG_TURN_SIGNAL_LEFT,

  // Control Stalk digital events
  INPUT_EVENT_CONTROL_STALK_DIGITAL_CC_SET_PRESSED,
  INPUT_EVENT_CONTROL_STALK_DIGITAL_CC_SET_RELEASED,
  INPUT_EVENT_CONTROL_STALK_DIGITAL_CC_ON,
  INPUT_EVENT_CONTROL_STALK_DIGITAL_CC_OFF,
  INPUT_EVENT_CONTROL_STALK_DIGITAL_CC_LANE_ASSIST_PRESSED,
  INPUT_EVENT_CONTROL_STALK_DIGITAL_CC_LANE_ASSIST_RELEASED,
  INPUT_EVENT_CONTROL_STALK_DIGITAL_HEADLIGHT_FWD_PRESSED,
  INPUT_EVENT_CONTROL_STALK_DIGITAL_HEADLIGHT_FWD_RELEASED,
  INPUT_EVENT_CONTROL_STALK_DIGITAL_HEADLIGHT_BACK_PRESSED,
  INPUT_EVENT_CONTROL_STALK_DIGITAL_HEADLIGHT_BACK_RELEASED,
  INPUT_EVENT_CONTROL_STALK_DIGITAL_HORN_PRESSED,
  INPUT_EVENT_CONTROL_STALK_DIGITAL_HORN_RELEASED,

  // Speed update
  INPUT_EVENT_SPEED_UPDATE,

  // Steering Pedal Events
  INPUT_EVENT_STEERING_PEDAL_BRAKE,
  INPUT_EVENT_STEERING_PEDAL_ACCEL,

  // Steering Power state events
  INPUT_EVENT_STEERING_POWER_STATE_OFF,
  INPUT_EVENT_STEERING_POWER_STATE_DRIVE,
  INPUT_EVENT_STEERING_POWER_STATE_CHARGE,
  INPUT_EVENT_STEERING_POWER_STATE_FAULT,

  // Steering Mech Brake events
  INPUT_EVENT_STEERING_MECHANICAL_BRAKE_PRESSED,
  INPUT_EVENT_STEERING_MECHANICAL_BRAKE_RELEASED,
<<<<<<< HEAD
=======
  INPUT_EVENT_STEERING_PEDAL_BRAKE,
  INPUT_EVENT_STEERING_PEDAL_COAST,
  INPUT_EVENT_STEERING_PEDAL_ACCEL,
  INPUT_EVENT_STEERING_PEDAL_FAULT,
>>>>>>> 544480fc4813ebdf72f2afcf6bcf5b8541abeae2

  NUM_INPUT_EVENTS,
} InputEvent;
