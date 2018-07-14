#pragma once

typedef enum {
  PEDAL_EVENT_CAN_TX = 0,
  PEDAL_EVENT_CAN_RX,
  PEDAL_EVENT_CAN_FAULT,

  PEDAL_EVENT_THROTTLE_BRAKE,
  PEDAL_EVENT_THROTTLE_COAST,
  PEDAL_EVENT_THROTTLE_ACCEL,

  NUM_PEDAL_EVENTS
} PedalEvent;