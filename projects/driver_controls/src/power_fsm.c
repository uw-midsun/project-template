#include "power_fsm.h"
#include "event_arbiter.h"
#include "input_event.h"
#include "log.h"
#include "can_output.h"

// Power FSM state definitions

FSM_DECLARE_STATE(state_off);
FSM_DECLARE_STATE(state_off_brake);
FSM_DECLARE_STATE(state_charging);
FSM_DECLARE_STATE(state_on);

// Power FSM transition table definitions

FSM_STATE_TRANSITION(state_off) {
  FSM_ADD_TRANSITION(INPUT_EVENT_POWER, state_charging);
  FSM_ADD_TRANSITION(INPUT_EVENT_MECHANICAL_BRAKE_PRESSED, state_off_brake);
}

FSM_STATE_TRANSITION(state_off_brake) {
  FSM_ADD_TRANSITION(INPUT_EVENT_POWER, state_on);
}

FSM_STATE_TRANSITION(state_charging) {
  FSM_ADD_TRANSITION(INPUT_EVENT_POWER, state_on);
}

FSM_STATE_TRANSITION(state_on) {
  FSM_ADD_TRANSITION(INPUT_EVENT_POWER, state_off);
}

// Power FSM arbiter functions
static bool prv_check_off(const Event *e) {
  // The car must accept a command to power on while off. It must also acknowledge mechanical brake
  // events, as the mechanical brake does not rely on the car being powered.
  switch (e->id) {
    case INPUT_EVENT_POWER:
    case INPUT_EVENT_MECHANICAL_BRAKE_RELEASED:
    case INPUT_EVENT_MECHANICAL_BRAKE_PRESSED:
      return true;
    default:
      return false;
  }
}

// Power FSM output functions

static void prv_state_off(FSM *fsm, const Event *e, void *context) {
  EventArbiterCheck *event_check = fsm->context;
  *event_check = prv_check_off;

  PowerFSMState power_state = POWER_FSM_STATE_OFF;

  State *current_state = fsm->current_state;

  if (current_state == &state_off_brake) {
    // No CAN message gets sent
    return;
  } else if (current_state == &state_charging) {
    power_state = POWER_FSM_STATE_CHARGED;
  }

  EventArbiterOutputData data = {
    .id = CAN_OUTPUT_MESSAGE_POWER,
    .state = power_state,
    .data = 0
  };

  event_arbiter_output(data);
}

static void prv_state_on(FSM *fsm, const Event *e, void *context) {
  EventArbiterCheck *event_check = fsm->context;
  *event_check = NULL;

  EventArbiterOutputData data = {
    .id = CAN_OUTPUT_MESSAGE_POWER,
    .state = POWER_FSM_STATE_ON,
    .data = 0
  };

  event_arbiter_output(data);
}

StatusCode power_fsm_init(FSM *fsm) {
  fsm_state_init(state_off, prv_state_off);
  fsm_state_init(state_off_brake, prv_state_off);
  fsm_state_init(state_charging, prv_state_off);
  fsm_state_init(state_on, prv_state_on);

  void *context = event_arbiter_add_fsm(fsm, prv_check_off);

  if (context == NULL) {
    return status_code(STATUS_CODE_RESOURCE_EXHAUSTED);
  }

  fsm_init(fsm, "power_fsm", &state_off, context);

  return STATUS_CODE_OK;
}
