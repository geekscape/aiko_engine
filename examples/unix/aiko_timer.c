/**
 * Please do not remove the following notices.
 *
 * \file       aiko_timer.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * Description
 * ~~~~~~~~~~~
 * Timer example: Unix implementation
 *
 * To Do
 * ~~~~~
 * - Command line arguments to change "period" and "maximum count".
 */

#include <stdio.h>

#include "aiko_engine.h"

/* ------------------------------------------------------------------------- */

static uint8_t timer_counter = 0;

uint8_t timer_handler(
  void *timer_self) {

  printf("timer_handler(): %d\n", timer_counter ++);

  if (timer_counter == 4) aiko_delete_timer((aiko_timer_t *) timer_self);
  return(AIKO_HANDLED);
}

/* ------------------------------------------------------------------------- */

int main(
  int   argc,
  char *argv[]) {

  printf("[%s %s]\n", argv[0], AIKO_VERSION);

  aiko_time_t   period = { 1, 0 };  // 1 second
  aiko_timer_t *timer  = aiko_add_timer(& period, timer_handler);

  aiko_loop(AIKO_LOOP_FOREVER);

  return(0);
}

/* ------------------------------------------------------------------------- */
