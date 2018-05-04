#include "x86_cmd.h"
#include <stddef.h>
#include <string.h>
#include "log.h"

static void prv_socket_handler(struct X86SocketThread *thread, int client_fd, const char *rx_data,
                               size_t rx_len, void *context) {
  X86CmdThread *cmd_thread = context;

  LOG_DEBUG("Received command %s\n", rx_data);

  // Data will be mangled by strtok - make sure we always end with a null terminator
  char buf[rx_len + 1];
  buf[rx_len] = '\0';

  const char *args[X86_CMD_MAX_ARGS] = { 0 };
  strncpy(buf, rx_data, rx_len);

  // Don't support more than one command per packet
  const char *cmd_word = strtok(buf, " \n");
  size_t num_args = 0;
  while (num_args < X86_CMD_MAX_ARGS && (args[num_args] = strtok(NULL, " \n")) != NULL) {
    num_args++;
  }

  for (size_t i = 0; i < cmd_thread->num_handlers; i++) {
    if (strcmp(cmd_thread->handlers[i].cmd, cmd_word) == 0) {
      cmd_thread->handlers[i].fn(client_fd, cmd_word, args, num_args, cmd_thread->handlers[i].context);
      break;
    }
  }
}

StatusCode x86_cmd_init(X86CmdThread *thread) {
  memset(thread, 0, sizeof(*thread));
  return x86_socket_init(&thread->socket, X86_CMD_SOCKET_NAME, prv_socket_handler, thread);
}

StatusCode x86_cmd_register_handler(X86CmdThread *thread, const char *cmd, X86CmdHandlerFn fn, void *context) {
  if (thread->num_handlers >= X86_CMD_MAX_HANDLERS) {
    return status_code(STATUS_CODE_RESOURCE_EXHAUSTED);
  }

  thread->handlers[thread->num_handlers] = (X86CmdHandler) {
    .cmd = cmd,
    .fn = fn,
    .context = context
  };
  thread->num_handlers++;

  return STATUS_CODE_OK;
}
