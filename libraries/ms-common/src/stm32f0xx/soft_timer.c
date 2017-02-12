#include "soft_timer.h"

#include <stdbool.h>
#include <stdint.h>

#include "critical_section.h"
#include "interrupt.h"
#include "status.h"
#include "stm32f0xx_interrupt.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_tim.h"

typedef struct Timer {
  uint32_t duration;
  void *context;
  bool inuse;
  SoftTimerCallback callback;
} Timer;

static Timer s_soft_timer_elems[SOFT_TIMER_MAX_TIMERS];
static SoftTimerID s_active_timer_id;
static uint32_t s_active_timer_duration;

void prv_start_timer(uint32_t duration_us) {
  // Stop the timer.
  TIM_Cmd(TIM2, DISABLE);

  // Clear any pending interrupts, this will always occur in an interrupt or new timer getting
  // started in a critical section so we need to prevent a accidental trigger if the time expires in
  // the middle of the critical section and we started a new timer which falls before it.
  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

  // Get the clock speed of the clocks to determine the SYSCLK speed which is what TIM2 uses.
  RCC_ClocksTypeDef clock_speeds;
  RCC_GetClocksFreq(&clock_speeds);

  // Configure each clock tick to be 1 microsecond from 0 to duration.
  TIM_TimeBaseInitTypeDef init_struct = { .TIM_Prescaler = clock_speeds.SYSCLK_Frequency / 1000000,
                                          .TIM_CounterMode = TIM_CounterMode_Up,
                                          .TIM_Period = duration_us,
                                          .TIM_ClockDivision = TIM_CKD_DIV1 };
  s_active_timer_duration = duration_us;

  // Reset the counter to 0.
  TIM_SetCounter(TIM2, 0);

  // Start the timer.
  TIM_TimeBaseInit(TIM2, &init_struct);
  TIM_Cmd(TIM2, ENABLE);
}

void prv_soft_timer_update(uint32_t curr_time) {
  // Default these values to max. If there is ever a newer timer it will replace them. In the event
  // there are no timers left the defaults will result in the module being reset to a state where no
  // timers exist.
  SoftTimerID min_time_id = SOFT_TIMER_MAX_TIMERS;
  uint32_t min_duration = UINT32_MAX;
  for (uint32_t i = 0; i < SOFT_TIMER_MAX_TIMERS; i++) {
    // If the timer is in use and wasn't just added then update its duration.
    if (s_soft_timer_elems[i].inuse) {
      if (curr_time >= s_soft_timer_elems[i].duration) {
        // For each inuse timer see if the timer expired. If it has run its callback.
        s_soft_timer_elems[i].callback(i, s_soft_timer_elems[i].context);
        s_soft_timer_elems[i].inuse = false;
      } else {
        // Otherwise, update the duration left on the timer.
        s_soft_timer_elems[i].duration -= curr_time;
        // Figure out the next timer.
        if (min_duration >= s_soft_timer_elems[i].duration) {
          min_time_id = i;
          min_duration = s_soft_timer_elems[i].duration;
        }
      }
    }
  }
  // Set the active timer id.
  s_active_timer_id = min_time_id;
}

void soft_timer_init(void) {
  // Start the PeiphClock
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  // Stop the timer if it was running and ensure only overflows trigger updates.
  // Also remove the ARR preload.
  TIM_Cmd(TIM2, DISABLE);
  TIM_UpdateRequestConfig(TIM2, TIM_UpdateSource_Regular);
  TIM_ARRPreloadConfig(TIM2, DISABLE);

  // Disable all the timers and forget the last running timer.
  s_active_timer_id = SOFT_TIMER_MAX_TIMERS;
  for (uint32_t i = 0; i < SOFT_TIMER_MAX_TIMERS; i++) {
    s_soft_timer_elems[i].inuse = false;
  }

  // Enable the interrupts. TIM2 uses IRQ channel 15.
  stm32f0xx_interrupt_nvic_enable(15, INTERRUPT_PRIORITY_NORMAL);
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}

StatusCode soft_timer_start(uint32_t duration_us, SoftTimerCallback callback, void *context,
                            SoftTimerID *timer_id) {
  // Get the time this was called.
  uint32_t curr_time = TIM_GetCounter(TIM2);

  // Enable a critical section.
  bool critical = critical_section_start();

  for (uint32_t i = 0; i < SOFT_TIMER_MAX_TIMERS; i++) {
    if (!s_soft_timer_elems[i].inuse) {
      // Look for an empty timer.

      if (s_active_timer_id >= SOFT_TIMER_MAX_TIMERS) {
        // New timer will be the only timer. Start it.
        s_active_timer_id = i;
        prv_start_timer(duration_us);
      } else if (duration_us < s_soft_timer_elems[s_active_timer_id].duration - curr_time) {
        // New timer will run be before the active timer. Update and start it. Use as close to now
        // as possible for the update.
        prv_soft_timer_update(TIM_GetCounter(TIM2));
        s_active_timer_id = i;
        prv_start_timer(duration_us);
      } else {
        // Add the current time to cancel what was on the clock when called.
        duration_us += curr_time;
      }
      // Otherwise, the new timer is longest ignore it until later.

      // Actually populate the timer. Do this after the previous steps as this is a critical section
      // and this timer shouldn't be updated in prv_soft_timer_update.
      s_soft_timer_elems[i].duration = duration_us;
      s_soft_timer_elems[i].callback = callback;
      s_soft_timer_elems[i].context = context;
      s_soft_timer_elems[i].inuse = true;
      *timer_id = i;
      critical_section_end(critical);
      return STATUS_CODE_OK;
    }
  }

  // Out of timers.
  critical_section_end(critical);
  return status_msg(STATUS_CODE_RESOURCE_EXHAUSTED, "Out of software timers.");
}

bool soft_timer_inuse(void) {
  for (uint32_t i = 0; i < SOFT_TIMER_MAX_TIMERS; i++) {
    if (s_soft_timer_elems[i].inuse) {
      return true;
    }
  }

  return false;
}

// TIM2 Interrupt handler as defined in stm32f0xx.h
void TIM2_IRQHandler(void) {
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
    // Stop the timer to prevent repeat triggers if it is a really short period.
    TIM_Cmd(TIM2, DISABLE);

    // Update the timer assuming the active timer expired.
    prv_soft_timer_update(s_active_timer_duration);

    // Start the next active timer if it exists as determined by the update.
    if (s_active_timer_id < SOFT_TIMER_MAX_TIMERS) {
      prv_start_timer(s_soft_timer_elems[s_active_timer_id].duration);
      return;
    }

    // Clear the pending bit to stop a repeat trigger if there are no new timers.
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  }
}
