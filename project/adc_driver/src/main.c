#include "adc.h"
#include "gpio.h"

int main() {
  GPIOAddress address = { 0, 0 };
  GPIOSettings settings = { GPIO_DIR_IN, GPIO_STATE_LOW, GPIO_RES_NONE, GPIO_ALTFN_ANALOG };

  gpio_init();
  gpio_init_pin(&address, &settings);

  adc_init(ADC_MODE_CONTINUOUS);
  adc_init_pin(&address, ADC_SAMPLE_RATE_1);

  while (1) {
    printf("%d mV\n", adc_read(&address, 3000));
  }
}
