#include "generic_can.h"

#include <stdint.h>

#include "generic_can_msg.h"
#include "status.h"

StatusCode generic_can_tx(const GenericCan *can, const GenericCanMsg *msg) {
  return can->interface->tx(can, msg);
}

StatusCode generic_can_register_rx(GenericCan *can, GenericCanRx rx_handler, uint32_t raw_id,
                                   void *context) {
  return can->interface->register_rx(can, rx_handler, raw_id, context);
}

StatusCode generic_can_enable_rx(GenericCan *can, uint32_t raw_id) {
  return can->interface->enable_rx(can, raw_id);
}

StatusCode generic_can_disable_rx(GenericCan *can, uint32_t raw_id) {
  return can->interface->disable_rx(can, raw_id);
}
