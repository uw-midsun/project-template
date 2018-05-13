#pragma once
// This module provides configuration for lights_gpio when operating on the front board.

#include "lights_gpio.h"

// All the front board outputs.
typedef enum {
  LIGHTS_GPIO_CONFIG_FRONT_OUTPUT_HORN = 0,
  LIGHTS_GPIO_CONFIG_FRONT_OUTPUT_HIGH_BEAMS_RIGHT,
  LIGHTS_GPIO_CONFIG_FRONT_OUTPUT_HIGH_BEAMS_LEFT,
  LIGHTS_GPIO_CONFIG_FRONT_OUTPUT_LOW_BEAMS_RIGHT,
  LIGHTS_GPIO_CONFIG_FRONT_OUTPUT_LOW_BEAMS_LEFT,
  LIGHTS_GPIO_CONFIG_FRONT_OUTPUT_DRL_RIGHT,
  LIGHTS_GPIO_CONFIG_FRONT_OUTPUT_DRL_LEFT,
  LIGHTS_GPIO_CONFIG_FRONT_OUTPUT_SIDE_LEFT_INDICATOR,
  LIGHTS_GPIO_CONFIG_FRONT_OUTPUT_LEFT_TURN,
  LIGHTS_GPIO_CONFIG_FRONT_OUTPUT_SIDE_RIGHT_INDICATOR,
  LIGHTS_GPIO_CONFIG_FRONT_OUTPUT_RIGHT_TURN,
  NUM_LIGHTS_GPIO_CONFIG_FRONT_OUTPUTS,
} LightsGpioConfigFrontOutput;

// Loads configuration for front board.
LightsGpio *lights_config_front_load(void);
