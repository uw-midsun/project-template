#include "gpio_expander.h"

#include <stdbool.h>

#include "delay.h"
#include "driver_io.h"
#include "gpio_it.h"
#include "i2c.h"
#include "mcp23008.h"
#include "soft_timer.h"

// Bouncing from the inputs can be mistaken as interrupts by the device. After
// each interrupt,
// the GPIO expander will be delayed for a brief period to prevent this
#define GPIO_EXPANDER_DELAY_MS 50

typedef struct GPIOExpanderInterrupt {
  GPIOExpanderCallback callback;
  void *context;
  GPIOState state;
  GPIOExpanderPin pin;
  SoftTimerID timer;
  uint8_t pin_mask;
} GPIOExpanderInterrupt;

static GPIOAddress s_address;
static I2CPort s_i2c_port;

static GPIOExpanderInterrupt s_interrupts[NUM_GPIO_EXPANDER_PINS];

// Check to see if the pin passed in has a correct value
static StatusCode prv_pin_is_valid(GPIOExpanderPin pin) {
  if (pin >= NUM_GPIO_EXPANDER_PINS) {
    return status_code(STATUS_CODE_INVALID_ARGS);
  }
  return STATUS_CODE_OK;
}

// Set a specific bit in a given register
static void prv_set_bit(uint8_t reg, GPIOExpanderPin pin, bool bit) {
  uint8_t data;
  i2c_read_reg(s_i2c_port, MCP23008_ADDRESS, reg, &data, 1);
  if (bit) {
    data |= (1 << pin);
  } else {
    data &= ~(1 << pin);
  }
  i2c_write_reg(s_i2c_port, MCP23008_ADDRESS, reg, &data, 1);
}

static void prv_debounce_delay(SoftTimerID timer_id, void *context) {
  GPIOExpanderInterrupt *interrupt = (GPIOExpanderInterrupt *)context;
  GPIOState state = GPIO_STATE_LOW;

  gpio_expander_get_state(interrupt->pin, &state);

  // If the value after delaying is not equal to the original value, then
  // disregard the input
  if (state != interrupt->state) {
    prv_set_bit(MCP23008_GPINTEN, interrupt->pin, 1);
    return;
  }

  // Execute the pin's callback
  if (interrupt->callback != NULL) {
    interrupt->callback(interrupt->pin, interrupt->state, interrupt->context);
  }

  // Re-enable interrupts for the pin
  prv_set_bit(MCP23008_GPINTEN, interrupt->pin, 1);
  return;
}

static void prv_interrupt_handler(const GPIOAddress *address, void *context) {
  // Disable interrupts until ISR has completed
  uint8_t intf = 0;
  uint8_t intcap = 0;
  uint8_t gpinten = 0;

  // Read the interrupt flag register to determine the pins with a pending
  // interrupt
  i2c_read_reg(s_i2c_port, MCP23008_ADDRESS, MCP23008_INTF, &intf, 1);

  // Obtain the port values captured at the time of the interrupts
  i2c_read_reg(s_i2c_port, MCP23008_ADDRESS, MCP23008_INTCAP, &intcap, 1);

  // Temporarily disable interrupts on the expander
  i2c_read_reg(s_i2c_port, MCP23008_ADDRESS, MCP23008_GPINTEN, &gpinten, 1);
  gpinten &= ~(intf);
  i2c_write_reg(s_i2c_port, MCP23008_ADDRESS, MCP23008_GPINTEN, &gpinten, 1);

  // Mask the interrupts of all offending pins. Then start a delay to check the
  // value it holds
  GPIOExpanderPin current_pin;
  while (intf != 0) {
    current_pin = __builtin_ffs(intf) - 1;

    s_interrupts[current_pin].state = (intcap >> current_pin) & 1;
    soft_timer_start_millis(GPIO_EXPANDER_DELAY_MS, prv_debounce_delay, &s_interrupts[current_pin],
                            &s_interrupts[current_pin].timer);

    intf &= ~(1 << current_pin);
  }
}

StatusCode gpio_expander_init(GPIOAddress address, I2CPort i2c_port) {
  // Set the static variables
  s_address = address;
  s_i2c_port = i2c_port;

  // Configure the pin to receive interrupts from the MCP23008
  GPIOSettings gpio_settings = { .direction = GPIO_DIR_IN, .alt_function = GPIO_ALTFN_NONE };
  InterruptSettings it_settings = { INTERRUPT_TYPE_INTERRUPT, INTERRUPT_PRIORITY_NORMAL };

  gpio_init_pin(&address, &gpio_settings);
  gpio_it_register_interrupt(&address, &it_settings, INTERRUPT_EDGE_FALLING, prv_interrupt_handler,
                             NULL);

  // Initialize the interrupt callbacks to NULL
  for (uint8_t i = 0; i < NUM_GPIO_EXPANDER_PINS; i++) {
    s_interrupts[i].callback = NULL;
    s_interrupts[i].context = NULL;
    s_interrupts[i].state = GPIO_STATE_LOW;
    s_interrupts[i].pin = i;
    s_interrupts[i].timer = i;

    // Use the current index to indicate the bit position of the pin
    s_interrupts[i].pin_mask = (1 << i);
  }

  return STATUS_CODE_OK;
}

StatusCode gpio_expander_init_pin(GPIOExpanderPin pin, GPIOSettings *settings) {
  StatusCode valid = prv_pin_is_valid(pin);
  status_ok_or_return(valid);

  // Set the direction of the data I/O
  prv_set_bit(MCP23008_IODIR, pin, (settings->direction == GPIO_DIR_IN));

  if (settings->direction == GPIO_DIR_OUT) {
    // Set the output state if the pin is an output
    prv_set_bit(MCP23008_GPIO, pin, (settings->state == GPIO_STATE_HIGH));
  } else {
    // Enable interrupts if the pin is an input
    prv_set_bit(MCP23008_GPINTEN, pin, true);

    // Configure so that any change in value triggers an interrupt
    prv_set_bit(MCP23008_INTCON, pin, false);
  }

  return STATUS_CODE_OK;
}

StatusCode gpio_expander_get_state(GPIOExpanderPin pin, GPIOState *state) {
  StatusCode valid = prv_pin_is_valid(pin);
  status_ok_or_return(valid);

  // Update our GPIO register value and read the pin state
  uint8_t data;
  status_ok_or_return(i2c_read_reg(s_i2c_port, MCP23008_ADDRESS, MCP23008_GPIO, &data, 1));

  *state = (data >> pin) & 1;

  return STATUS_CODE_OK;
}

StatusCode gpio_expander_set_state(GPIOExpanderPin pin, GPIOState new_state) {
  StatusCode valid = prv_pin_is_valid(pin);
  status_ok_or_return(valid);

  // Return if the pin is configured as an input
  uint8_t io_dir;
  status_ok_or_return(i2c_read_reg(s_i2c_port, MCP23008_ADDRESS, MCP23008_IODIR, &io_dir, 1));

  if ((io_dir >> pin) & 1) {
    return status_code(STATUS_CODE_INVALID_ARGS);
  }

  // Set the output latch to the new logical state
  prv_set_bit(MCP23008_OLAT, pin, new_state);

  return STATUS_CODE_OK;
}

StatusCode gpio_expander_register_callback(GPIOExpanderPin pin, GPIOExpanderCallback callback,
                                           void *context) {
  StatusCode valid = prv_pin_is_valid(pin);
  status_ok_or_return(valid);

  s_interrupts[pin].callback = callback;
  s_interrupts[pin].context = context;

  return STATUS_CODE_OK;
}
