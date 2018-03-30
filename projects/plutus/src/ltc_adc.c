#include "ltc_adc.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "delay.h"
#include "ltc2484.h"
#include "soft_timer.h"

uint8_t s_filter_modes[NUM_LTC_ADC_FILTER_MODES] = {
  LTC2484_REJECTION_50HZ_60HZ,
  LTC2484_REJECTION_50HZ,
  LTC2484_REJECTION_60HZ,
};

typedef struct {
  StatusCode status;
  int32_t value;
} LtcAdcBuffer;

static LtcAdcSettings *s_config;
static LtcAdcBuffer s_result_buffer = {
  .status = STATUS_CODE_UNINITIALIZED,
  .value = 0,
};
static SoftTimerID s_timer_id;

static void prv_toggle_pin_altfn(GPIOAddress addr, bool enable) {
  GPIOSettings settings = {
    .direction = GPIO_DIR_IN,
    .state = GPIO_STATE_HIGH,
    .resistor = GPIO_RES_NONE,
    .alt_function = GPIO_ALTFN_0,
  };

  if (enable) {
    settings.alt_function = GPIO_ALTFN_0;
  } else {
    settings.alt_function = GPIO_ALTFN_NONE;
  }

  gpio_init_pin(&addr, &settings);
}

static void prv_ltc_adc_read(SoftTimerID timer_id, void *context) {
  LtcAdcBuffer *buffer = (LtcAdcBuffer *)context;

  // Pull CS low so we can check for MISO to go low, signalling that the
  // conversion is now complete
  gpio_set_state(&s_config->cs, GPIO_STATE_LOW);

  // Disable the Alt Fn on MISO so we can read the value
  prv_toggle_pin_altfn(s_config->miso, false);

  // According to the Timing Characteristics (p.5 in the datasheet), we should
  // expect 149.9ms for conversion time (in the worst case).
  GPIOState state = NUM_GPIO_STATES;
  gpio_get_state(&s_config->miso, &state);

  // Restore CS high so we can trigger a SPI exchange
  gpio_set_state(&s_config->cs, GPIO_STATE_HIGH);

  // Restore the Alt Fn of the MISO pin
  prv_toggle_pin_altfn(s_config->miso, true);

  if (state != GPIO_STATE_LOW) {
    // MISO has now gone low, signaling that the conversion has finished
    buffer->status = status_code(STATUS_CODE_TIMEOUT);
  }

  // Keep the previous mode and don't do anything special (ie. send a command
  // byte equal to 0). Since our SPI driver sends 0x00 by default, we can just
  // send NULL.
  //
  // Due to the way our SPI driver works, we send NULL bytes in order to ensure
  // that 4 bytes are exchanged in total.
  uint8_t result[4] = { 0 };
  StatusCode status = spi_exchange(s_config->spi_port, NULL, 0, result, 4);

  buffer->status = ltc2484_raw_adc_to_uv(result, &buffer->value);

  soft_timer_start_millis(200, prv_ltc_adc_read, &s_result_buffer, &s_timer_id);
}

StatusCode ltc_adc_init(const LtcAdcSettings *config) {
  if (config->filter_mode >= NUM_LTC_ADC_FILTER_MODES) {
    return status_code(STATUS_CODE_INVALID_ARGS);
  }

  // The LTC2484 uses SPI Mode 0 (see Figure 5 on p.20 in the datasheet)
  SPISettings spi_config = {
    .baudrate = config->spi_baudrate,
    .mode = SPI_MODE_0,
    .mosi = config->mosi,
    .miso = config->miso,
    .sclk = config->sclk,
    .cs = config->cs,
  };

  spi_init(config->spi_port, &spi_config);

  uint8_t input[1] = { LTC2484_ENABLE | LTC2484_EXTERNAL_INPUT | LTC2484_AUTO_CALIBRATION |
                       s_filter_modes[config->filter_mode] };
  // send config
  spi_exchange(config->spi_port, input, 1, NULL, 0);

  s_result_buffer.status = STATUS_CODE_UNINITIALIZED;
  s_result_buffer.value = 0;
  s_config = config;

  // Wait for at least 200ms before attempting another read
  //
  // This hacky solution is to get around the fact that isoSPI seems to require
  // a clock signal in order to encode a change in state on the MISO pin. Since
  // the LTC2484 uses the MISO pin to signal that the EOC has completed
  // (without requiring a clock signal), we are unable to test that correctly,
  // and thus we are forced to delay for the maximum conversion time before
  // performing another read.
  return soft_timer_start_millis(200, prv_ltc_adc_read, &s_result_buffer, &s_timer_id);
}

StatusCode ltc2484_raw_adc_to_uv(uint8_t *spi_data, int32_t *voltage) {
  Ltc2484Response tmp;

  tmp.u8data[0] = spi_data[3];
  tmp.u8data[1] = spi_data[2];
  tmp.u8data[2] = spi_data[1];
  tmp.u8data[3] = spi_data[0];

  // We have an overrange condition when the SIG and MSB bits are both set
  // (see Table 3 on p.16 of the datasheet)
  if ((spi_data[0] & LTC2484_ERROR_CODE_MASK) == LTC2484_OVERRANGE_CODE) {
    return status_code(STATUS_CODE_OUT_OF_RANGE);
  }
  // We have an underrange condition when the SIG and MSB bits are not set and
  // B27-25 are set
  if ((spi_data[0] & LTC2484_ERROR_CODE_MASK) == LTC2484_UNDERRANGE_CODE) {
    return status_code(STATUS_CODE_OUT_OF_RANGE);
  }

  // Sign extend tmp.i32data as two's complement
  // We don't care about the following values, so we can shift them out:
  // - The higher 3 bits are the SIG, DMY and MSB
  // - The lower 4 bits are the configuration word from the previous conversion
  int32_t adc_value = ((tmp.i32data << 3) >> 8);

  // Convert the voltage to uV
  // 2^24 * 4092 * 1000 = 6.86523679e13 (which fits in an int64_t)
  // dividing by 2^24 gives 4092000, which fits in an int32_t
  *voltage = (int32_t)(((int64_t)(adc_value)*LTC2484_V_REF_MILLIVOLTS * 1000) >> 24);

  return STATUS_CODE_OK;
}

StatusCode ltc_adc_get_value(const LtcAdcSettings *config, int32_t *value) {
  StatusCode result = s_result_buffer.status;
  *value = s_result_buffer.value;

  s_result_buffer.status = STATUS_CODE_UNINITIALIZED;
  s_result_buffer.value = 0;

  return result;
}