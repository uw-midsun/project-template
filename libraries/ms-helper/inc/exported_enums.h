#pragma once

#include "can_msg_defs.h"

// This file stores enums which are exported between projects to allow both sides to use the same
// enums when sending and receiving CAN Messages over the primary network. To make things easier all
// enums in this file must follow a slightly modified naming convention.
//
// Example:
// typedef enum {
//   EE_<MY_CAN_MESSAGE_NAME>_<FIELD_NAME>_<VALUE> = 0,
//   // ...
//   NUM_EE_<MY_CAN_MESSAGE_NAME>_<FIELD_NAME>_<PLURAL>,
// } EE<MyCanMessageName><FieldName>

typedef enum {
  EE_CHARGER_SET_RELAY_STATE_OPEN = 0,
  EE_CHARGER_SET_RELAY_STATE_CLOSE,
  NUM_EE_CHARGER_SET_RELAY_STATES,
} EEChargerSetRelayState;

typedef enum {
  EE_CHARGER_CONN_STATE_DISCONNECTED = 0,
  EE_CHARGER_CONN_STATE_CONNECTED,
  NUM_EE_CHARGER_CONN_STATES,
} EEChargerConnState;

// Pedal output
// Mech brake + throttle
#define EE_PEDAL_OUTPUT_DENOMINATOR (1 << 12)
// Arbitrary 5% minimum pressure before considering it as engaged
#define EE_PEDAL_OUTPUT_MECH_THRESHOLD (5 * (EE_PEDAL_OUTPUT_DENOMINATOR) / 100)

typedef enum {
  EE_CONSOLE_OUTPUT_DIRECTION_NEUTRAL = 0,
  EE_CONSOLE_OUTPUT_DIRECTION_FORWARD,
  EE_CONSOLE_OUTPUT_DIRECTION_REVERSE,
  NUM_EE_CONSOLE_OUTPUT_DIRECTIONS,
} EEConsoleOutputDirection;

// Light type to be used with a SYSTEM_CAN_MESSAGE_LIGHTS_STATE message.
typedef enum EELightType {
  EE_LIGHT_TYPE_HIGH_BEAMS = 0,
  EE_LIGHT_TYPE_LOW_BEAMS,
  EE_LIGHT_TYPE_DRL,
  EE_LIGHT_TYPE_BRAKES,
  EE_LIGHT_TYPE_SIGNAL_RIGHT,
  EE_LIGHT_TYPE_SIGNAL_LEFT,
  EE_LIGHT_TYPE_SIGNAL_HAZARD,
  EE_LIGHT_TYPE_STROBE,
  NUM_EE_LIGHT_TYPES,
} EELightType;

// Light state to be used with a SYSTEM_CAN_MESSAGE_LIGHTS_STATE message.
typedef enum EELightState {
  EE_LIGHT_STATE_OFF = 0,  //
  EE_LIGHT_STATE_ON,       //
  NUM_EE_LIGHT_STATES,     //
} EELightState;

// Horn state, used with a SYSTEM_CAN_MESSAGE_HORN message.
typedef enum EEHornState {
  EE_HORN_STATE_OFF = 0,  //
  EE_HORN_STATE_ON,       //
  NUM_EE_HORN_STATES,     //
} EEHornState;

// Used with most _RELAY messages to request a relay state change.
typedef enum EERelayState {
  EE_RELAY_STATE_OPEN = 0,
  EE_RELAY_STATE_CLOSE,
  NUM_EE_RELAY_STATES,
} EERelayState;

// Used with the POWER_STATE message sent from driver controls to power distribution to request a
// state change.
typedef enum {
  EE_POWER_STATE_IDLE = 0,
  EE_POWER_STATE_CHARGE,
  EE_POWER_STATE_DRIVE,
  NUM_EE_POWER_STATES,
} EEPowerState;

// Used with the BPS heartbeat message
typedef enum {
  EE_BPS_HEARTBEAT_FAULT_SOURCE_KILLSWITCH = 0,
  EE_BPS_HEARTBEAT_FAULT_SOURCE_LTC_AFE_CELL,
  EE_BPS_HEARTBEAT_FAULT_SOURCE_LTC_AFE_TEMP,
  EE_BPS_HEARTBEAT_FAULT_SOURCE_LTC_AFE_FSM,
  EE_BPS_HEARTBEAT_FAULT_SOURCE_LTC_ADC,
  EE_BPS_HEARTBEAT_FAULT_SOURCE_ACK_TIMEOUT,
  NUM_EE_BPS_HEARTBEAT_FAULT_SOURCES,
} EEBpsHeartbeatFaultSource;

// BPS heartbeat bitset representing fault reason
typedef uint8_t EEBpsHeartbeatState;
#define EE_BPS_HEARTBEAT_STATE_OK 0x0
#define EE_BPS_HEARTBEAT_STATE_FAULT_KILLSWITCH (1 << EE_BPS_HEARTBEAT_FAULT_SOURCE_KILLSWITCH)
#define EE_BPS_HEARTBEAT_STATE_FAULT_LTC_AFE (1 << EE_BPS_HEARTBEAT_FAULT_SOURCE_LTC_AFE)
#define EE_BPS_HEARTBEAT_STATE_FAULT_LTC_AFE_FSM (1 << EE_BPS_HEARTBEAT_FAULT_SOURCE_LTC_AFE_FSM)
#define EE_BPS_HEARTBEAT_STATE_FAULT_LTC_ADC (1 << EE_BPS_HEARTBEAT_FAULT_SOURCE_LTC_ADC)
#define EE_BPS_HEARTBEAT_STATE_FAULT_ACK_TIMEOUT (1 << EE_BPS_HEARTBEAT_FAULT_SOURCE_ACK_TIMEOUT)

typedef enum {
  EE_FAN_CONTROL_STATE_ENABLE = 0,
  EE_FAN_CONTROL_STATE_DISABLE,
  NUM_EE_FAN_CONTROL_STATE,
} EEFanControlStates;

typedef enum {
  EE_POWER_DISTRIBUTION_FAULT_REASON_BPS_HB = 0,
  EE_POWER_DISTRIBUTION_FAULT_REASON_BPS_HB_WATCHDOG,
  EE_POWER_DISTRIBUTION_FAULT_REASON_POWERTRAIN_HB_WATCHDOG,
  EE_POWER_DISTRIBUTION_FAULT_REASON_RELAY_RETRY_EXPIRY,
  EE_POWER_DISTRIBUTION_FAULT_REASON_SEQUENCE_RETRY_EXPIRY,
  NUM_EE_POWER_DISTRIBUTION_FAULT_REASONS,
} EEPowerDistributionFaultReason;
