#pragma once

// Define common typedefs for the various FSMs to use.

#include "event_queue.h"

// ID definitions for the driver input events.
typedef enum {
  INPUT_EVENT_POWER = 0,                        // Event IDs for driver input events
  INPUT_EVENT_PEDAL_BRAKE,
  INPUT_EVENT_PEDAL_COAST,
  INPUT_EVENT_PEDAL_PRESSED,
  INPUT_EVENT_CRUISE_CONTROL,
  INPUT_EVENT_CRUISE_CONTROL_INC,
  INPUT_EVENT_CRUISE_CONTROL_DEC,
  INPUT_EVENT_DIRECTION_SELECTOR_NEUTRAL,
  INPUT_EVENT_DIRECTION_SELECTOR_DRIVE,
  INPUT_EVENT_DIRECTION_SELECTOR_REVERSE,
  INPUT_EVENT_TURN_SIGNAL_NONE,
  INPUT_EVENT_TURN_SIGNAL_LEFT,
  INPUT_EVENT_TURN_SIGNAL_RIGHT,
  INPUT_EVENT_HAZARD_LIGHT,
  INPUT_EVENT_MECHANICAL_BRAKE_PRESSED,
  INPUT_EVENT_MECHANICAL_BRAKE_RELEASED,
  INPUT_EVENT_HORN,
  INPUT_EVENT_PUSH_TO_TALK,
  INPUT_EVENT_CAN_ID_POWER,                     // Event IDs for CAN output events
  INPUT_EVENT_CAN_ID_PEDAL,
  INPUT_EVENT_CAN_ID_DIRECTION_SELECTOR,
  INPUT_EVENT_CAN_ID_TURN_SIGNAL,
  INPUT_EVENT_CAN_ID_HAZARD_LIGHT,
  INPUT_EVENT_CAN_ID_MECHANICAL_BRAKE,
  INPUT_EVENT_CAN_ID_HORN,
  INPUT_EVENT_CAN_PUSH_TO_TALK,
  NUM_INPUT_EVENT
} InputEvent;

// 12 bits are used for holding the event data.
// 3 bits of the data field are used for holding the device state, while the last
// 1 bit to use as a flag for broadcasting the data bit
// TODO: Add digital/analog bit
typedef union InputEventData {
  uint16_t raw;
  struct {
    uint8_t digital:1;
    uint8_t state:3;
    uint16_t data:12;
  } components;
} InputEventData;
