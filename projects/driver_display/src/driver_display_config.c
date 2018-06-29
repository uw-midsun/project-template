#include "driver_display_config.h"

static const UARTSettings uart_settings = {
  .baudrate = CAN_HW_BITRATE_500KBPS,        //
  .tx = { .port = GPIO_PORT_B, .pin = 10 },  //
  .rx = { .port = GPIO_PORT_B, .pin = 11 },  //
  .alt_fn = GPIO_ALTFN_4                     //
};

static const CANHwSettings can_hw_settings = {
  .tx = { .port = GPIO_PORT_A, .pin = 12 },  //
  .rx = { .port = GPIO_PORT_A, .pin = 11 },  //
  .bitrate = CAN_HW_BITRATE_500KBPS,         //
  .loopback = false,
};

static const CanUart can_uart = { .uart = UART_PORT_3,
                                  .rx_cb = NULL,  // Ignore RX'd messages from the master
                                  .context = NULL };

UARTSettings *driver_display_config_load_uart(void) {
  return &uart_settings;
}

CANHwSettings *driver_display_config_load_can(void) {
  return &can_hw_settings;
}

CanUart *driver_display_config_load_can_uart(void) {
  return &can_uart;
}