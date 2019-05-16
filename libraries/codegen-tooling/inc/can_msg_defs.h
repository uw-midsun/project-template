#pragma once

#include <stdbool.h>

#include "can_msg.h"

// For setting the CAN device
typedef enum {
  SYSTEM_CAN_DEVICE_RESERVED = 0,
  SYSTEM_CAN_DEVICE_PLUTUS = 1,
  SYSTEM_CAN_DEVICE_PLUTUS_SLAVE = 2,
  SYSTEM_CAN_DEVICE_CHAOS = 3,
  SYSTEM_CAN_DEVICE_TELEMETRY = 4,
  SYSTEM_CAN_DEVICE_LIGHTS_FRONT = 5,
  SYSTEM_CAN_DEVICE_LIGHTS_REAR = 6,
  SYSTEM_CAN_DEVICE_MOTOR_CONTROLLER = 7,
  SYSTEM_CAN_DEVICE_DRIVER_CONTROLS_CENTER_CONSOLE = 8,
  SYSTEM_CAN_DEVICE_DRIVER_CONTROLS_STEERING = 9,
  SYSTEM_CAN_DEVICE_SOLAR_MASTER_FRONT = 10,
  SYSTEM_CAN_DEVICE_SOLAR_MASTER_REAR = 11,
  SYSTEM_CAN_DEVICE_SENSOR_BOARD = 12,
  SYSTEM_CAN_DEVICE_CHARGER = 13,
  SYSTEM_CAN_DEVICE_DRIVER_CONTROLS_PEDAL = 14,
  NUM_SYSTEM_CAN_DEVICES = 15
} SystemCanDevice;

// For setting the CAN message ID
typedef enum {
  SYSTEM_CAN_MESSAGE_BPS_HEARTBEAT = 0,
  SYSTEM_CAN_MESSAGE_POWER_DISTRIBUTION_FAULT = 1,
  SYSTEM_CAN_MESSAGE_BATTERY_RELAY_MAIN = 2,
  SYSTEM_CAN_MESSAGE_BATTERY_RELAY_SLAVE = 3,
  SYSTEM_CAN_MESSAGE_MOTOR_RELAY = 4,
  SYSTEM_CAN_MESSAGE_SOLAR_RELAY_REAR = 5,
  SYSTEM_CAN_MESSAGE_SOLAR_RELAY_FRONT = 6,
  SYSTEM_CAN_MESSAGE_POWER_STATE = 7,
  SYSTEM_CAN_MESSAGE_POWERTRAIN_HEARTBEAT = 8,
  SYSTEM_CAN_MESSAGE_MOTOR_CONTROLLER_RESET = 15,
  SYSTEM_CAN_MESSAGE_OVUV_DCDC_AUX = 16,
  SYSTEM_CAN_MESSAGE_MC_ERROR_LIMITS = 17,
  SYSTEM_CAN_MESSAGE_DRIVE_OUTPUT = 18,
  SYSTEM_CAN_MESSAGE_FAN_CONTROL = 20,
  SYSTEM_CAN_MESSAGE_SET_DISCHARGE_BITSET = 21,
  SYSTEM_CAN_MESSAGE_DISCHARGE_STATE = 22,
  SYSTEM_CAN_MESSAGE_LIGHTS_SYNC = 23,
  SYSTEM_CAN_MESSAGE_LIGHTS_STATE = 24,
  SYSTEM_CAN_MESSAGE_HORN = 25,
  SYSTEM_CAN_MESSAGE_CHARGER_CONN_STATE = 26,
  SYSTEM_CAN_MESSAGE_CHARGER_SET_RELAY_STATE = 27,
  SYSTEM_CAN_MESSAGE_STEERING_OUTPUT = 28,
  SYSTEM_CAN_MESSAGE_PEDAL_OUTPUT = 29,
  SYSTEM_CAN_MESSAGE_BATTERY_SOC = 31,
  SYSTEM_CAN_MESSAGE_BATTERY_VT = 32,
  SYSTEM_CAN_MESSAGE_BATTERY_AGGREGATE_VC = 33,
  SYSTEM_CAN_MESSAGE_MOTOR_CONTROLLER_VC = 35,
  SYSTEM_CAN_MESSAGE_MOTOR_VELOCITY = 36,
  SYSTEM_CAN_MESSAGE_MOTOR_DEBUG = 37,
  SYSTEM_CAN_MESSAGE_MOTOR_TEMPS = 38,
  SYSTEM_CAN_MESSAGE_MOTOR_AMP_HR = 39,
  SYSTEM_CAN_MESSAGE_ODOMETER = 40,
  SYSTEM_CAN_MESSAGE_AUX_DCDC_VC = 43,
  SYSTEM_CAN_MESSAGE_DCDC_TEMPS = 44,
  SYSTEM_CAN_MESSAGE_SOLAR_DATA_FRONT = 45,
  SYSTEM_CAN_MESSAGE_SOLAR_DATA_REAR = 46,
  SYSTEM_CAN_MESSAGE_CHARGER_INFO = 47,
  SYSTEM_CAN_MESSAGE_LINEAR_ACCELERATION = 51,
  SYSTEM_CAN_MESSAGE_ANGULAR_ROTATION = 52,
  NUM_SYSTEM_CAN_MESSAGES = 39
} SystemCanMessage;
