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

static uint8_t timer_counter = 0;
static uint8_t timer_maximum = 4;

uint8_t timer_handler(
  void *timer_self) {

  printf("timer_handler(): %d\n", timer_counter ++);

  if (timer_counter == timer_maximum) {
    aiko_delete_timer((aiko_timer_t *) timer_self);
  }

  return(AIKO_HANDLED);
}

tExpression *primitiveAddTimer(
  tExpression *expression,
  tExpression *environment) {

  timer_counter = 0;
  timer_maximum = 4;            // TODO: Assign value using the first parameter

  aiko_time_t   period = { 1, 0 };  // 1 second
  aiko_timer_t *timer  = aiko_add_timer(& period, timer_handler);

  return(truth);
}

tExpression *primitiveDebug(
  tExpression *expression,
  tExpression *environment) {

  lispDebug = ! lispDebug;

  return(truth);
}

/* ------------------------------------------------------------------------- */

FILE *initialize(
  int   argc,
  char *argv[]) {

  tExpression *lisp_environment = lisp_initialize(LISP_DEBUG);

  if (aikoError) {
    printf("Initialization error: %d\n", aikoError);    // TODO: Better message
    exit(-1);
  }

  aikoAppend(lisp_environment, aikoCreatePrimitive("debug", primitiveDebug));

  aikoAppend(
    lisp_environment, aikoCreatePrimitive("addTimer", primitiveAddTimer)
  );

// TODO: Ultimately, shouldn't need to do this ...
  aikoExpressionBookmark = aikoExpressionCurrent;

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

  aiko_loop(AIKO_LOOP_FOREVER);

  return(0);
}

/* ------------------------------------------------------------------------- */
