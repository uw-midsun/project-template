#include "turn_signal_fsm.h"
#include "event_arbiter.h"
#include "input_event.h"
#include "log.h"

// Turn signal FSM state definitions

FSM_DECLARE_STATE(state_no_signal);
FSM_DECLARE_STATE(state_left_signal);
FSM_DECLARE_STATE(state_right_signal);

// Turn signal FSM transition table definitions

FSM_STATE_TRANSITION(state_no_signal) {
  FSM_ADD_TRANSITION(INPUT_EVENT_TURN_SIGNAL_LEFT, state_left_signal);
  FSM_ADD_TRANSITION(INPUT_EVENT_TURN_SIGNAL_RIGHT, state_right_signal);
}

FSM_STATE_TRANSITION(state_left_signal) {
  FSM_ADD_TRANSITION(INPUT_EVENT_POWER, state_no_signal);
  FSM_ADD_TRANSITION(INPUT_EVENT_TURN_SIGNAL_NONE, state_no_signal);
  FSM_ADD_TRANSITION(INPUT_EVENT_TURN_SIGNAL_RIGHT, state_right_signal);
}

FSM_STATE_TRANSITION(state_right_signal) {
  FSM_ADD_TRANSITION(INPUT_EVENT_POWER, state_no_signal);
  FSM_ADD_TRANSITION(INPUT_EVENT_TURN_SIGNAL_LEFT, state_left_signal);
  FSM_ADD_TRANSITION(INPUT_EVENT_TURN_SIGNAL_NONE, state_no_signal);
}

// Turn signal FSM output function

static void prv_state_output(FSM *fsm, const Event *e, void *context) {
  InputEventData data = { 0 };

  // woah this was very wrong
  data.components.data = e->data;

  State *current_state = fsm->current_state;

  if (current_state == &state_no_signal) {
    data.components.state = TURN_SIGNAL_FSM_STATE_NO_SIGNAL;
  } else if (current_state == &state_left_signal) {
    data.components.state = TURN_SIGNAL_FSM_STATE_LEFT_SIGNAL;
  } else if (current_state == &state_right_signal) {
    data.components.state = TURN_SIGNAL_FSM_STATE_RIGHT_SIGNAL;
  }

  event_raise(INPUT_EVENT_CAN_ID_TURN_SIGNAL, data.raw);
}

StatusCode turn_signal_fsm_init(FSM *fsm) {
  fsm_state_init(state_no_signal, prv_state_output);
  fsm_state_init(state_left_signal, prv_state_output);
  fsm_state_init(state_right_signal, prv_state_output);

  void *context = event_arbiter_add_fsm(fsm, NULL);

  if (context == NULL) {
    return status_code(STATUS_CODE_RESOURCE_EXHAUSTED);
  }

  fsm_init(fsm, "turn_signal_fsm", &state_no_signal, context);

  return STATUS_CODE_OK;
}