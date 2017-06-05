#include "can.h"
#include "event_queue.h"
#include "interrupt.h"
#include "log.h"
#include "unity.h"
#include "test_helpers.h"
#include <inttypes.h>

#define TEST_CAN_DEVICE_ID 0x1

typedef enum {
  TEST_CAN_EVENT_RX = 10,
  TEST_CAN_EVENT_FAULT
} TestCanEvent;

static volatile CANConfig s_can;

static StatusCode prv_rx_callback(const CANMessage *msg, void *context, CANAckStatus *ack_reply) {
  LOG_DEBUG("CAN RX: ID %d from %d\n", msg->msg_id, msg->source_id);
  // printf for STM32 has %lx disabled
  LOG_DEBUG("Data: 0x%x%x (%d bytes)\n", msg->data_u32[1], msg->data_u32[0], msg->dlc);
  CANMessage *rx_msg = context;
  *rx_msg = *msg;

  return STATUS_CODE_OK;
}

static StatusCode prv_ack_callback(CANMessageID msg_id, uint16_t device, CANAckStatus status,
                                   uint16_t num_remaining, void *context) {
  LOG_DEBUG("CAN ACK RX: Status %d for ID %d from %d (%d remaining)\n",
            status, msg_id, device, num_remaining);

  uint16_t *device_acked = context;
  *device_acked = device;

  return STATUS_CODE_OK;
}

void setup_test(void) {
  event_queue_init();
  interrupt_init();
  soft_timer_init();
  can_init(&s_can, TEST_CAN_DEVICE_ID, 125, true, TEST_CAN_EVENT_RX, TEST_CAN_EVENT_FAULT);
}

void teardown_test(void) { }

void test_can_basic(void) {
  volatile CANMessage rx_msg = { 0 };
  can_register_rx_handler(&s_can, 0x6, prv_rx_callback, &rx_msg);
  can_register_rx_handler(&s_can, 0x1, prv_rx_callback, &rx_msg);
  can_register_rx_handler(&s_can, 0x5, prv_rx_callback, &rx_msg);

  CANMessage msg = {
    .msg_id = 0x5,
    .type = CAN_MSG_TYPE_DATA,
    .data = 0x1,
    .dlc = 1
  };

  StatusCode ret = can_transmit(&s_can, &msg, NULL);
  TEST_ASSERT_OK(ret);

  Event e = { 0 };
  while (event_process(&e) != STATUS_CODE_OK) { }
  TEST_ASSERT_EQUAL(TEST_CAN_EVENT_RX, e.id);
  TEST_ASSERT_EQUAL(1, e.data);

  bool processed = fsm_process_event(&s_can.fsm, &e);
  TEST_ASSERT_TRUE(processed);

  TEST_ASSERT_EQUAL(msg.msg_id, rx_msg.msg_id);
  TEST_ASSERT_EQUAL(msg.data, rx_msg.data);
}

void test_can_filter(void) {
  volatile CANMessage rx_msg = { 0 };
  can_add_filter(&s_can, 0x2);

  can_register_rx_handler(&s_can, 0x1, prv_rx_callback, &rx_msg);
  can_register_rx_handler(&s_can, 0x2, prv_rx_callback, &rx_msg);

  CANMessage msg = {
    .msg_id = 0x1,
    .type = CAN_MSG_TYPE_DATA,
    .data = 0x1122334455667788,
    .dlc = 8
  };

  StatusCode ret = can_transmit(&s_can, &msg, NULL);
  TEST_ASSERT_OK(ret);
  msg.msg_id = 0x2;
  ret = can_transmit(&s_can, &msg, NULL);
  TEST_ASSERT_OK(ret);

  Event e = { 0 };
  while (event_process(&e) != STATUS_CODE_OK) { }
  TEST_ASSERT_EQUAL(TEST_CAN_EVENT_RX, e.id);
  TEST_ASSERT_EQUAL(1, e.data);

  bool processed = fsm_process_event(&s_can.fsm, &e);
  TEST_ASSERT_TRUE(processed);

  TEST_ASSERT_EQUAL(msg.msg_id, rx_msg.msg_id);
  TEST_ASSERT_EQUAL(msg.data, rx_msg.data);
}

static void prv_status_callback(const Status *status) {
  LOG_DEBUG("Status: %d (%s) from %s:%s\n", status->code, status->message, status->source, status->caller);
}

void test_can_ack(void) {
  volatile uint16_t device_acked = CAN_MSG_INVALID_DEVICE;
  status_register_callback(prv_status_callback);

  CANMessage msg = {
    .msg_id = 0x1,
    .type = CAN_MSG_TYPE_DATA,
    .data = 0x1122334455667788,
    .dlc = 8
  };

  CANAckRequest ack_req = {
    .callback = prv_ack_callback,
    .context = &device_acked,
    .num_expected = 1
  };

  StatusCode ret = can_transmit(&s_can, &msg, &ack_req);
  TEST_ASSERT_OK(ret);

  while (device_acked != CAN_MSG_INVALID_DEVICE) { }

  TEST_ASSERT_EQUAL(TEST_CAN_DEVICE_ID, device_acked);
}
