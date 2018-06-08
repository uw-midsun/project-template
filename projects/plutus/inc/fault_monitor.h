#pragma once
// Monitor voltage/current/temperature for faults
// Requires LTC AFE, LTC ADC, BPS heartbeat to be initialized
#include "bps_heartbeat.h"
#include "ltc_afe.h"

typedef struct FaultMonitorSettings {
  // TODO(ELEC-439): add current sense
  LtcAfeStorage *ltc_afe;
  BpsHeartbeatStorage *bps_heartbeat;

  // In 100uV (0.1mV)
  uint16_t overvoltage;
  uint16_t undervoltage;

  // In mC
  uint16_t overtemp_charge;
  uint16_t overtemp_discharge;

  // In mA
  int32_t overcurrent_charge;
  int32_t overcurrent_discharge;
} FaultMonitorSettings;

typedef struct FaultMonitorResult {
  uint16_t cell_voltages[PLUTUS_CFG_AFE_TOTAL_CELLS];
  uint16_t temp_voltages[PLUTUS_CFG_AFE_TOTAL_CELLS];
  int32_t current;
} FaultMonitorResult;

StatusCode fault_monitor_check(const FaultMonitorSettings *settings, FaultMonitorResult *result);