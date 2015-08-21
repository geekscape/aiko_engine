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
 * (3:car(1:a1:b))
 * (5:debug)               // toggle lispDebug flag
 * (8:addTimer)            // add timer every 1 second  for a single count
 * (8:addTimer4:2000)      // add timer every 2 seconds for a single count
 * (8:addtimer4:2000:1:4)  // add timer every 2 seconds for 4 counts
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#include <stdio.h>

#include "aiko_engine.h"
#include "lisp.h"

#include "../common/aiko_server/lisp_extend.h"

/* ------------------------------------------------------------------------- */

FILE *initialize(
  int   argc,
  char *argv[]) {

  tExpression *lisp_environment = lisp_initialize(LISP_DEBUG);

  lisp_extend(lisp_environment);

  if (aikoError) {
    printf("Initialization error: %d\n", aikoError);    // TODO: Better message
    exit(-1);
  }

  FILE *inputFile = stdin;
  if (argc > 1) inputFile = fopen(argv[1], "r");

  return(inputFile);
}

/* ------------------------------------------------------------------------- */

int main(
  int   argc,
  char *argv[]) {

  FILE *input_file = initialize(argc, argv);

  aiko_add_handler(aiko_create_file_source(input_file), lisp_message_handler);

  aiko_add_handler(
    aiko_create_socket_source(AIKO_SOURCE_SOCKET_UDP4, AIKO_PORT),
    lisp_message_handler
  );

  aiko_loop(AIKO_LOOP_FOREVER);

  return(0);
}

/* ------------------------------------------------------------------------- */
