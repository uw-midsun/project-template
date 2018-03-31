#pragma once

typedef enum {
  CHARGER_EVENT_CAN_FAULT = 0,
  CHARGER_EVENT_CAN_TX,
  CHARGER_EVENT_CAN_RX,
  CHARGER_EVENT_STOP_CHARGING,
  CHARGER_EVENT_START_CHARGING,
  CHARGER_EVENT_DISCONNECTED,
  CHARGER_EVENT_CONNECTED,
} ChargerEvents;
