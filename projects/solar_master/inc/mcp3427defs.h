#pragma once

#define MCP3427_DEVICE_CODE 0xd
#define MCP3427_RDY_MASK (1 << 7)
#define MCP3427_CH_SEL_OFFSET 5
#define MCP3427_CONVERSION_MODE_OFFSET 4
#define MCP3427_SAMPLE_RATE_OFFSET 2
#define MCP3427_GAIN_SEL_OFFSET 0
#define MCP3427_NUM_CONFIG_BYTES 1
#define MCP3427_NUM_DATA_BYTES 3
#define MCP3427_DATA_MASK_12_BIT MCP3427_DATA_MASK_N_BIT(12)
#define MCP3427_DATA_MASK_14_BIT MCP3427_DATA_MASK_N_BIT(14)
#define MCP3427_DATA_MASK_16_BIT MCP3427_DATA_MASK_N_BIT(16)

#define MCP3427_DATA_MASK_N_BIT(N) ((1 << N) - 1)
