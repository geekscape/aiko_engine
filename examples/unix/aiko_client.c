/**
 * Please do not remove the following notices.
 *
 * \file       aiko_client.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * Description
 * ~~~~~~~~~~~
 * Aiko (Lisp) client: Unix implementation
 *
 * Usage
 * ~~~~~
 * ./aiko_client [ hostname ]
 *
 * To Do
 * ~~~~~
 * - Discover local aiko_servers.
 * - Command line argument for input file to send.
 * - Translate Canonical to/from Regular S-Expressions on message send/receive.
 * - Translate JSON to/from S-Expressions on message send/receive.
 *
 * - Add console "readline" like functionality ...
 *   - Incorporate into src/unix/engine.c, AIKO_STREAM_FILE, fgets().
 */

#include <stdio.h>

#include "aiko_engine.h"
#include "aiko_network.h"

aiko_stream_t *aiko_server_stream;

/* ------------------------------------------------------------------------- */

uint8_t console_message_handler(
  void     *aiko_stream,
  uint8_t  *message,
  uint16_t  length) {

  aiko_stream_send(aiko_server_stream, message, length);
  return(AIKO_HANDLED);
}

uint8_t udp_message_handler(
  void     *aiko_stream,
  uint8_t  *message,
  uint16_t  length) {

  if (message[length-1] == 0x0a) length --;
  if (message[length-1] == 0x0d) length --;

  printf("%.*s\n", length, message);
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

  aiko_server_stream = aiko_create_socket_stream(
    AIKO_STREAM_SOCKET_UDP4, FALSE, address_ipv4, AIKO_PORT
  );

  aiko_add_handler(aiko_server_stream, udp_message_handler);
  aiko_add_handler(aiko_create_file_stream(stdin), console_message_handler);

  aiko_loop(AIKO_LOOP_FOREVER);

  return(0);
}

/* ------------------------------------------------------------------------- */
