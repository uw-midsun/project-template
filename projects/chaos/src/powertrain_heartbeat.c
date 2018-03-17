#include "powertrain_heartbeat.h"

#include <stdbool.h>
#include <stddef.h>

#include "can.h"
#include "can_ack.h"
#include "can_msg_defs.h"
#include "can_transmit.h"
#include "chaos_events.h"
#include "event_queue.h"
#include "soft_timer.h"
#include "status.h"

#define POWERTRAIN_WD_MS 1000
#define POWERTRAIN_HB_MS 500

static SoftTimerID s_interval_id = SOFT_TIMER_INVALID_TIMER;
static SoftTimerID s_watchdog_id = SOFT_TIMER_INVALID_TIMER;
static CANAckRequest s_ack_request;

// SoftTimerCallback
static void prv_hb_watchdog(SoftTimerID timer_id, void *context) {
  (void)timer_id;
  (void)context;
  // TODO(ELEC-105): Raise emergency event.
}

static void prv_kick_watchdog(void) {
  if (s_watchdog_id != SOFT_TIMER_INVALID_TIMER) {
    soft_timer_cancel(s_watchdog_id);
  }
  soft_timer_start_millis(POWERTRAIN_WD_MS, prv_hb_watchdog, NULL, &s_watchdog_id);
}

// CANAckRequestCb
static StatusCode prv_ack_cb(CANMessageID id, uint16_t device, CANAckStatus status,
                             uint16_t num_remaining, void *context) {
  (void)id;
  (void)device;
  (void)context;
  if (status != CAN_ACK_STATUS_OK) {
    return status_code(STATUS_CODE_INTERNAL_ERROR);
  }

  if (!num_remaining) {
    prv_kick_watchdog();
  }
  return STATUS_CODE_OK;
}

// SoftTimerCallback
static void prv_send_hb_request(SoftTimerID timer_id, void *context) {
  (void)timer_id;
  (void)context;
  CAN_TRANSMIT_POWERTRAIN_HEARTBEAT(&s_ack_request);
  soft_timer_start_millis(POWERTRAIN_HB_MS, prv_send_hb_request, NULL, &s_interval_id);
}

StatusCode powertrain_heartbeat_init(void) {
  if (s_watchdog_id != SOFT_TIMER_INVALID_TIMER) {
    soft_timer_cancel(s_watchdog_id);
    s_watchdog_id = SOFT_TIMER_INVALID_TIMER;
  }
  if (s_interval_id != SOFT_TIMER_INVALID_TIMER) {
    soft_timer_cancel(s_interval_id);
    s_interval_id = SOFT_TIMER_INVALID_TIMER;
  }

  s_ack_request.callback = prv_ack_cb;
  s_ack_request.expected_bitset =
      CAN_ACK_EXPECTED_DEVICES(SYSTEM_CAN_DEVICE_PLUTUS, SYSTEM_CAN_DEVICE_DRIVER_CONTROLS,
                               SYSTEM_CAN_DEVICE_MOTOR_CONTROLLER);
}

bool powertrain_heartbeat_process_event(const Event *e) {
  if (e->id == CHAOS_EVENT_SEQUENCE_DRIVE_DONE) {
    if (s_interval_id != SOFT_TIMER_INVALID_TIMER) {
      return false;
    }
    prv_kick_watchdog();
    prv_send_hb_request(SOFT_TIMER_INVALID_TIMER, NULL);
    soft_timer_start_millis(POWERTRAIN_HB_MS, prv_send_hb_request, NULL, &s_interval_id);
  } else if (e->id > NUM_CHAOS_EVENTS_FSM && e->id < NUM_CHAOS_EVENT_SEQUENCES) {
    soft_timer_cancel(s_interval_id);
    soft_timer_cancel(s_watchdog_id);
    s_interval_id = SOFT_TIMER_INVALID_TIMER;
    s_watchdog_id = SOFT_TIMER_INVALID_TIMER;
  }
}
