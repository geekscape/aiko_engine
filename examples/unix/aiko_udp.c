/**
 * Please do not remove the following notices.
 *
 * \file       aiko_udp.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * Description
 * ~~~~~~~~~~~
 * UDP example: Unix implementation
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#include <stdio.h>

#include "aiko_engine.h"

#define UDP_PORT0  (AIKO_PORT)
#define UDP_PORT1  (AIKO_PORT + 1)

/* ------------------------------------------------------------------------- */

uint8_t udp_message_handler0(
  uint8_t  *message,
  uint16_t  length) {

  printf("udp_message_handler0(): %d:%s\n", length, message);
  return(AIKO_HANDLED);
}

uint8_t udp_message_handler1(
  uint8_t  *message,
  uint16_t  length) {

  printf("udp_message_handler1(): %d:%s\n", length, message);
  return(AIKO_HANDLED);
}

/* ------------------------------------------------------------------------- */

int main(
  int   argc,
  char *argv[]) {

  aiko_add_handler(
    aiko_create_socket_source(AIKO_SOURCE_SOCKET_UDP4, UDP_PORT0),
    udp_message_handler0
  );
  printf("Listening on port %d\n", UDP_PORT0);

  aiko_add_handler(
    aiko_create_socket_source(AIKO_SOURCE_SOCKET_UDP4, UDP_PORT1),
    udp_message_handler1
  );
  printf("Listening on port %d\n", UDP_PORT1);

  aiko_loop(AIKO_LOOP_FOREVER);

  return(0);
}

/* ------------------------------------------------------------------------- */
