/**
 * Please do not remove the following notices.
 *
 * \file       aiko_state_machine.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * Description
 * ~~~~~~~~~~~
 * Aiko state machine example: Unix implementation
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#include "aiko_engine.h"
#include "aiko_network.h"
#include "aiko_state.h"

typedef enum {
  STATE_WAIT_FOR_1 = AIKO_STATE_USER_DEFINED,
  STATE_WAIT_FOR_2
}
  state_type;

/* ------------------------------------------------------------------------- */

aiko_state_type action_error(
  aiko_stream_t *aiko_stream,
  tExpression   *expression) {

  printf("Error: Retry message maximum limit\n");
  exit(0);
}

aiko_state_type action_state_1(
  aiko_stream_t *aiko_stream,
  tExpression   *expression) {

  printf("action_state_1(): Send 'state_1' and wait for response\n");

  char *message = "(5:quote7:state_1)\n";

  aiko_socket_send_broadcast(aiko_stream, message, strlen(message));

  return(STATE_WAIT_FOR_1);
}

aiko_state_type action_state_2(
  aiko_stream_t *aiko_stream,
  tExpression   *expression) {

  printf("action_state_2(): Send 'state_2' and wait for response\n");

  char *message = "(5:quote7:state_2)\n";

  aiko_socket_send_broadcast(aiko_stream, message, strlen(message));

  return(STATE_WAIT_FOR_2);
}

aiko_state_type action_state_3(
  aiko_stream_t *aiko_stream,
  tExpression   *expression) {

  printf("action_state_3(): Success\n");
  exit(0);
}

/* ------------------------------------------------------------------------- */

aiko_state_t states[] = {
  { AIKO_STATE_ERROR, AIKO_MATCH_ANY,          NULL,     action_error   },
  { STATE_WAIT_FOR_1, AIKO_MATCH_ATOM_SYMBOL, "state_1", action_state_2 },
  { STATE_WAIT_FOR_2, AIKO_MATCH_ATOM_SYMBOL, "state_2", action_state_3 }
};

uint8_t states_count = sizeof(states) / sizeof(aiko_state_t);

/* ------------------------------------------------------------------------- */

int main(
  int   argc,
  char *argv[]) {

  printf("[%s %s]\n", argv[0], AIKO_VERSION);

  tExpression *lisp_environment = lisp_initialize(LISP_DEBUG);

  if (lispError) {
    printf("lisp_initialize(): %s\n", lispErrorMessage);
    exit(-1);
  }

  aiko_stream_t *aiko_stream = aiko_create_socket_stream(
    AIKO_STREAM_SOCKET_UDP4, TRUE, 0, AIKO_PORT
  );

  aiko_state_machine(states, states_count, aiko_stream, action_state_1);

  aiko_loop(AIKO_LOOP_FOREVER);

  return(0);
}

/* ------------------------------------------------------------------------- */
