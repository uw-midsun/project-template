#include "input_interrupt.h"

#include <stdio.h>
#include <string.h>

// Replace the switch statement with a jump table if they get too big

static InputEvent prv_get_event(GPIOAddress* address, FSMGroup* fsm_group) {
	GPIOState key_pressed;
  gpio_get_value(address, &key_pressed);
	debounce(address, &key_pressed);

	switch (address->pin) {
		case 0:
			return (!strcmp(fsm_group->pedal_fsm.current_state->name, "state_off")) ? INPUT_EVENT_POWER_ON : INPUT_EVENT_POWER_OFF; 

		case 1:
			return (adc_read(address, MAX_SPEED) > COAST_THRESHOLD) ? INPUT_EVENT_GAS_PRESSED : INPUT_EVENT_GAS_RELEASED; 
			break;

		case 2:
			return (key_pressed) ? INPUT_EVENT_BRAKE_PRESSED : INPUT_EVENT_BRAKE_RELEASED; 
			break;

		case 3:
		case 4:
			switch ((GPIOB->IDR & (GPIO_IDR_3 | GPIO_IDR_4)) >> 3) {
				case 0:
					return INPUT_EVENT_DIRECTION_SELECTOR_NEUTRAL;
				case 1:
					return INPUT_EVENT_DIRECTION_SELECTOR_DRIVE;
				case 2: 
					return INPUT_EVENT_DIRECTION_SELECTOR_REVERSE;
			}
			break;

		case 5:
			return (strcmp(fsm_group->pedal_fsm.current_state->name, "state_cruise_control")) ? INPUT_EVENT_CRUISE_CONTROL_ON : INPUT_EVENT_CRUISE_CONTROL_OFF;
			break;

		case 6:
			if (!strcmp(fsm_group->pedal_fsm.current_state->name, "state_cruise_control")) {
				printf("Cruise control increase speed\n");
			}
			return INPUT_EVENT_CRUISE_CONTROL_INC; 
			break;

		case 7:
			if (!strcmp(fsm_group->pedal_fsm.current_state->name, "state_cruise_control")) {
				printf("Cruise control decrease speed\n");
			}
			return INPUT_EVENT_CRUISE_CONTROL_DEC;
			break;
		
		case 8:
		case 9:
			switch ((GPIOC->IDR & (GPIO_IDR_8 | GPIO_IDR_9)) >> 8) {
				case 0:
					return INPUT_EVENT_TURN_SIGNAL_NONE;
				case 1:
					return INPUT_EVENT_TURN_SIGNAL_LEFT;
				case 2:
					return INPUT_EVENT_TURN_SIGNAL_RIGHT;
			}
			break;

		case 10:
			return (!strcmp(fsm_group->hazard_light_fsm.current_state->name, "state_hazard_off")) ? INPUT_EVENT_HAZARD_LIGHT_ON : INPUT_EVENT_HAZARD_LIGHT_OFF;
	}
}

void input_callback (GPIOAddress* address, FSMGroup* fsm_group) {
	Event e = { prv_get_event(address, fsm_group), 0 };
	bool transitioned = 0;

	printf("GPIO_ISR =  "BYTE_TO_BINARY_PATTERN"\nEXTI_PR =   "BYTE_TO_BINARY_PATTERN"\nEXTI_RTSR = "BYTE_TO_BINARY_PATTERN"\nEXTI_FSTR = "BYTE_TO_BINARY_PATTERN"\n",
					BYTE_TO_BINARY(GPIOC->IDR), 
					BYTE_TO_BINARY(EXTI->PR),
          BYTE_TO_BINARY(EXTI->RTSR),
          BYTE_TO_BINARY(EXTI->FTSR)
	);

	switch (e.id) {
		case INPUT_EVENT_POWER_OFF:
			printf("current_state = %s\n", fsm_group->pedal_fsm.current_state->name);
			if (strcmp(fsm_group->pedal_fsm.current_state->name, "state_idle")) {
				printf("Cannot power off while pedals are pressed\n");
				break;
			}
			
			if (strcmp(fsm_group->direction_fsm.current_state->name, "state_neutral")) {
				printf("Cannot power off until in neutral\n");
				break;
			}
				
			transitioned = fsm_process_event(&fsm_group->pedal_fsm, &e);
			break;
		
		case INPUT_EVENT_GAS_PRESSED:
            if (strcmp(fsm_group->direction_fsm.current_state->name, "state_forward") && 
				strcmp(fsm_group->direction_fsm.current_state->name, "state_reverse")) {
				printf("Cannot start moving unless in forward or reverse\n");
				break;
			}
			
			transitioned = fsm_process_event(&fsm_group->pedal_fsm, &e);
			break;
		
		case INPUT_EVENT_DIRECTION_SELECTOR_NEUTRAL:		
		case INPUT_EVENT_DIRECTION_SELECTOR_DRIVE:		
		case INPUT_EVENT_DIRECTION_SELECTOR_REVERSE:
			if (!(strcmp(fsm_group->pedal_fsm.current_state->name, "state_brake"))) {
				transitioned = fsm_process_event(&fsm_group->direction_fsm, &e);
			}
      break;
		
		case INPUT_EVENT_TURN_SIGNAL_NONE:
		case INPUT_EVENT_TURN_SIGNAL_LEFT:
		case INPUT_EVENT_TURN_SIGNAL_RIGHT:
			transitioned = fsm_process_event(&fsm_group->turn_signal_fsm, &e);
			break;
		case INPUT_EVENT_HAZARD_LIGHT_ON:
		case INPUT_EVENT_HAZARD_LIGHT_OFF:
			if (strcmp(fsm_group->pedal_fsm.current_state->name, "state_off")) {
				transitioned = fsm_process_event(&fsm_group->hazard_light_fsm, &e);
			}
			break;	
		default:		
			transitioned = fsm_process_event(&fsm_group->pedal_fsm, &e);
	}
	
	printf("P%c%d : Event = %d  Car Status = %s : Direction = %s : Turn Signal = %s : Hazard Lights = %s \n", 
			(uint8_t)(address->port+65), 
			address->pin, 
			e.id,
			fsm_group->pedal_fsm.current_state->name,
			fsm_group->direction_fsm.current_state->name,
			fsm_group->turn_signal_fsm.current_state->name,
			fsm_group->hazard_light_fsm.current_state->name
	);
	
	return;
}

