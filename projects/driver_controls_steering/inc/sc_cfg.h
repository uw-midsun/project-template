#pragma once
// Steering controls config file

// #define SC_CFG_DISABLE_CONTROL_STALK
// #define SC_CFG_DEBUG_PRINT_EVENTS

#define SC_CFG_CAN_DEVICE_ID SYSTEM_CAN_DEVICE_STEERING_CONTROLS
#define SC_CFG_CAN_BITRATE CAN_HW_BITRATE_500KBPS

#define CC_CFG_CAN_RX \
  { GPIO_PORT_A, 11 }
#define CC_CFG_CAN_TX \
  { GPIO_PORT_A, 12 }

#define SC_CFG_I2C_BUS_PORT I2C_PORT_1
#define SC_CFG_I2C_BUS_SDA \
  { GPIO_PORT_B, 9 }
#define SC_CFG_I2C_BUS_SCL \
  { GPIO_PORT_B, 8 }