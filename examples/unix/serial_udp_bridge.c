/**
 * Please do not remove the following notices.
 *
 * \file       serial_udp_bridge.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * Description
 * ~~~~~~~~~~~
 * Serial-UDP bridge: Unix implementation
 *
 * Usage
 * ~~~~~
 * ./serial_udp_bridge [-p SERIAL_PORT] [-u UDP_PORT]
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#include <getopt.h>
#include <stdlib.h>

#include "aiko_engine.h"
#include "lisp.h"

#include "../common/serial_udp/serial_udp.h"

/* ------------------------------------------------------------------------- */

void show_help(void) {
  fprintf(stderr, "\nUsage: serial_udp_bridge [-p serial_port] [-u udp_port]\n\n");
}

/* ------------------------------------------------------------------------- */

void *initialize(
  int   argc,
  char *argv[]) {

  extern char *optarg;
  extern int   optind;
  int          option;

  char     *serial_port = SERIAL_PORT;
  uint16_t  udp_port    = UDP_SERVER_PORT;

  while ((option = getopt(argc, argv, "hp:u:")) != -1) {
    switch (option) {
      case 'p':
        serial_port = optarg;
        break;

      case 'u':
        udp_port = atoi(optarg);
        break;

      case 'h':
      default:
        show_help();
        exit(-1);
    }
  }

  tExpression *lisp_environment = serial_udp_initialize(
    serial_port, udp_port, LISP_DEBUG
  );

  if (lispError) {
    printf("initialize(): %s\n", lispErrorMessage);
    exit(-1);
  }

  if (lisp_environment == NULL) {
    printf("serial_udp_initialize(): failed\n");
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
