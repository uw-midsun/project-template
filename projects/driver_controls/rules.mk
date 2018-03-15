# Defines $(T)_SRC, $(T)_INC, $(T)_DEPS, and $(T)_CFLAGS for the build makefile.
# Pre-defined:
# $(T)_SRC_ROOT: $(T)_DIR/src
# $(T)_INC_DIRS: $(T)_DIR/inc
# $(T)_SRC: $(T)_DIR/src{/$(PLATFORM}}/*.{c,s}
# $(T)_INC: $(T)_DIR/inc{/$(PLATFORM}}/*.h

# Specify the device library you want to include
$(T)_DEPS := ms-common

ifeq (x86,$(PLATFORM))
$(T)_EXCLUDE_TESTS := gpio_expander magnetic_sensor 
endif

