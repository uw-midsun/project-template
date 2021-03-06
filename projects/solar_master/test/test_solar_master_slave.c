#include "i2c.h"
#include "mcp3427.h"
#include "test_helpers.h"
#include "unity.h"

#include "solar_master_config.h"
#include "solar_master_slave.h"

#define TEST_SLAVE_I2C_PORT I2C_PORT_2

static Mcp3427Storage s_slave_mcp3427[SOLAR_MASTER_NUM_SOLAR_SLAVES] = { 0 };
static SolarMasterSlave s_slave_storage[SOLAR_MASTER_NUM_SOLAR_SLAVES] = { 0 };

static const uint8_t slave_addr_lookup_reverse[8] = { 0, 1, 2, 3, 4, 5, 0, 0 };

static const Mcp3427PinState adc_address_map[SOLAR_MASTER_NUM_SOLAR_SLAVES][2] = {
  { MCP3427_PIN_STATE_LOW, MCP3427_PIN_STATE_LOW },     // I2C Address 0x68 i.e. 0x68 ^ 0x00
  { MCP3427_PIN_STATE_LOW, MCP3427_PIN_STATE_FLOAT },   // I2C Address 0x69 i.e. 0x68 ^ 0x01
  { MCP3427_PIN_STATE_LOW, MCP3427_PIN_STATE_HIGH },    // I2C Address 0x70 i.e. 0x68 ^ 0x02
  { MCP3427_PIN_STATE_FLOAT, MCP3427_PIN_STATE_LOW },   // I2C Address 0x71 i.e. 0x68 ^ 0x03
  { MCP3427_PIN_STATE_HIGH, MCP3427_PIN_STATE_LOW },    // I2C Address 0x72 i.e. 0x68 ^ 0x04
  { MCP3427_PIN_STATE_HIGH, MCP3427_PIN_STATE_FLOAT },  // I2C Address 0x73 i.e. 0x68 ^ 0x05
};

const Mcp3427Setting slave_mcp3427_settings_base = {
  .sample_rate = MCP3427_SAMPLE_RATE_12_BIT,
  .Adr0 = MCP3427_PIN_STATE_FLOAT,
  .Adr1 = MCP3427_PIN_STATE_FLOAT,
  .amplifier_gain = MCP3427_AMP_GAIN_1,
  .conversion_mode = MCP3427_CONVERSION_MODE_CONTINUOUS,
  .port = SOLAR_MASTER_SLAVE_I2C_BUS_PORT,
};

void setup_test(void) {
  const I2CSettings slave_i2c_settings = {
    .speed = I2C_SPEED_STANDARD,
    .sda = { GPIO_PORT_B, 11 },
    .scl = { GPIO_PORT_B, 10 },
  };
  TEST_ASSERT_OK(solar_master_config_init());
  TEST_ASSERT_OK(i2c_init(TEST_SLAVE_I2C_PORT, &slave_i2c_settings));
}

void teardown_test(void) {}

void test_solar_master_slave_init(void) {
  for (int i = 0; i < SOLAR_MASTER_NUM_SOLAR_SLAVES; i++) {
    Mcp3427Setting temp_slave_settings = slave_mcp3427_settings_base;
    temp_slave_settings.Adr0 = adc_address_map[i][0];
    temp_slave_settings.Adr1 = adc_address_map[i][1];
    TEST_ASSERT_OK(mcp3427_init(&(s_slave_mcp3427[i]), &temp_slave_settings));
    TEST_ASSERT_OK(solar_master_slave_init(&(s_slave_storage[i]), &(s_slave_mcp3427[i])));
  }
}
