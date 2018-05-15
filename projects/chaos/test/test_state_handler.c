#include "state_handler.h"

#include "can.h"
#include "can_ack.h"
#include "can_msg_defs.h"
#include "can_transmit.h"
#include "chaos_events.h"
#include "exported_enums.h"
#include "gpio.h"
#include "interrupt.h"
#include "ms_test_helpers.h"
#include "soft_timer.h"
#include "test_helpers.h"
#include "unity.h"

#define NUM_TEST_STATE_HANDLER_CAN_HANDLERS 2

static const Event s_tx_event = { CHAOS_EVENT_CAN_TX, 0 };
static const Event s_rx_event = { CHAOS_EVENT_CAN_RX, 0 };
static CANStorage s_can_storage;
static CANAckRequests s_can_ack_requests;
static CANRxHandler s_rx_handlers[NUM_TEST_STATE_HANDLER_CAN_HANDLERS];

// CANAckRequestCb
static StatusCode prv_ack_callback(CANMessageID msg_id, uint16_t device, CANAckStatus status,
                                   uint16_t num_remaining, void *context) {
  (void)num_remaining;
  CANAckStatus *expected_status = context;
  TEST_ASSERT_EQUAL(SYSTEM_CAN_MESSAGE_POWER_STATE, msg_id);
  TEST_ASSERT_EQUAL(SYSTEM_CAN_DEVICE_CHAOS, device);
  TEST_ASSERT_EQUAL(*expected_status, status);
  return STATUS_CODE_OK;
}

void setup_test(void) {
  event_queue_init();
  interrupt_init();
  soft_timer_init();

  CANSettings can_settings = {
    .device_id = SYSTEM_CAN_DEVICE_CHAOS,
    .bitrate = CAN_HW_BITRATE_125KBPS,
    .rx_event = CHAOS_EVENT_CAN_RX,
    .tx_event = CHAOS_EVENT_CAN_TX,
    .fault_event = CHAOS_EVENT_CAN_FAULT,
    .tx = { GPIO_PORT_A, 12 },
    .rx = { GPIO_PORT_A, 11 },
    .loopback = true,
  };
  TEST_ASSERT_OK(
      can_init(&can_settings, &s_can_storage, s_rx_handlers, NUM_TEST_STATE_HANDLER_CAN_HANDLERS));
  can_ack_init(&s_can_ack_requests);
}

void teardown_test(void) {}

void test_state_handler(void) {
  TEST_ASSERT_OK(state_handler_init());
  CANAckStatus expected_status = CAN_ACK_STATUS_OK;
  CANAckRequest req = {
    .callback = prv_ack_callback,
    .context = &expected_status,
    .expected_bitset = CAN_ACK_EXPECTED_DEVICES(SYSTEM_CAN_DEVICE_CHAOS),
  };

  Event e = { 0, 0 };
  // IDLE
  CAN_TRANSMIT_POWER_STATE(&req, EE_POWER_STATE_IDLE);
  MS_TEST_HELPER_CAN_TX_RX_WITH_ACK(s_tx_event, s_rx_event);
  MS_TEST_HELPER_AWAIT_EVENT(e);
  TEST_ASSERT_EQUAL(CHAOS_EVENT_SEQUENCE_IDLE, e.id);

  // CHARGE
  CAN_TRANSMIT_POWER_STATE(&req, EE_POWER_STATE_CHARGE);
  MS_TEST_HELPER_CAN_TX_RX_WITH_ACK(s_tx_event, s_rx_event);
  MS_TEST_HELPER_AWAIT_EVENT(e);
  TEST_ASSERT_EQUAL(CHAOS_EVENT_SEQUENCE_CHARGE, e.id);

  // DRIVE
  CAN_TRANSMIT_POWER_STATE(&req, EE_POWER_STATE_DRIVE);
  MS_TEST_HELPER_CAN_TX_RX_WITH_ACK(s_tx_event, s_rx_event);
  MS_TEST_HELPER_AWAIT_EVENT(e);
  TEST_ASSERT_EQUAL(CHAOS_EVENT_SEQUENCE_DRIVE, e.id);

  // Invalid
  expected_status = CAN_ACK_STATUS_INVALID;
  CAN_TRANSMIT_POWER_STATE(&req, NUM_EE_POWER_STATES);
  MS_TEST_HELPER_CAN_TX_RX_WITH_ACK(s_tx_event, s_rx_event);
  TEST_ASSERT_EQUAL(STATUS_CODE_EMPTY, event_process(&e));
}
