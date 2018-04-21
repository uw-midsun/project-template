#include "notify.h"

#include <stddef.h>

#include "can_interval.h"
#include "can_msg_defs.h"
#include "can_pack.h"
#include "can_unpack.h"
#include "charger_events.h"
#include "event_queue.h"
#include "generic_can.h"
#include "generic_can_msg.h"
#include "log.h"
#include "soft_timer.h"
#include "status.h"

static uint32_t s_watchdog_period_s;
static CanInterval *s_interval;
static SoftTimerID s_watchdog = SOFT_TIMER_INVALID_TIMER;

// Watchdog activated when commanded to start. Requires reaffirmation every minute or it will cease
// charging.
static void prv_command_watchdog(SoftTimerID id, void *context) {
  (void)id;
  (void)context;
  event_raise(CHARGER_EVENT_STOP_CHARGING, 0);
}

static void prv_kick_watchdog(void) {
  soft_timer_cancel(s_watchdog);
  soft_timer_start_seconds(s_watchdog_period_s, prv_command_watchdog, NULL, &s_watchdog);
}

// GenericCanRx
static void prv_command_rx(const GenericCanMsg *msg, void *context) {
  (void)context;
  CANMessage can_msg = { 0 };
  generic_can_msg_to_can_message(msg, &can_msg);
  uint8_t allowed = 0;
  CAN_UNPACK_CHARGING_PERMISSION(&can_msg, &allowed);
  if (allowed) {
    event_raise(CHARGER_EVENT_START_CHARGING, 0);
    prv_kick_watchdog();
  } else {
    event_raise(CHARGER_EVENT_STOP_CHARGING, 0);
  }
}

StatusCode notify_init(GenericCan *can, uint32_t send_period_s, uint32_t watchdog_period_s) {
  s_watchdog_period_s = watchdog_period_s;
  soft_timer_cancel(s_watchdog);
  s_watchdog = SOFT_TIMER_INVALID_TIMER;

  CANMessage msg = { 0 };
  CAN_PACK_CHARGING_REQ(&msg);
  GenericCanMsg generic_msg;
  status_ok_or_return(can_message_to_generic_can_message(&msg, &generic_msg));

  status_ok_or_return(
      can_interval_factory(can, &generic_msg, send_period_s * 1000000, &s_interval));
  return generic_can_register_rx(can, prv_command_rx, GENERIC_CAN_EMPTY_MASK,
                                 SYSTEM_CAN_MESSAGE_CHARGING_PERMISSION, false, NULL);
}

void notify_post(void) {
  // Don't start the watchdog until the first message is received.
  can_interval_enable(s_interval);
}

void notify_cease(void) {
  can_interval_disable(s_interval);
  soft_timer_cancel(s_watchdog);
  s_watchdog = SOFT_TIMER_INVALID_TIMER;
}
