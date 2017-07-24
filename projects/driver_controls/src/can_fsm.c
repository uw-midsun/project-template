#include "can_fsm.h"
#include "input_event.h"
#include "event_arbiter.h"
#include "log.h"

FSM_DECLARE_STATE(state_can_transmit);

FSM_STATE_TRANSITION(state_can_transmit) {
  FSM_ADD_TRANSITION(INPUT_EVENT_CAN_ID_POWER, state_can_transmit);
  FSM_ADD_TRANSITION(INPUT_EVENT_CAN_ID_PEDAL, state_can_transmit);
  FSM_ADD_TRANSITION(INPUT_EVENT_CAN_ID_DIRECTION_SELECTOR, state_can_transmit);
  FSM_ADD_TRANSITION(INPUT_EVENT_CAN_ID_TURN_SIGNAL, state_can_transmit);
  FSM_ADD_TRANSITION(INPUT_EVENT_CAN_ID_HAZARD_LIGHT, state_can_transmit);
  FSM_ADD_TRANSITION(INPUT_EVENT_CAN_ID_MECHANICAL_BRAKE, state_can_transmit);
}

static StatusCode prv_transmit_data(FSM *fsm, const Event *e, void *context) {
  InputEventData *data = &e->data;

  printf("Device = %d, State = %d, Data = %d\n",
          e->id,
          data->components.state,
          data->components.data);
}

static bool prv_check_state(const Event *e) {
  return true;
}

StatusCode can_fsm_init(FSM *fsm) {
  fsm_state_init(state_can_transmit, prv_transmit_data);

  fsm_init(fsm, "can_fsm", &state_can_transmit, NULL);

  fsm->context = event_arbiter_add_fsm(fsm, prv_check_state);

  if (fsm->context == NULL) {
    return status_code(STATUS_CODE_RESOURCE_EXHAUSTED);
  }

  return STATUS_CODE_OK;
}
