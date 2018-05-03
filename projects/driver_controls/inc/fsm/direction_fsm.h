#pragma once

// Monitors the current state of the car's direction selector (Neutral, Forward, or Reverse)

// Coast and drive events are forbidden while the FSM is in the neutral state
// Power off events are forbidden while the FSM is in either the forward or reverse state

#include "event_arbiter.h"
#include "fsm.h"

StatusCode direction_fsm_init(FSM *fsm, EventArbiterStorage *storage);
