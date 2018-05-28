#include "can.h"
#include "can_transmit.h"
#include "drive_can.h"
#include "event_queue.h"
#include "exported_enums.h"
#include "interrupt.h"
#include "log.h"
#include "ms_test_helpers.h"
#include "test_helpers.h"
#include "unity.h"

#define TEST_DRIVE_CAN_DEVICE_ID 0x1
#define TEST_DRIVE_CAN_NUM_RX_HANDLERS 2

typedef enum {
  TEST_DRIVE_CAN_EVENT_RX = 10,
  TEST_DRIVE_CAN_EVENT_TX,
  TEST_DRIVE_CAN_EVENT_FAULT,
} TestDriveCanEvent;

static CANStorage s_can_storage;
static CANRxHandler s_rx_handlers[TEST_DRIVE_CAN_NUM_RX_HANDLERS];

static int16_t s_throttle;
static EEDriveOutputDirection s_direction;
static int16_t s_speed_cms;

void TEST_MOCK(motor_controller_set_throttle)(MotorControllerStorage *controller, int16_t throttle,
                                              EEDriveOutputDirection direction) {
  LOG_DEBUG("setting throttle: %d (direction %d)\n", throttle, direction);
  s_throttle = throttle;
  s_direction = direction;
  s_speed_cms = 0;
}

void TEST_MOCK(motor_controller_set_cruise)(MotorControllerStorage *controller, int16_t speed_cms) {
  LOG_DEBUG("setting cruise target: %d cm/s\n", speed_cms);
  s_throttle = 0;
  s_direction = 0;
  s_speed_cms = speed_cms;
}

void setup_test(void) {
  event_queue_init();
  interrupt_init();
  soft_timer_init();

  CANSettings can_settings = {
    .device_id = TEST_DRIVE_CAN_DEVICE_ID,
    .bitrate = CAN_HW_BITRATE_125KBPS,
    .rx_event = TEST_DRIVE_CAN_EVENT_RX,
    .tx_event = TEST_DRIVE_CAN_EVENT_TX,
    .fault_event = TEST_DRIVE_CAN_EVENT_FAULT,
    .tx = { GPIO_PORT_A, 12 },
    .rx = { GPIO_PORT_A, 11 },
    .loopback = true,
  };

  StatusCode ret =
      can_init(&can_settings, &s_can_storage, s_rx_handlers, TEST_DRIVE_CAN_NUM_RX_HANDLERS);
  TEST_ASSERT_OK(ret);

  drive_can_init(NULL);

  s_throttle = 0;
  s_direction = EE_DRIVE_OUTPUT_DIRECTION_NEUTRAL;
  s_speed_cms = 0;
}

void teardown_test(void) {}

// TODO: lazy and need to replace this
static const Event s_tx_event = { TEST_DRIVE_CAN_EVENT_TX, 0 };
static const Event s_rx_event = { TEST_DRIVE_CAN_EVENT_RX, 0 };

void test_drive_can_basic(void) {
  // Make sure we prevent throttle/cruise from activating if mechanical brake is active
  CAN_TRANSMIT_MOTOR_CONTROLS(100, EE_DRIVE_OUTPUT_DIRECTION_FORWARD, 0,
                              EE_DRIVE_OUTPUT_MECH_THRESHOLD + 1);
  MS_TEST_HELPER_CAN_TX_RX(s_tx_event, s_rx_event);
  TEST_ASSERT_EQUAL(0, s_throttle);

  CAN_TRANSMIT_MOTOR_CONTROLS(0, EE_DRIVE_OUTPUT_DIRECTION_FORWARD, 45,
                              EE_DRIVE_OUTPUT_MECH_THRESHOLD + 1);
  MS_TEST_HELPER_CAN_TX_RX(s_tx_event, s_rx_event);
  TEST_ASSERT_EQUAL(0, s_throttle);
  TEST_ASSERT_EQUAL(0, s_speed_cms);

  // Allow regen braking to pass through even if mechanical brake is active
  CAN_TRANSMIT_MOTOR_CONTROLS((uint16_t)-300, EE_DRIVE_OUTPUT_DIRECTION_FORWARD, 0,
                              EE_DRIVE_OUTPUT_MECH_THRESHOLD + 1);
  MS_TEST_HELPER_CAN_TX_RX(s_tx_event, s_rx_event);
  TEST_ASSERT_EQUAL(-300, s_throttle);
  TEST_ASSERT_EQUAL(0, s_speed_cms);

  // Cruise should override throttle
  CAN_TRANSMIT_MOTOR_CONTROLS(200, EE_DRIVE_OUTPUT_DIRECTION_FORWARD, 45, 0);
  MS_TEST_HELPER_CAN_TX_RX(s_tx_event, s_rx_event);
  TEST_ASSERT_EQUAL(0, s_throttle);
  TEST_ASSERT_EQUAL(45, s_speed_cms);

  // Throttle
  CAN_TRANSMIT_MOTOR_CONTROLS(300, EE_DRIVE_OUTPUT_DIRECTION_REVERSE, 0, 0);
  MS_TEST_HELPER_CAN_TX_RX(s_tx_event, s_rx_event);
  TEST_ASSERT_EQUAL(300, s_throttle);
}
