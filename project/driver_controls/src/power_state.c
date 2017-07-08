#include "power_state.h"

FSM_DECLARE_STATE(state_off);
FSM_DECLARE_STATE(state_on);

// State machine transition tables
FSM_STATE_TRANSITION(state_off) {
  FSM_ADD_TRANSITION(INPUT_EVENT_POWER, state_on);
}

FSM_STATE_TRANSITION(state_on) {
  FSM_ADD_TRANSITION(INPUT_EVENT_POWER, state_off);
}

// Transition check functions
static bool prv_check_off(const Event *e) {
  return (e->id == INPUT_EVENT_POWER || e->id == INPUT_EVENT_MECHANICAL_BRAKE);
}

static bool prv_check_on(const Event *e) {
  return true;
}

// State output functions
static void prv_state_off(FSM *fsm, const Event *e, void *context) {
  fsm->context = prv_check_off;
}

static void prv_state_on(FSM *fsm, const Event *e, void *context) {
  fsm->context = prv_check_on;
}

void power_state_init(FSM *power_fsm, void *context) {
  fsm_state_init(state_off, prv_state_off);
  fsm_state_init(state_on, prv_state_on);

  fsm_init(power_fsm, "power_fsm", &state_off, prv_check_off);
}
