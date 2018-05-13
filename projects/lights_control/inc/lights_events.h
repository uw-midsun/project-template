#pragma once

// Every event is post-fixed by the module that processes it.
// i.e. LIGHTS_EVENT_GPIO* is used by the lights_gpio module.

typedef enum {
  // Used internally by the can module.
  LIGHTS_EVENT_CAN_RX = 0,
  LIGHTS_EVENT_CAN_TX,
  LIGHTS_EVENT_CAN_FAULT,
  // Events processed by the lights_gpio module.
  LIGHTS_EVENT_GPIO_OFF,
  LIGHTS_EVENT_GPIO_ON,
  NUM_LIGHTS_EVENTS
} LightsEvent;

// Possible data fields to be used with a LIGHTS_EVENT_GPIO* event.
typedef enum {
  LIGHTS_EVENT_GPIO_PERIPHERAL_HORN = 0,
  LIGHTS_EVENT_GPIO_PERIPHERAL_HIGH_BEAMS,
  LIGHTS_EVENT_GPIO_PERIPHERAL_LOW_BEAMS,
  LIGHTS_EVENT_GPIO_PERIPHERAL_DRL,
  LIGHTS_EVENT_GPIO_PERIPHERAL_SIGNAL_HAZARD,
  LIGHTS_EVENT_GPIO_PERIPHERAL_SIGNAL_LEFT,
  LIGHTS_EVENT_GPIO_PERIPHERAL_SIGNAL_RIGHT,
  LIGHTS_EVENT_GPIO_PERIPHERAL_STROBE,
  LIGHTS_EVENT_GPIO_PERIPHERAL_BRAKES,
  LIGHTS_EVENT_GPIO_PERIPHERAL_SYNC,
  NUM_LIGHTS_EVENT_GPIO_PERIPHERALS
} LightsEventGPIOPeripheral;