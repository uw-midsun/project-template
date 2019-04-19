#include "button_led_radio_fsm.h"

#include "gpio.h"
#include "input_event.h"

typedef struct RadioButtonFsmCtx {
  GpioExpanderStorage *expander_storage;

  GpioExpanderPin drive_pin;
  GpioExpanderPin neutral_pin;
  GpioExpanderPin reverse_pin;
} RadioButtonFsmCtx;

static RadioButtonFsmCtx s_fsm_ctxs;

static void prv_button_drive_on(Fsm *fsm, const Event *e, void *context) {
  // All LEDs should be off except for Drive
  RadioButtonFsmCtx *button_fsm_ctx = context;

  gpio_expander_set_state(button_fsm_ctx->expander_storage, button_fsm_ctx->neutral_pin,
                          GPIO_STATE_LOW);
  gpio_expander_set_state(button_fsm_ctx->expander_storage, button_fsm_ctx->reverse_pin,
                          GPIO_STATE_LOW);

  gpio_expander_set_state(button_fsm_ctx->expander_storage, button_fsm_ctx->drive_pin,
                          GPIO_STATE_HIGH);
}

static void prv_button_neutral_on(Fsm *fsm, const Event *e, void *context) {
  // All LEDs should be off except for Neutral
  RadioButtonFsmCtx *button_fsm_ctx = context;

  gpio_expander_set_state(button_fsm_ctx->expander_storage, button_fsm_ctx->reverse_pin,
                          GPIO_STATE_LOW);
  gpio_expander_set_state(button_fsm_ctx->expander_storage, button_fsm_ctx->drive_pin,
                          GPIO_STATE_LOW);

  gpio_expander_set_state(button_fsm_ctx->expander_storage, button_fsm_ctx->neutral_pin,
                          GPIO_STATE_HIGH);
}

static void prv_button_reverse_on(Fsm *fsm, const Event *e, void *context) {
  // Go through all pins and turn them all off
  RadioButtonFsmCtx *button_fsm_ctx = context;

  gpio_expander_set_state(button_fsm_ctx->expander_storage, button_fsm_ctx->drive_pin,
                          GPIO_STATE_LOW);
  gpio_expander_set_state(button_fsm_ctx->expander_storage, button_fsm_ctx->neutral_pin,
                          GPIO_STATE_LOW);

  gpio_expander_set_state(button_fsm_ctx->expander_storage, button_fsm_ctx->reverse_pin,
                          GPIO_STATE_HIGH);
}

FSM_DECLARE_STATE(button_group_neutral_on);
FSM_DECLARE_STATE(button_group_drive_on);
FSM_DECLARE_STATE(button_group_reverse_on);

FSM_STATE_TRANSITION(button_group_neutral_on) {
  FSM_ADD_TRANSITION(CENTER_CONSOLE_EVENT_BUTTON_DRIVE_PRESSED, button_group_drive_on);
  FSM_ADD_TRANSITION(CENTER_CONSOLE_EVENT_BUTTON_REVERSE_PRESSED, button_group_reverse_on);
}

FSM_STATE_TRANSITION(button_group_drive_on) {
  FSM_ADD_TRANSITION(CENTER_CONSOLE_EVENT_BUTTON_NEUTRAL_PRESSED, button_group_neutral_on);
  FSM_ADD_TRANSITION(CENTER_CONSOLE_EVENT_BUTTON_REVERSE_PRESSED, button_group_reverse_on);
}

FSM_STATE_TRANSITION(button_group_reverse_on) {
  FSM_ADD_TRANSITION(CENTER_CONSOLE_EVENT_BUTTON_DRIVE_PRESSED, button_group_drive_on);
  FSM_ADD_TRANSITION(CENTER_CONSOLE_EVENT_BUTTON_NEUTRAL_PRESSED, button_group_neutral_on);
}

void button_led_radio_fsm_init(void) {
  fsm_state_init(button_group_neutral_on, prv_button_neutral_on);
  fsm_state_init(button_group_drive_on, prv_button_drive_on);
  fsm_state_init(button_group_reverse_on, prv_button_reverse_on);
}

StatusCode button_led_radio_fsm_create(Fsm *fsm, GpioExpanderStorage *expander_storage,
                                       ButtonLedRadioSettings *settings, const char *fsm_name) {
  s_fsm_ctxs.expander_storage = expander_storage;
  s_fsm_ctxs.drive_pin = settings->drive_pin;
  s_fsm_ctxs.neutral_pin = settings->neutral_pin;
  s_fsm_ctxs.reverse_pin = settings->reverse_pin;

  fsm_init(fsm, fsm_name, &button_group_neutral_on, &s_fsm_ctxs);

  return STATUS_CODE_OK;
}
