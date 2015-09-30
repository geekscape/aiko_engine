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
 * Timers: ESP8266 implementation
 *
 * To Do
 * ~~~~~
 * - Optionally use microseconds timer, i.e os_timer_arm_us().
 * - Are timer periods longer than 49.7 days required ?
 */

#include "aiko_engine.h"

#define ONCE    0
#define REPEAT  1

aiko_timer_t *aiko_timers[AIKO_TIMER_MAXIMUM];
uint8_t       aiko_timer_count = 0;

aiko_timer_t *aiko_add_timer(
  aiko_time_t          *period,
  aiko_timer_handler_t *handler) {

  if (aiko_timer_count >= AIKO_TIMER_MAXIMUM) {
    printf("Error: Aiko timer limit\n");
    return(NULL);
  }

  uint32_t milliseconds = period->seconds * 1000 + period->microseconds / 1000;

  aiko_timer_t *aiko_timer = (aiko_timer_t *) malloc(sizeof(aiko_timer_t));
  aiko_timers[aiko_timer_count ++] = aiko_timer;

  os_timer_disarm(& aiko_timer->esp8266_timer);
  os_timer_setfn(& aiko_timer->esp8266_timer, handler, aiko_timer);
  os_timer_arm(& aiko_timer->esp8266_timer, milliseconds, REPEAT);

  return(aiko_timer);
}

void aiko_delete_timer(
  aiko_timer_t *aiko_timer) {

  int index;
  for (index = 0;  index < aiko_timer_count;  index ++) {
    if (aiko_timers[index] == aiko_timer) {    // validate aiko_timer reference
      os_timer_disarm(& aiko_timer->esp8266_timer);
      free(aiko_timer);

      while ((index + 1) < aiko_timer_count) {
        aiko_timers[index] = aiko_timers[index + 1];
        index ++;
      }

      aiko_timers[-- aiko_timer_count] = NULL;
      break;
    }
  }
}
