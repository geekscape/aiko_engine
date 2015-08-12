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
 * Main Aiko (Lisp) server: Unix implementation
 *
 * Usage
 * ~~~~~
 *   ./aiko_server [ source_file ]
 *
 *   nc -u localhost 4000
 *   (3:car(1:a1:b))
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#include <stdio.h>

#include "aiko_engine.h"
#include "lisp.h"

#define AIKO_SERVER_PORT  4000

/* ------------------------------------------------------------------------- */

static FILE *initialize(
  int   argc,
  char *argv[]) {

  uint8_t error = lisp_initialize();

  if (error) {
    printf("Initialization error: %d\n", error);  // TODO: Better error message
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
    aiko_create_socket_source(AIKO_SOURCE_SOCKET_UDP4, AIKO_SERVER_PORT),
    lisp_message_handler
  );

  aiko_loop();

  return(0);
}

/* ------------------------------------------------------------------------- */
