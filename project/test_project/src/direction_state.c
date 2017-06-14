#include "direction_state.h"

FSM_DECLARE_STATE(state_neutral);
FSM_DECLARE_STATE(state_forward);
FSM_DECLARE_STATE(state_reverse);

// Transition table for direction state machine

FSM_STATE_TRANSITION(state_neutral) {
  FSM_ADD_TRANSITION(INPUT_EVENT_DIRECTION_SELECTOR_DRIVE, state_forward);
  FSM_ADD_TRANSITION(INPUT_EVENT_DIRECTION_SELECTOR_REVERSE, state_reverse);
}

FSM_STATE_TRANSITION(state_forward) {
  FSM_ADD_TRANSITION(INPUT_EVENT_DIRECTION_SELECTOR_NEUTRAL, state_neutral);
  FSM_ADD_TRANSITION(INPUT_EVENT_DIRECTION_SELECTOR_REVERSE, state_reverse);
}

FSM_STATE_TRANSITION(state_reverse) {
  FSM_ADD_TRANSITION(INPUT_EVENT_DIRECTION_SELECTOR_DRIVE, state_forward);
  FSM_ADD_TRANSITION(INPUT_EVENT_DIRECTION_SELECTOR_NEUTRAL, state_neutral);
}

// Output functions for the direction state

static void prv_driver_state_neutral(FSM* fsm, const Event* e, FSMGroup* fsm_group) {
  fsm_group->direction.state = STATE_NEUTRAL;
}

static void prv_driver_state_forward(FSM* fsm, const Event* e, FSMGroup* fsm_group) {
  fsm_group->direction.state = STATE_FORWARD;
}

static void prv_driver_state_reverse(FSM* fsm, const Event* e, FSMGroup* fsm_group) {
  fsm_group->direction.state = STATE_REVERSE;
}

void direction_state_init(FSM* direction_fsm, FSMGroup* fsm_group) {
	fsm_state_init(state_neutral, prv_driver_state_neutral);
	fsm_state_init(state_forward, prv_driver_state_forward);
	fsm_state_init(state_reverse, prv_driver_state_reverse);

	fsm_init(direction_fsm, "direction_fsm", &state_neutral, fsm_group);
}