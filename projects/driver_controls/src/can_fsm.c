#include "can_fsm.h"
#include "event_arbiter.h"
#include "input_event.h"
#include "log.h"

FSM_DECLARE_STATE(state_can_transmit);

FSM_STATE_TRANSITION(state_can_transmit) {
  FSM_ADD_TRANSITION(INPUT_EVENT_CAN_ID_POWER, state_can_transmit);
  FSM_ADD_TRANSITION(INPUT_EVENT_CAN_ID_PEDAL, state_can_transmit);
  FSM_ADD_TRANSITION(INPUT_EVENT_CAN_ID_DIRECTION_SELECTOR, state_can_transmit);
  FSM_ADD_TRANSITION(INPUT_EVENT_CAN_ID_TURN_SIGNAL, state_can_transmit);
  FSM_ADD_TRANSITION(INPUT_EVENT_CAN_ID_HAZARD_LIGHT, state_can_transmit);
  FSM_ADD_TRANSITION(INPUT_EVENT_CAN_ID_MECHANICAL_BRAKE, state_can_transmit);
  FSM_ADD_TRANSITION(INPUT_EVENT_CAN_ID_HORN, state_can_transmit);
  FSM_ADD_TRANSITION(INPUT_EVENT_CAN_ID_PUSH_TO_TALK, state_can_transmit);
}

static void prv_transmit_data(FSM *fsm, const Event *e, void *context) {
  InputEventData *data = (InputEventData *)&e->data;

  printf("Device = %d, State = %d, Data = %d\n", e->id, data->components.state,
         data->components.data);
}

StatusCode can_fsm_init(FSM *fsm) {
  fsm_state_init(state_can_transmit, prv_transmit_data);

  void *context = event_arbiter_add_fsm(fsm, NULL);

  if (context == NULL) {
    return status_code(STATUS_CODE_RESOURCE_EXHAUSTED);
  }

  fsm_init(fsm, "can_fsm", &state_can_transmit, context);

  return STATUS_CODE_OK;
}