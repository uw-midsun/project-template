#include "powertrain_heartbeat.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "can.h"
#include "can_ack.h"
#include "can_hw.h"
#include "can_msg_defs.h"
#include "can_pack.h"
#include "can_transmit.h"
#include "can_unpack.h"
#include "chaos_events.h"
#include "delay.h"
#include "event_queue.h"
#include "fsm.h"
#include "gpio.h"
#include "interrupt.h"
#include "log.h"
#include "misc.h"
#include "ms_test_helpers.h"
#include "soft_timer.h"
#include "status.h"
#include "test_helpers.h"
#include "unity.h"

static CanStorage s_storage;

void setup_test(void) {
  interrupt_init();
  event_queue_init();
  gpio_init();
  soft_timer_init();

  CanSettings settings = {
    .device_id = SYSTEM_CAN_DEVICE_CHAOS,
    .bitrate = CAN_HW_BITRATE_125KBPS,
    .rx_event = CHAOS_EVENT_CAN_RX,
    .tx_event = CHAOS_EVENT_CAN_TX,
    .fault_event = CHAOS_EVENT_CAN_FAULT,
    .tx = { GPIO_PORT_A, 12 },
    .rx = { GPIO_PORT_A, 11 },
    .loopback = true,
  };

  can_init(&s_storage, &settings);
  TEST_ASSERT_OK(powertrain_heartbeat_init());
}

void teardown_test(void) {}

void test_powertrain_heartbeat_watchdog(void) {
  Event e = { 0 };
  volatile StatusCode status = NUM_STATUS_CODES;
  e.id = CHAOS_EVENT_SEQUENCE_DRIVE_DONE;
  TEST_ASSERT_TRUE(powertrain_heartbeat_process_event(&e));

  // Send 5 times (all will have ack failures).
  MS_TEST_HELPER_CAN_TX_RX(CHAOS_EVENT_CAN_TX, CHAOS_EVENT_CAN_RX);
  MS_TEST_HELPER_CAN_TX_RX(CHAOS_EVENT_CAN_TX, CHAOS_EVENT_CAN_RX);
  MS_TEST_HELPER_CAN_TX_RX(CHAOS_EVENT_CAN_TX, CHAOS_EVENT_CAN_RX);
  MS_TEST_HELPER_CAN_TX_RX(CHAOS_EVENT_CAN_TX, CHAOS_EVENT_CAN_RX);
  MS_TEST_HELPER_CAN_TX_RX(CHAOS_EVENT_CAN_TX, CHAOS_EVENT_CAN_RX);

  // Watchdog should activate.
  do {
    status = event_process(&e);
  } while (status == STATUS_CODE_EMPTY);
  TEST_ASSERT_EQUAL(CHAOS_EVENT_SEQUENCE_EMERGENCY, e.id);

  // No more should activate.
  delay_ms(POWERTRAIN_HEARTBEAT_WATCHDOG_MS);
  TEST_ASSERT_EQUAL(STATUS_CODE_EMPTY, event_process(&e));
}

void test_powertrain_heartbeat_stop_heartbeat(void) {
  Event e = { 0 };
  StatusCode status = NUM_STATUS_CODES;
  e.id = CHAOS_EVENT_SEQUENCE_DRIVE_DONE;
  TEST_ASSERT_TRUE(powertrain_heartbeat_process_event(&e));

  // Immediately send heartbeat update
  MS_TEST_HELPER_CAN_TX_RX_WITH_ACK(CHAOS_EVENT_CAN_TX, CHAOS_EVENT_CAN_RX);

  e.id = CHAOS_EVENT_SEQUENCE_EMERGENCY;
  TEST_ASSERT_TRUE(powertrain_heartbeat_process_event(&e));

  delay_ms(POWERTRAIN_HEARTBEAT_WATCHDOG_MS + 100);
  // No activation should occur.
  TEST_ASSERT_EQUAL(STATUS_CODE_EMPTY, event_process(&e));
}

void test_powertrain_heartbeat_kick_watchdog(void) {
  CanMessage msg = {
    .type = CAN_MSG_TYPE_ACK,
    .msg_id = SYSTEM_CAN_MESSAGE_POWERTRAIN_HEARTBEAT,
  };

  Event e = { 0 };
  volatile StatusCode status = NUM_STATUS_CODES;
  e.id = CHAOS_EVENT_SEQUENCE_DRIVE_DONE;
  TEST_ASSERT_TRUE(powertrain_heartbeat_process_event(&e));

  // Send heartbeat - watchdog should be kicked (manually ACK message)
  msg.source_id = SYSTEM_CAN_DEVICE_PLUTUS;
  TEST_ASSERT_OK(can_ack_handle_msg(&s_storage.ack_requests, &msg));
  msg.source_id = SYSTEM_CAN_DEVICE_DRIVER_CONTROLS_PEDAL;
  TEST_ASSERT_OK(can_ack_handle_msg(&s_storage.ack_requests, &msg));
  msg.source_id = SYSTEM_CAN_DEVICE_MOTOR_CONTROLLER;
  TEST_ASSERT_OK(can_ack_handle_msg(&s_storage.ack_requests, &msg));

  MS_TEST_HELPER_CAN_TX_RX(CHAOS_EVENT_CAN_TX, CHAOS_EVENT_CAN_RX);

  // We will TX 4 more times before the watchdog times out
  MS_TEST_HELPER_CAN_TX_RX(CHAOS_EVENT_CAN_TX, CHAOS_EVENT_CAN_RX);
  MS_TEST_HELPER_CAN_TX_RX(CHAOS_EVENT_CAN_TX, CHAOS_EVENT_CAN_RX);
  MS_TEST_HELPER_CAN_TX_RX(CHAOS_EVENT_CAN_TX, CHAOS_EVENT_CAN_RX);
  MS_TEST_HELPER_CAN_TX_RX(CHAOS_EVENT_CAN_TX, CHAOS_EVENT_CAN_RX);

  // Watchdog should trigger.
  do {
    status = event_process(&e);
  } while (status == STATUS_CODE_EMPTY);
  TEST_ASSERT_EQUAL(CHAOS_EVENT_SEQUENCE_EMERGENCY, e.id);
}
