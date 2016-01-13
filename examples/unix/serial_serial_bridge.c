/**
 * Please do not remove the following notices.
 *
 * \file       serial_serial_bridge.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2016 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * Description
 * ~~~~~~~~~~~
 * Serial-Serial bridge: Unix implementation
 *
 * Usage
 * ~~~~~
 * ./serial_serial_bridge SERIAL_PORT_0 SERIAL_PORT_1
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#include <stdlib.h>

#include "aiko_engine.h"
#include "lisp.h"

#include "../common/serial_serial/serial_serial.h"

/* ------------------------------------------------------------------------- */

void show_help(void) {
  fprintf(stderr, "\nUsage: serial_serial SERIAL_PORT_0 SERIAL_PORT_1\n\n");
}

/* ------------------------------------------------------------------------- */

void initialize(
  int   argc,
  char *argv[]) {

  if (argc != 3) {
    show_help();
    exit(-1);
  }

  tExpression *lisp_environment = serial_serial_initialize(
    argv[1], argv[2], LISP_DEBUG
  );

  if (lispError) {
    printf("initialize(): %s\n", lispErrorMessage);
    exit(-1);
  }

  if (lisp_environment == NULL) {
    printf("serial_serial_initialize(): failed\n");
    exit(-1);
  }
}

/* ------------------------------------------------------------------------- */

int main(
  int   argc,
  char *argv[]) {

  printf("[%s %s]\n", argv[0], AIKO_VERSION);

  initialize(argc, argv);

  aiko_loop(AIKO_LOOP_FOREVER);

  return(0);
}

/* ------------------------------------------------------------------------- */
