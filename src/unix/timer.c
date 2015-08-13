/**
 * Please do not remove the following notices.
 *
 * \file       timer.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * Description
 * ~~~~~~~~~~~
 * Timers: Unix implementation
 *
 * Intended for a small number of timers, optimized for minimal memory usage.
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#include <assert.h>
#include <sys/time.h>

#include "aiko_engine.h"

aiko_timer_t *aiko_timers[AIKO_TIMER_MAXIMUM];
aiko_timer_t *aiko_timer_next  = NULL;
uint8_t       aiko_timer_count = 0;

void aiko_timer_dump(
  char *label) {

  printf("%s ----------\n", label);

  for (int index = 0;  index < aiko_timer_count;  index ++) {
    aiko_timer_t *aiko_timer = aiko_timers[index];

    printf("aiko_timer[%d]: %d.%06d %c\n", index,
      aiko_timer->period.seconds, aiko_timer->period.microseconds,
      aiko_timer == aiko_timer_next  ?  '*'  :  ' ');
  }
}

void aiko_timer_next_update(void) {
  assert(aiko_timer_count > 0);

  aiko_timer_next = aiko_timers[0];

  for (int index = 1;  index < aiko_timer_count;  index ++) {
    if (aiko_timers[index]->timeout < aiko_timer_next->timeout) {
      aiko_timer_next = aiko_timers[index];
    }
  }
}

void aiko_timer_update(
  aiko_timer_t *aiko_timer) {

  for (int i = 0;  i < aiko_timer_count;  i ++) {
    if (aiko_timers[i] == aiko_timer) {        // validate aiko_timer reference
      struct timeval time_value;
      gettimeofday(& time_value, NULL);

      aiko_timer->timeout =
        (time_value.tv_sec  + aiko_timer->period.seconds) * 1000000 +
         time_value.tv_usec + aiko_timer->period.microseconds;

      aiko_timer_next_update();
    }
  }
}

aiko_timer_t *aiko_add_timer(
  aiko_time_t          *period,
  aiko_timer_handler_t *handler) {

  assert(aiko_timer_count < AIKO_TIMER_MAXIMUM);

  aiko_timer_t *aiko_timer = (aiko_timer_t *) malloc(sizeof(aiko_timer_t));
  aiko_timers[aiko_timer_count ++] = aiko_timer;

  aiko_timer->period.seconds      = period->seconds;
  aiko_timer->period.microseconds = period->microseconds;
  aiko_timer->handler             = handler;

  aiko_timer_update(aiko_timer);
  return(aiko_timer);
}

void aiko_delete_timer(
  aiko_timer_t *aiko_timer) {

  for (int i = 0;  i < aiko_timer_count;  i ++) {
    if (aiko_timers[i] == aiko_timer) {        // validate aiko_timer reference
      free(aiko_timer);

      while ((i + 1) < aiko_timer_count) {
        aiko_timers[i] = aiko_timers[i + 1];
        i ++;
      }

      aiko_timers[-- aiko_timer_count] = NULL;

      if (aiko_timer_count > 0) aiko_timer_next_update();
      break;
    }
  }
}

aiko_timer_t *aiko_next_timer(void) {
  assert(aiko_timer_count > 0);
  return(aiko_timer_next);
}
