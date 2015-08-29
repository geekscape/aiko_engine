/*
 * Please do not remove the following notices.
 *
 * \file       user_main.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * Description
 * ~~~~~~~~~~~
 * Aiko engine: ESP8266 implementation
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#include "user_interface.h"

#include "driver/uart.h"

#include "aiko_engine.h"

/* ------------------------------------------------------------------------- */

static uint8_t timer_counter = 0;

uint8_t ICACHE_FLASH_ATTR
timer_handler(
  void *timer_self) {

  os_printf("timer_handler(): %d\n", timer_counter ++);

  if (timer_counter == 4) aiko_delete_timer((aiko_timer_t *) timer_self);
  return(AIKO_HANDLED);
}

/* ------------------------------------------------------------------------- */

void user_rf_pre_init(void) {
}

void ICACHE_FLASH_ATTR
user_init(void) {
  uart_init(BIT_RATE_38400, BIT_RATE_38400);

  os_printf("# ---------------\n");
  os_printf("# SDK version: %s\n", system_get_sdk_version());
  os_printf("# CPU clock:   %d\n", system_get_cpu_freq());
  os_printf("# Heap free:   %d\n", system_get_free_heap_size());

  aiko_time_t   period = { 1, 0 };  // 1 second
  aiko_timer_t *timer  = aiko_add_timer(& period, timer_handler);
}
