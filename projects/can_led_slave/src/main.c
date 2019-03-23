// Example program for STM32F072 Controller board or Discovery Board.
// Blinks the LEDs sequentially.
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "delay.h"       // For real-time delays
#include "gpio.h"        // General Purpose I/O control.
#include "gpio_it.h"
#include "interrupt.h"   // For enabling interrupts.
#include "misc.h"        // Various helper functions/macros.
#include "soft_timer.h"  // Software timers for scheduling future events.


#include "can.h"
#include "can_interval.h"
#include "can_msg_defs.h"
#include "can_transmit.h"
#include "can_unpack.h"
#include "event_queue.h"

#include "log.h" 

typedef enum {
  TEST_EVENT_CAN_RX = 1, 
  TEST_EVENT_CAN_TX = 2,
  TEST_EVENT_CAN_FAULT = 3, 
} TestEvent;

struct debug_leds {
    GpioAddress debug_led[4]; 
  }; 

static CanStorage s_can_storage = { 0 };

uint16_t LED_GREEN = 0x1; 

StatusCode led_cb(const CanMessage *msg, void *context, CanAckStatus *ack_reply) {
  struct debug_leds *ptr = (struct debug_leds *) context; 
  uint16_t led_state = 0;
  CAN_UNPACK_LED(msg, &led_state);

  if (led_state == 1) {
    gpio_set_state(&(ptr->debug_led[0]), GPIO_STATE_LOW); 
  }

  else {
    gpio_set_state(&(ptr->debug_led[0]), GPIO_STATE_HIGH); 
  }

  return STATUS_CODE_OK; 
  }


int main(void) {
  gpio_init();
  // Enable various peripherals
  interrupt_init();
  gpio_init();
  gpio_it_init();
  soft_timer_init();
  event_queue_init();

  static const GpioAddress led_green = {
    .port = GPIO_PORT_B, 
    .pin = 5,
  }; 

  GpioSettings led_settings = {
    .direction = GPIO_DIR_OUT,        // The pin needs to output.
    .state = GPIO_STATE_HIGH,         // Start in the "on" state.
    .alt_function = GPIO_ALTFN_NONE,  // No connections to peripherals.
    .resistor = GPIO_RES_NONE,        // No need of a resistor to modify floating logic levels.
  };

  struct debug_leds led = {
    .debug_led = {
      { .port = GPIO_PORT_B, .pin = 5 },   //
      { .port = GPIO_PORT_B, .pin = 4 },   //
      { .port = GPIO_PORT_B, .pin = 3 },   //
      { .port = GPIO_PORT_A, .pin = 15 },  //
    }
  }; 

  GpioAddress *green_led_addr = &led.debug_led[0]; 

  gpio_init_pin(green_led_addr, &led_settings);

  struct debug_leds *led_addr = &led; 

  Event e = { 0 };

 const CanSettings can_settings = {
    .device_id = SYSTEM_CAN_DEVICE_TEST_SLAVE,
    .bitrate = CAN_HW_BITRATE_500KBPS,
    .rx_event = TEST_EVENT_CAN_RX,
    .tx_event = TEST_EVENT_CAN_TX,
    .fault_event = TEST_EVENT_CAN_FAULT,
    .tx = { GPIO_PORT_A, 12 },
    .rx = { GPIO_PORT_A, 11 },
    .loopback = false,
  };

  StatusCode status = can_init(&s_can_storage, &can_settings);

  can_register_rx_handler(SYSTEM_CAN_MESSAGE_LED, led_cb, (void*)led_addr);

  while (true) {
  
    while (status_ok(event_process(&e))) {
      can_process_event(&e);
    }

  }

  return 0;
}
