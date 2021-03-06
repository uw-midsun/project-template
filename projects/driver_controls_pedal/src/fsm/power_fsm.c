// The driver uses the mechanical brake to control the powered state of the car.

// The car initializes in the off state:
//    - Pressing the power button without holding down the brake will cause the car
//      to transition from the off to the charging state, and vice versa
//    - Pressing the power button while the mechanical brake is held down will cause the car
//      to transition between the off and the on state.
// Note that the additional "brake" states are to ensure that braking behavior is consistent
// i.e. If the mechanical brake is held during a fault, it does not need to be released and
// reapplied when pressing the power button to enter drive. If these states did not exist, the car
// would enter charging instead as the mechanical brake state is lost.
// The alternative would be to use guarded transitions and expose the mechanical brake state.

#include "power_fsm.h"

#include "bps_indicator.h"
#include "drive_output.h"
#include "event_arbiter.h"
#include "exported_enums.h"
#include "log.h"
#include "pedal_events.h"
#include "power_distribution_controller.h"

// Power FSM state definitions
FSM_DECLARE_STATE(state_off);
FSM_DECLARE_STATE(state_off_brake);
FSM_DECLARE_STATE(state_charging);
FSM_DECLARE_STATE(state_on);
FSM_DECLARE_STATE(state_fault);

// Power FSM transition table definitions
FSM_STATE_TRANSITION(state_off) {
  FSM_ADD_TRANSITION(PEDAL_EVENT_INPUT_MECHANICAL_BRAKE_PRESSED, state_off_brake);
  FSM_ADD_TRANSITION(PEDAL_EVENT_INPUT_CENTER_CONSOLE_POWER_PRESSED, state_charging);

  FSM_ADD_TRANSITION(PEDAL_EVENT_INPUT_BPS_FAULT, state_fault);
  FSM_ADD_TRANSITION(PEDAL_EVENT_QUEUEING_FAULT, state_fault);
}

FSM_STATE_TRANSITION(state_off_brake) {
  FSM_ADD_TRANSITION(PEDAL_EVENT_INPUT_CENTER_CONSOLE_POWER_PRESSED, state_on);
  FSM_ADD_TRANSITION(PEDAL_EVENT_INPUT_MECHANICAL_BRAKE_RELEASED, state_off);

  FSM_ADD_TRANSITION(PEDAL_EVENT_INPUT_BPS_FAULT, state_fault);
  FSM_ADD_TRANSITION(PEDAL_EVENT_QUEUEING_FAULT, state_fault);
}

FSM_STATE_TRANSITION(state_charging) {
  FSM_ADD_TRANSITION(PEDAL_EVENT_INPUT_CENTER_CONSOLE_POWER_PRESSED, state_off);

  FSM_ADD_TRANSITION(PEDAL_EVENT_INPUT_BPS_FAULT, state_fault);
  FSM_ADD_TRANSITION(PEDAL_EVENT_QUEUEING_FAULT, state_fault);
}

FSM_STATE_TRANSITION(state_on) {
  FSM_ADD_TRANSITION(PEDAL_EVENT_INPUT_CENTER_CONSOLE_POWER_PRESSED, state_off);

  FSM_ADD_TRANSITION(PEDAL_EVENT_INPUT_BPS_FAULT, state_fault);
  FSM_ADD_TRANSITION(PEDAL_EVENT_QUEUEING_FAULT, state_fault);
}

FSM_STATE_TRANSITION(state_fault) {
  FSM_ADD_TRANSITION(PEDAL_EVENT_INPUT_CENTER_CONSOLE_POWER_PRESSED, state_off);
}

// Power FSM arbiter functions
static bool prv_guard_off(const Event *e) {
  // The only valid events when the car isn't in drive are the power button,
  // mechanical brake, and fault.
  //
  // This also prevents lights, etc. from being turned on unless the
  // unprotected rail is powered.
  switch (e->id) {
    case PEDAL_EVENT_QUEUEING_FAULT:
    case PEDAL_EVENT_INPUT_BPS_FAULT:
    case PEDAL_EVENT_INPUT_POWER_STATE_DRIVE:
    case PEDAL_EVENT_INPUT_POWER_STATE_CHARGE:
    case PEDAL_EVENT_INPUT_POWER_STATE_OFF:
    case PEDAL_EVENT_INPUT_POWER_STATE_FAULT:
    case PEDAL_EVENT_INPUT_HAZARDS_STATE_OFF:
    case PEDAL_EVENT_INPUT_CENTER_CONSOLE_HAZARDS_PRESSED:
    case PEDAL_EVENT_INPUT_CENTER_CONSOLE_POWER_PRESSED:
    case PEDAL_EVENT_INPUT_MECHANICAL_BRAKE_RELEASED:
    case PEDAL_EVENT_INPUT_MECHANICAL_BRAKE_PRESSED:
      /* LOG_DEBUG("e->id: %d\n", e->id); */
      return true;
    default:
      return false;
  }
}

// Power FSM output functions
static void prv_off_output(Fsm *fsm, const Event *e, void *context) {
  EventArbiterGuard *guard = fsm->context;
  power_distribution_controller_send_update(EE_POWER_STATE_IDLE);

  bps_indicator_clear_fault();

  // Disable periodic drive output updates if not running
  drive_output_set_enabled(drive_output_global(), false);
  event_arbiter_set_guard_fn(guard, prv_guard_off);

  event_raise(PEDAL_EVENT_INPUT_POWER_STATE_OFF, 0);
  LOG_DEBUG("Off\n");
}

static void prv_drive_output(Fsm *fsm, const Event *e, void *context) {
  EventArbiterGuard *guard = fsm->context;
  power_distribution_controller_send_update(EE_POWER_STATE_DRIVE);

  // Allow all events and begin sending periodic drive commands
  drive_output_set_enabled(drive_output_global(), true);
  event_arbiter_set_guard_fn(guard, NULL);

  event_raise(PEDAL_EVENT_INPUT_POWER_STATE_DRIVE, 0);
  LOG_DEBUG("Drive\n");
}

static void prv_fault_output(Fsm *fsm, const Event *e, void *context) {
  EventArbiterGuard *guard = fsm->context;

  if (e->id == PEDAL_EVENT_INPUT_BPS_FAULT && e->data) {
    bps_indicator_set_fault();
  }

  // Disable periodic drive output updates if not running
  drive_output_set_enabled(drive_output_global(), false);
  event_arbiter_set_guard_fn(guard, prv_guard_off);

  event_raise(PEDAL_EVENT_INPUT_POWER_STATE_FAULT, 0);
  LOG_DEBUG("Fault\n");
}

static void prv_charge_output(Fsm *fsm, const Event *e, void *context) {
  EventArbiterGuard *guard = fsm->context;
  power_distribution_controller_send_update(EE_POWER_STATE_CHARGE);

  // Disable periodic drive output updates if not running
  drive_output_set_enabled(drive_output_global(), false);
  // Allow lights, etc to turn on
  event_arbiter_set_guard_fn(guard, NULL);

  event_raise(PEDAL_EVENT_INPUT_POWER_STATE_CHARGE, 0);
  LOG_DEBUG("Charging\n");
}

StatusCode power_fsm_init(Fsm *fsm, EventArbiterStorage *storage) {
  fsm_state_init(state_off, prv_off_output);
  fsm_state_init(state_off_brake, prv_off_output);
  fsm_state_init(state_charging, prv_charge_output);
  fsm_state_init(state_on, prv_drive_output);
  fsm_state_init(state_fault, prv_fault_output);

  EventArbiterGuard *guard = event_arbiter_add_fsm(storage, fsm, prv_guard_off);

  if (guard == NULL) {
    return status_code(STATUS_CODE_RESOURCE_EXHAUSTED);
  }

  fsm_init(fsm, "Power FSM", &state_off, guard);

  return STATUS_CODE_OK;
}
