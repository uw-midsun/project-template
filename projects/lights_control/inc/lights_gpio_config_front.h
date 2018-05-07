#pragma once

#include "lights_gpio.h"
// This module holds the storage for lights_gpio when operating on the front board.

// All the front board peripherals.
typedef enum {
  LIGHTS_GPIO_CONFIG_FRONT_PERIPHERAL_HORN = 0,              //
  LIGHTS_GPIO_CONFIG_FRONT_PERIPHERAL_HIGH_BEAMS_RIGHT,      //
  LIGHTS_GPIO_CONFIG_FRONT_PERIPHERAL_HIGH_BEAMS_LEFT,       //
  LIGHTS_GPIO_CONFIG_FRONT_PERIPHERAL_LOW_BEAMS_RIGHT,       //
  LIGHTS_GPIO_CONFIG_FRONT_PERIPHERAL_LOW_BEAMS_LEFT,        //
  LIGHTS_GPIO_CONFIG_FRONT_PERIPHERAL_DRL_RIGHT,             //
  LIGHTS_GPIO_CONFIG_FRONT_PERIPHERAL_DRL_LEFT,              //
  LIGHTS_GPIO_CONFIG_FRONT_PERIPHERAL_SIDE_LEFT_INDICATOR,   //
  LIGHTS_GPIO_CONFIG_FRONT_PERIPHERAL_LEFT_TURN,             //
  LIGHTS_GPIO_CONFIG_FRONT_PERIPHERAL_SIDE_RIGHT_INDICATOR,  //
  LIGHTS_GPIO_CONFIG_FRONT_PERIPHERAL_RIGHT_TURN,            //
  NUM_LIGHTS_GPIO_CONFIG_FRONT_PERIPHERALS,                  //
} LightsGPIOConfigFrontPeripheral;

// Config for front board.
LightsGPIO *lights_config_front_load(void);
