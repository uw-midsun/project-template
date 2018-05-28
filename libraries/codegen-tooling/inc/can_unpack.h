#pragma once

#include "can_msg_defs.h"
#include "can_unpack_impl.h"

#define CAN_UNPACK_BPS_HEARTBEAT(msg_ptr, status_u8_ptr)                                          \
  can_unpack_impl_u8((msg_ptr), 1, (status_u8_ptr), CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, \
                     CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY,         \
                     CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY)

#define CAN_UNPACK_POWER_DISTRIBUTION_FAULT(msg_ptr) can_unpack_impl_empty((msg_ptr), 0)

#define CAN_UNPACK_BATTERY_RELAY_MAIN(msg_ptr, relay_state_u8_ptr)                        \
  can_unpack_impl_u8((msg_ptr), 1, (relay_state_u8_ptr), CAN_UNPACK_IMPL_EMPTY,           \
                     CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, \
                     CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY)

#define CAN_UNPACK_BATTERY_RELAY_SLAVE(msg_ptr, relay_state_u8_ptr)                       \
  can_unpack_impl_u8((msg_ptr), 1, (relay_state_u8_ptr), CAN_UNPACK_IMPL_EMPTY,           \
                     CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, \
                     CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY)

#define CAN_UNPACK_MOTOR_RELAY(msg_ptr, relay_state_u8_ptr)                               \
  can_unpack_impl_u8((msg_ptr), 1, (relay_state_u8_ptr), CAN_UNPACK_IMPL_EMPTY,           \
                     CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, \
                     CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY)

#define CAN_UNPACK_SOLAR_RELAY_REAR(msg_ptr, relay_state_u8_ptr)                          \
  can_unpack_impl_u8((msg_ptr), 1, (relay_state_u8_ptr), CAN_UNPACK_IMPL_EMPTY,           \
                     CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, \
                     CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY)

#define CAN_UNPACK_SOLAR_RELAY_FRONT(msg_ptr, relay_state_u8_ptr)                         \
  can_unpack_impl_u8((msg_ptr), 1, (relay_state_u8_ptr), CAN_UNPACK_IMPL_EMPTY,           \
                     CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, \
                     CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY)

#define CAN_UNPACK_POWER_STATE(msg_ptr, power_state_u8_ptr)                               \
  can_unpack_impl_u8((msg_ptr), 1, (power_state_u8_ptr), CAN_UNPACK_IMPL_EMPTY,           \
                     CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, \
                     CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY)

#define CAN_UNPACK_POWERTRAIN_HEARTBEAT(msg_ptr) can_unpack_impl_empty((msg_ptr), 0)

#define CAN_UNPACK_OVUV_DCDC_AUX(msg_ptr, dcdc_ov_flag_u8_ptr, dcdc_uv_flag_u8_ptr,             \
                                 aux_bat_ov_flag_u8_ptr, aux_bat_uv_flag_u8_ptr)                \
  can_unpack_impl_u8((msg_ptr), 4, (dcdc_ov_flag_u8_ptr), (dcdc_uv_flag_u8_ptr),                \
                     (aux_bat_ov_flag_u8_ptr), (aux_bat_uv_flag_u8_ptr), CAN_UNPACK_IMPL_EMPTY, \
                     CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY)

#define CAN_UNPACK_MC_ERROR_LIMITS(msg_ptr, error_id_u16_ptr, limits_u16_ptr)                    \
  can_unpack_impl_u16((msg_ptr), 4, (error_id_u16_ptr), (limits_u16_ptr), CAN_UNPACK_IMPL_EMPTY, \
                      CAN_UNPACK_IMPL_EMPTY)

#define CAN_UNPACK_MOTOR_CONTROLS(msg_ptr, throttle_u16_ptr, direction_u16_ptr,           \
                                  cruise_control_u16_ptr, mechanical_brake_state_u16_ptr) \
  can_unpack_impl_u16((msg_ptr), 8, (throttle_u16_ptr), (direction_u16_ptr),              \
                      (cruise_control_u16_ptr), (mechanical_brake_state_u16_ptr))

#define CAN_UNPACK_LIGHT_STATE(msg_ptr, light_id_u8_ptr, light_state_u8_ptr)                       \
  can_unpack_impl_u8((msg_ptr), 2, (light_id_u8_ptr), (light_state_u8_ptr), CAN_UNPACK_IMPL_EMPTY, \
                     CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY,          \
                     CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY)

#define CAN_UNPACK_HORN(msg_ptr, state_u8_ptr)                                                   \
  can_unpack_impl_u8((msg_ptr), 1, (state_u8_ptr), CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, \
                     CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY,        \
                     CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY)

#define CAN_UNPACK_CHARGER_CONN_STATE(msg_ptr, is_connected_u8_ptr)                       \
  can_unpack_impl_u8((msg_ptr), 1, (is_connected_u8_ptr), CAN_UNPACK_IMPL_EMPTY,          \
                     CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, \
                     CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY)

#define CAN_UNPACK_CHARGER_SET_RELAY_STATE(msg_ptr, state_u8_ptr)                                \
  can_unpack_impl_u8((msg_ptr), 1, (state_u8_ptr), CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, \
                     CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY,        \
                     CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY)

#define CAN_UNPACK_STEERING_ANGLE(msg_ptr, steering_angle_u16_ptr)                   \
  can_unpack_impl_u16((msg_ptr), 2, (steering_angle_u16_ptr), CAN_UNPACK_IMPL_EMPTY, \
                      CAN_UNPACK_IMPL_EMPTY, CAN_UNPACK_IMPL_EMPTY)

#define CAN_UNPACK_BATTERY_SOC(msg_ptr) can_unpack_impl_empty((msg_ptr), 0)

#define CAN_UNPACK_BATTERY_VCT(msg_ptr, module_id_u16_ptr, voltage_u16_ptr, current_u16_ptr,   \
                               temperature_u16_ptr)                                            \
  can_unpack_impl_u16((msg_ptr), 8, (module_id_u16_ptr), (voltage_u16_ptr), (current_u16_ptr), \
                      (temperature_u16_ptr))

#define CAN_UNPACK_MOTOR_CONTROLLER_VC(msg_ptr, mc_voltage_1_u16_ptr, mc_current_1_u16_ptr, \
                                       mc_voltage_2_u16_ptr, mc_current_2_u16_ptr)          \
  can_unpack_impl_u16((msg_ptr), 8, (mc_voltage_1_u16_ptr), (mc_current_1_u16_ptr),         \
                      (mc_voltage_2_u16_ptr), (mc_current_2_u16_ptr))

#define CAN_UNPACK_MOTOR_VELOCITY(msg_ptr, vehicle_velocity_left_u32_ptr, \
                                  vehicle_velocity_right_u32_ptr)         \
  can_unpack_impl_u32((msg_ptr), 8, (vehicle_velocity_left_u32_ptr),      \
                      (vehicle_velocity_right_u32_ptr))

#define CAN_UNPACK_MOTOR_ANGULAR_FREQUENCY(msg_ptr, angular_freq_left_u32_ptr, \
                                           angular_freq_right_u32_ptr)         \
  can_unpack_impl_u32((msg_ptr), 8, (angular_freq_left_u32_ptr), (angular_freq_right_u32_ptr))

#define CAN_UNPACK_MOTOR_TEMPS(msg_ptr, motor_temp_l_u32_ptr, motor_temp_r_u32_ptr) \
  can_unpack_impl_u32((msg_ptr), 8, (motor_temp_l_u32_ptr), (motor_temp_r_u32_ptr))

#define CAN_UNPACK_MOTOR_AMP_HR(msg_ptr, motor_amp_hr_l_u32_ptr, motor_amp_hr_r_u32_ptr) \
  can_unpack_impl_u32((msg_ptr), 8, (motor_amp_hr_l_u32_ptr), (motor_amp_hr_r_u32_ptr))

#define CAN_UNPACK_ODOMETER(msg_ptr, odometer_val_u32_ptr) \
  can_unpack_impl_u32((msg_ptr), 4, (odometer_val_u32_ptr), CAN_UNPACK_IMPL_EMPTY)

#define CAN_UNPACK_AUX_DCDC_VC(msg_ptr, aux_voltage_u16_ptr, aux_current_u16_ptr, \
                               dcdc_voltage_u16_ptr, dcdc_current_u16_ptr)        \
  can_unpack_impl_u16((msg_ptr), 8, (aux_voltage_u16_ptr), (aux_current_u16_ptr), \
                      (dcdc_voltage_u16_ptr), (dcdc_current_u16_ptr))

#define CAN_UNPACK_DCDC_TEMPS(msg_ptr, temp_1_u16_ptr, temp_2_u16_ptr)                         \
  can_unpack_impl_u16((msg_ptr), 4, (temp_1_u16_ptr), (temp_2_u16_ptr), CAN_UNPACK_IMPL_EMPTY, \
                      CAN_UNPACK_IMPL_EMPTY)

#define CAN_UNPACK_SOLAR_DATA_FRONT(msg_ptr, module_id_u16_ptr, voltage_u16_ptr, current_u16_ptr, \
                                    temperature_u16_ptr)                                          \
  can_unpack_impl_u16((msg_ptr), 8, (module_id_u16_ptr), (voltage_u16_ptr), (current_u16_ptr),    \
                      (temperature_u16_ptr))

#define CAN_UNPACK_SOLAR_DATA_REAR(msg_ptr, module_id_u16_ptr, voltage_u16_ptr, current_u16_ptr, \
                                   temperature_u16_ptr)                                          \
  can_unpack_impl_u16((msg_ptr), 8, (module_id_u16_ptr), (voltage_u16_ptr), (current_u16_ptr),   \
                      (temperature_u16_ptr))

#define CAN_UNPACK_LINEAR_ACCELERATION(msg_ptr) can_unpack_impl_empty((msg_ptr), 0)

#define CAN_UNPACK_ANGULAR_ROTATION(msg_ptr) can_unpack_impl_empty((msg_ptr), 0)
