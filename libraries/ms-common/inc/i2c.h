#pragma once
// Generic blocking I2C master driver
// Supports 7-bit addresses
#include <stdint.h>
#include <stddef.h>
#include "i2c_mcu.h"
#include "gpio.h"
#include "status.h"

typedef uint8_t I2CAddress;

typedef enum {
  I2C_SPEED_STANDARD = 0, // 100kHz
  I2C_SPEED_FAST, // 400 kHz
  I2C_SPEED_FAST_PLUS // 1 Mhz
} I2CSpeed;

typedef struct {
  I2CSpeed speed;
  GPIOAddress sda;
  GPIOAddress scl;
} I2CSettings;

StatusCode i2c_init(I2CPort i2c, const I2CSettings *settings);

StatusCode i2c_read(I2CPort i2c, I2CAddress addr, uint8_t opcode, uint8_t *rx_data, size_t rx_len);

StatusCode i2c_write(I2CPort i2c, I2CAddress addr, uint8_t opcode, uint8_t *tx_data, size_t tx_len);
