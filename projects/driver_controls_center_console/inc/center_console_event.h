#pragma once

// High priority messages in the event queue
typedef enum {
  CENTER_CONSOLE_EVENT_DUMMY = 0,
  NUM_CENTER_CONSOLE_EVENTS_CRITICAL,
} CenterConsoleEventsCritical;

// CAN related messages in the event queue
typedef enum {
  CENTER_CONSOLE_EVENT_CAN_FAULT = NUM_CENTER_CONSOLE_EVENTS_CRITICAL + 1,
  CENTER_CONSOLE_EVENT_CAN_RX,
  CENTER_CONSOLE_EVENT_CAN_TX,
  NUM_CENTER_CONSOLE_EVENTS_CAN,
} CenterConsoleEventsCAN;

// State transition messages in the event queue
typedef enum {
  CENTER_CONSOLE_EVENT_BUTTON_SET_STATE_OFF = NUM_CENTER_CONSOLE_EVENTS_CAN + 1,
  CENTER_CONSOLE_EVENT_BUTTON_SET_STATE_ON,
  NUM_CENTER_CONSOLE_EVENTS_BUTTON,
} CenterConsoleEventsButton;

typedef enum {
  CENTER_CONSOLE_EVENT_DRIVE_OUTPUT_DIRECTION_NEUTRAL = NUM_CENTER_CONSOLE_EVENTS_BUTTON + 1,
  CENTER_CONSOLE_EVENT_DRIVE_OUTPUT_DIRECTION_DRIVE,
  CENTER_CONSOLE_EVENT_DRIVE_OUTPUT_DIRECTION_REVERSE,
  NUM_CENTER_CONSOLE_EVENT_DRIVE_OUTPUT_DIRECTION,
} CenterConsoleEventsDirection;

typedef enum {
  CENTER_CONSOLE_EVENT_LIGHT_STATE_HIGH_BEAMS_ON =
      NUM_CENTER_CONSOLE_EVENT_DRIVE_OUTPUT_DIRECTION + 1,
  CENTER_CONSOLE_EVENT_LIGHT_STATE_HIGH_BEAMS_OFF,
  CENTER_CONSOLE_EVENT_LIGHT_STATE_LOW_BEAMS_ON,
  CENTER_CONSOLE_EVENT_LIGHT_STATE_LOW_BEAMS_OFF,
  CENTER_CONSOLE_EVENT_LIGHT_STATE_DRL_ON,
  CENTER_CONSOLE_EVENT_LIGHT_STATE_DRL_OFF,
  NUM_CENTER_CONSOLE_EVENTS_LIGHT_STATE,
} CenterConsoleEventsLightsState;
