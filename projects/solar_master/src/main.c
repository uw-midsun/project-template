// Placeholder code for Solar Master

#include <stdint.h>
#include <stdio.h>

#include "can_transmit.h"   // For creating and sending CAN message
#include "delay.h"          // For real-time delays

// Number of slaves to iterate through
#define NUM_SLAVES 6

typedef enum {
  SLAVE_0_REQUEST = 0,
  SLAVE_1_REQUEST,
  SLAVE_2_REQUEST,
  SLAVE_3_REQUEST,
  SLAVE_4_REQUEST,
  SLAVE_5_REQUEST,
  SLAVE_0_RESPONSE,
  SLAVE_1_RESPONSE,
  SLAVE_2_RESPONSE,
  SLAVE_3_RESPONSE,
  SLAVE_4_RESPONSE,
  SLAVE_5_RESPONSE,
  NUM_SLAVE_MESSAGES, } SlaveMessages;

// Sends LIN message to the solar slave with the corresponding request id
void slave_send_lin_message(SlaveMessages slave_request_id, uint8_t *arr,
                       size_t arr_size) {
  printf("Send request for slave #%d \n", slave_request_id);
}

// The "template" or design to the callback function
typedef int (*slave_callback)(SlaveMessages message_id, uint8_t *data,
                              size_t data_length, void *context);

void slave_register_callback(SlaveMessages slave_response_id, slave_callback callback,
                             void *context);

// An application of *slave_callback
static int prv_process_message(SlaveMessages slave_response_id, uint8_t *data, size_t data_length,
                               void *context) {
 // Extract data
  uint8_t slave_board_id = data[0]; // uint8_t instead of uint16_t because there are not many ids
  uint16_t voltage = data[1] << 8 | data[2];
  uint16_t current = data[3] << 8 | data[3];
  uint16_t temperature = data[4] << 8 | data[5];

  // Output data
  printf("Received: Response ID (%d)\n", slave_response_id);
  printf("          Slave ID (%d)\n", slave_board_id);
  printf("          Voltage (%d)\n", voltage);
  printf("          Current (%d)\n", current);
  printf("          Temperature (%d)\n", temperature);

  // Transmite data over CAN
  CAN_TRANSMIT_SOLAR_DATA_FRONT(slave_id, current, voltage, temperature);
}

int main(void) {

  // Initialize callback functions for each valid slave response
  for (SlaveMessages slave_num_response = slave_0_response;
  slave_num_response < NUM_SLAVE_MESSAGES; slave_num_response++) {
    slave_register_callback(slave_num_response, prv_process_message, NULL);
  }

  while (1) {
    // Loop through all slaves
    for (SlaveMessages slave_num_request = slave_0_request; slave_num_request < NUM_SLAVES;
     slave_num_request++) {
      slave_send_lin_message(slave_num_request, NULL, 0); // send request message to each slave
    }

    delay_s(10);
  }

  return 0;
}
