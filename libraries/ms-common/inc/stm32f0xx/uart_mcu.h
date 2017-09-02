#pragma once

#include "gpio.h"

typedef enum {
  UART_PORT_1 = 0,
  UART_PORT_2,
  UART_PORT_3,
  UART_PORT_4,
  NUM_UART_PORTS,
} UARTPort;

#define UART_PORT_1_RXTX_GPIO_PA_ALTFN GPIO_ALTFN_1
#define UART_PORT_1_RXTX_GPIO_PB_ALTFN GPIO_ALTFN_0

#define UART_PORT_2_RXTX_GPIO_PA_ALTFN GPIO_ALTFN_1
#define UART_PORT_2_RXTX_GPIO_PD_ALTFN GPIO_ALTFN_0

#define UART_PORT_3_RXTX_GPIO_PB_ALTFN GPIO_ALTFN_4
#define UART_PORT_3_RXTX_GPIO_PC_ALTFN GPIO_ALTFN_1
#define UART_PORT_3_RXTX_GPIO_PD_ALTFN GPIO_ALTFN_0

#define UART_PORT_4_RXTX_GPIO_PA_ALTFN GPIO_ALTFN_4
#define UART_PORT_4_RXTX_GPIO_PC_ALTFN GPIO_ALTFN_0
