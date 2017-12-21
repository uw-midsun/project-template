#include "gps.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "gpio.h"
#include "interrupt.h"  // For enabling interrupts.
#include "misc.h"
#include "nmea.h"
#include "soft_timer.h"  // Software timers for scheduling future events.
#include "status.h"
#include "uart_mcu.h"

// How many handlers can we possibly need?
#define GPS_HANDLER_ARRAY_LENGTH 5

static GPSHandler gps_handler[GPS_HANDLER_ARRAY_LENGTH] = { 0 };
static GGAHandler gga_handler[GPS_HANDLER_ARRAY_LENGTH] = { 0 };
static UARTStorage s_storage = { 0 };

static void s_nmea_read(const uint8_t *rx_arr, size_t len, void *context) {
  // Check that the context is correct
  NMEAResult r = parse_nmea_sentence(rx_arr, len);

  for (uint32_t i = 0; i < GPS_HANDLER_ARRAY_LENGTH; i++) {
    if (gps_handler[i] != NULL) {
      (*gps_handler[i])(r);
    }
    if (gga_handler[i] != NULL && r.gga.message_id == GGA) {
      (*gga_handler[i])(r.gga);
    }
  }
}

StatusCode add_gps_handler(GPSHandler handler, size_t *index) {
  for (uint16_t i = 0; i < GPS_HANDLER_ARRAY_LENGTH; i++) {
    if (gps_handler[i] == NULL) {
      gps_handler[i] = handler;
      // This cast won't be an issue, there's no way "i" will be big enought to
      // cause an overflow
      if (index) {
        *index = i;
      }
      return STATUS_CODE_OK;
    }
  }
  return STATUS_CODE_UNKNOWN;
}

StatusCode add_gga_handler(GGAHandler handler, size_t *index) {
  for (uint16_t i = 0; i < GPS_HANDLER_ARRAY_LENGTH; i++) {
    if (gga_handler[i] == NULL) {
      gga_handler[i] = handler;
      // This cast won't be an issue, there's no way "i" will be big enought to
      // cause an overflow
      if (index) {
        *index = i;
      }
      return STATUS_CODE_OK;
    }
  }
  return STATUS_CODE_UNKNOWN;
}

StatusCode remove_gps_handler(size_t index) {
  if (index >= GPS_HANDLER_ARRAY_LENGTH) return STATUS_CODE_UNKNOWN;
  gps_handler[index] = NULL;
  return STATUS_CODE_OK;
}

StatusCode remove_gga_handler(size_t index) {
  if (index >= GPS_HANDLER_ARRAY_LENGTH) return STATUS_CODE_UNKNOWN;
  gga_handler[index] = NULL;
  return STATUS_CODE_OK;
}

void stop_ON_OFF(SoftTimerID timer_id, void *context) {
  EvmSettings *settings = context;
  gpio_toggle_state(settings->pin_on_off);
  // Here we should wait for one second
  // During this period of time we should hear something from the GPS chip
  // If not, start again from pull_ON_OFF
}

// This is useful as a method for callbacks
void evm_toggle_chip_power(SoftTimerID timer_id, void *context) {
  EvmSettings *settings = context;
  gpio_toggle_state(settings->pin_power);
}

// This callback should start the initialization sequence
void pull_ON_OFF(SoftTimerID timer_id, void *context) {
  EvmSettings *settings = context;

  // Can't really return a StatusCode here. Might need a messy work around / leave it?
  gpio_toggle_state(settings->pin_on_off);
  soft_timer_start_millis(100, stop_ON_OFF, settings, NULL);
}

// Initialization of this chip is described on page 10 of:
// https://www.linxtechnologies.com/wp/wp-content/uploads/rxm-gps-f4.pdf
StatusCode evm_gps_init(EvmSettings *settings) {
  memset(&gps_handler, 0, SIZEOF_ARRAY(gps_handler));
  memset(&gga_handler, 0, SIZEOF_ARRAY(gga_handler));
  memset(&s_storage, 0, sizeof(s_storage));

  // Making sure all settings as passed in
  // Maybe this should be made into its own method?
  if (!settings) {
    return status_msg(STATUS_CODE_INVALID_ARGS, "The 'settings' argument is null\n");
  }
  if (!settings->uart_settings) {
    return status_msg(STATUS_CODE_INVALID_ARGS, "The 'settings->uart_settings' argument is null\n");
  }
  if (!settings->settings_tx) {
    return status_msg(STATUS_CODE_INVALID_ARGS, "The 'settings->settings_tx' argument is null\n");
  }
  if (!settings->settings_rx) {
    return status_msg(STATUS_CODE_INVALID_ARGS, "The 'settings->settings_rx' argument is null\n");
  }
  if (!settings->settings_power) {
    return status_msg(STATUS_CODE_INVALID_ARGS,
                      "The 'settings->settings_power' argument is null\n");
  }
  if (!settings->settings_power) {
    return status_msg(STATUS_CODE_INVALID_ARGS,
                      "The 'settings->settings_on_off' argument is null\n");
  }
  if (!settings->pin_rx) {
    return status_msg(STATUS_CODE_INVALID_ARGS, "The 'settings->pin_rx' argument is null\n");
  }
  if (!settings->pin_tx) {
    return status_msg(STATUS_CODE_INVALID_ARGS, "The 'settings->pin_tx' argument is null\n");
  }
  if (!settings->pin_power) {
    return status_msg(STATUS_CODE_INVALID_ARGS, "The 'settings->pin_power' argument is null\n");
  }
  if (!settings->pin_on_off) {
    return status_msg(STATUS_CODE_INVALID_ARGS, "The 'settings->pin_on_off' argument is null\n");
  }

  settings->uart_settings->rx_handler = s_nmea_read;
  // Makes sure that status codes are handled
  status_ok_or_return(uart_init(*(settings->port), settings->uart_settings, &s_storage));

  // These should already be initialized, but we do so anyway, to make sure
  interrupt_init();
  soft_timer_init();
  status_ok_or_return(gpio_init());

  status_ok_or_return(gpio_init_pin(settings->pin_tx, settings->settings_tx));
  status_ok_or_return(gpio_init_pin(settings->pin_rx, settings->settings_rx));
  status_ok_or_return(gpio_init_pin(settings->pin_power, settings->settings_power));
  status_ok_or_return(gpio_init_pin(settings->pin_on_off, settings->settings_on_off));

  // From the documentation: Power needs to be on for one second before continuing
  evm_toggle_chip_power(0, settings);
  status_ok_or_return(soft_timer_start_seconds(1, pull_ON_OFF, settings, NULL));
  return STATUS_CODE_OK;
}

// Implementing shut down here:
// From page 25 of:
// https://www.linxtechnologies.com/wp/wp-content/uploads/rxm-gps-f4.pdf
StatusCode evm_gps_clean_up(EvmSettings *settings) {
  // This string is taken from the pdf mentioned above. The \r\n may not be necessary depending on
  // if the uart library adds it automatically or not.
  // The char array below should read $PSRF117,16*0B\r\n
  uint8_t message[] = { '$', 'P', 'S', 'R', 'F', '1', '1',  '7',
                        ',', '1', '6', '*', '0', 'B', '\r', '\n' };
  status_ok_or_return(uart_tx(*(settings->port), message, sizeof(message) / sizeof(message[0])));
  status_ok_or_return(soft_timer_start_seconds(1, evm_toggle_chip_power, settings, NULL));
  return STATUS_CODE_OK;
}
