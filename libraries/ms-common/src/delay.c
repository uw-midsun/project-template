#include "delay.h"

#include <stdbool.h>
#include <stddef.h>

#include "soft_timer.h"
#include "wait.h"

static void prv_delay_it(SoftTimerID timer_id, void *context) {
  bool *block = context;
  *block = false;
}

void delay_us(uint32_t t) {
  volatile bool block = true;
  soft_timer_start(t, prv_delay_it, &block, NULL);
  while (block) {
    wait();
  }
}
