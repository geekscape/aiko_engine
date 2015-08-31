/**
 * Please do not remove the following notices.
 *
 * \file       aiko_server.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * Description
 * ~~~~~~~~~~~
 * Aiko (Lisp) server: Unix implementation
 *
 * Usage
 * ~~~~~
 * ./aiko_server [ source_file ]
 *
 * nc -u localhost 4149
 * (3:car(1:a1:b))             // Lisp 1.5 with canonical S-expressions :)
 * (5:debug)                   // toggle lispDebug flag
 * (8:addTimer)                // add timer every 1 second  for a single count
 * (8:addTimer4:2000)          // add timer every 2 seconds for a single count
 * (8:addtimer4:2000:1:4)      // add timer every 2 seconds for 4 counts
 * (4:load)                    // load "aikoStore" from persistant storage
 * (4:save)                    // save "aikoStore" to persistent storage
 * (4:wifi(4:ssid8:password))  // set Wi-Fi station SSID and password
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#include <stdio.h>
#include <string.h>

#include "aiko_engine.h"

#include "lisp.h"
#include "../common/aiko_server/lisp_extend.h"

aiko_store_t aiko_server_store;

/* ------------------------------------------------------------------------- */

FILE *initialize(
  int   argc,
  char *argv[]) {

  tExpression *lisp_environment = lisp_initialize(LISP_DEBUG);

  if (lispError) {
    printf("Initialization error: %d\n", lispError);    // TODO: Better message
    exit(-1);
  }

  memset(& aiko_server_store, 0x00, sizeof(aiko_server_store));
  aiko_server_store.size = sizeof(aiko_server_store);

  lisp_extend(lisp_environment, & aiko_server_store);

  FILE *inputFile = stdin;
  if (argc > 1) inputFile = fopen(argv[1], "r");

  return(inputFile);
}

/* ------------------------------------------------------------------------- */

int main(
  int   argc,
  char *argv[]) {

  FILE *input_file = initialize(argc, argv);

  printf("[%s %s]\n", argv[0], AIKO_VERSION);

  aiko_add_handler(aiko_create_file_source(input_file), lisp_message_handler);

  aiko_add_handler(
    aiko_create_socket_source(AIKO_SOURCE_SOCKET_UDP4, AIKO_PORT),
    lisp_message_handler
  );

  aiko_loop(AIKO_LOOP_FOREVER);

  return(0);
}

/* ------------------------------------------------------------------------- */
