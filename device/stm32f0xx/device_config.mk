# Specify Compiler
CC = arm-none-eabi-gcc
OBJCPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE = arm-none-eabi-size
AR = arm-none-eabi-ar

# Makes life easy if the library moves
STM32F0_DIR = libraries/stm32f0xx

# Architecture dependent variables
ARCH = -mlittle-endian -mcpu=cortex-m0 -march=armv6-m -mthumb

# Device Library specific variables
INC = -include $(STM32F0_DIR)/stm32f0xx_conf.h \
			-isystem $(STM32F0_DIR)/CMSIS/Include \
			-isystem $(STM32F0_DIR)/CMSIS/Device/ST/STM32F0xx/Include \
			-isystem $(STM32F0_DIR)/STM32F0xx_StdPeriph_Driver/inc \
			-isystem $(STM32F0_DIR)/STM32F0xx_StdPeriph_Driver/src \
			-I $(STM32F0_DIR)

# Linker and startup script locations
STARTUP = device/stm32f0xx/startup_stm32f0xx.s
LDSCRIPT = device/stm32f0xx/ldscripts

# Build flags for the device
CFLAGS = -Wall -Werror -g -Os -Wno-unused-variable -pedantic \
				 $(ARCH) -ffunction-sections -fdata-sections -Wl,--gc-sections \
				 -Wl,-Map=$(BIN_DIR)/$(PROJECT_NAME).map 

# Linker flags
LINKER = $(INC) -L$(LDSCRIPT) -Tstm32f0.ld 

# Device openocd config file
OPENOCD_CFG = $(STM32F0_DIR)/stm32f0-openocd.cfg
