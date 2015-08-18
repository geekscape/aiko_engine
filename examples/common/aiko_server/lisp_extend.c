/**
 * Please do not remove the following notices.
 *
 * \file       lisp_extend.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * Description
 * ~~~~~~~~~~~
 * Aiko server Lisp extensions.
 *
 * Usage
 * ~~~~~
 *   (5:debug)     // toggle lispDebug flag
 *   (8:addTimer)
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#include "aiko_engine.h"
#include "lisp.h"

#include "lisp_extend.h"

/* ------------------------------------------------------------------------- */

static uint8_t timer_counter = 0;
static uint8_t timer_maximum = 1;

uint8_t ATTRIBUTES
timer_handler(
  void *timer_self) {

  printf("timer_handler(): %d\n", timer_counter ++);

  if (timer_counter == timer_maximum) {
    aiko_delete_timer((aiko_timer_t *) timer_self);
  }

  return(AIKO_HANDLED);
}

tExpression ATTRIBUTES
*primitiveAddTimer(
  tExpression *expression,
  tExpression *environment) {

  timer_counter = 0;
  timer_maximum = 1;

  uint32_t microseconds = 1000000;  // default timer period: 1 second

  if (expression != NULL) {
    tExpression *parameter1 = expression->list.car;   // milliseconds
    tExpression *parameter2 = expression->list.cdr;   // (timer_maximum)

    microseconds = aikoToInteger(parameter1) * 1000;

    if (parameter2 != NULL) {
      timer_maximum = aikoToInteger(parameter2->list.car);
    }
  }

  aiko_time_t   period = { 0, microseconds };
  aiko_timer_t *timer  = aiko_add_timer(& period, timer_handler);

  return(truth);
}

tExpression ATTRIBUTES
*primitiveDebug(
  tExpression *expression,
  tExpression *environment) {

  lispDebug = ! lispDebug;

  return(truth);
}

/* ------------------------------------------------------------------------- */

void ATTRIBUTES
lisp_extend(
  tExpression *lisp_environment) {

  aikoAppend(
    lisp_environment, aikoCreatePrimitive("addTimer", primitiveAddTimer)
  );

  aikoAppend(lisp_environment, aikoCreatePrimitive("debug", primitiveDebug));

// TODO: Ultimately, shouldn't need to do this ...
  aikoExpressionBookmark = aikoExpressionCurrent;
}
