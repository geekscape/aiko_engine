/**
 * Please do not remove the following notices.
 *
 * \file       aiko_tcp.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * Description
 * ~~~~~~~~~~~
 * TCP example: Unix implementation
 *
 * Usage
 * ~~~~~
 * nc -l HOSTNAME 4149
 *   Test messsage 0
 *
 * ./aiko_tcp HOSTNAME
 *   Test messsage 1
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#include <netdb.h>
#include <stdio.h>

#include "aiko_engine.h"
#include "aiko_network.h"

aiko_stream_t *socket_stream;

/* ------------------------------------------------------------------------- */

uint8_t console_message_handler(
  void     *aiko_stream,
  uint8_t  *message,
  uint16_t  length) {

  int result = aiko_stream_send(socket_stream, message, length);
  return(AIKO_HANDLED);
}

uint8_t tcp_message_handler(
  void     *aiko_stream,
  uint8_t  *message,
  uint16_t  length) {

  printf("tcp_message_handler(): %d:%s\n", length, message);
  return(AIKO_HANDLED);
}

/* ------------------------------------------------------------------------- */

int main(
  int   argc,
  char *argv[]) {

  printf("[%s %s]\n", argv[0], AIKO_VERSION);

  char *hostname = "127.0.0.1";
  if (argc == 2) hostname = argv[1];

  uint32_t address_ipv4 = aiko_get_ip_address(hostname);

  socket_stream = aiko_create_socket_stream(
    AIKO_STREAM_SOCKET_TCP4, address_ipv4, AIKO_PORT
  );
  printf("Connected to port %d\n", AIKO_PORT);

  aiko_add_handler(aiko_create_file_stream(stdin), console_message_handler);
  aiko_add_handler(socket_stream, tcp_message_handler);

  aiko_loop(AIKO_LOOP_FOREVER);

  return(0);
}

/* ------------------------------------------------------------------------- */
