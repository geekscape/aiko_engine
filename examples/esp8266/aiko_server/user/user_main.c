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
 * Usage (on remote machine)
 * ~~~~~
 *  nc -u ESP8266_IP_ADDRESS 4000
 *  (5:debug)
 *  (3:cdr(1:a1:b))
 *  (8:addTimer)
 *
 * To Do
 * ~~~~~
 * - Refactor .../examples/unix/aiko_server.c common code into examples/
 */

#include "user_interface.h"

#include "user_config.h"

#include "aiko_engine.h"
#include "lisp.h"

#define AIKO_SERVER_PORT  4000

/* ------------------------------------------------------------------------- */

void ICACHE_FLASH_ATTR
wifi_ap_scan_done(
  void   *arg,
  STATUS  status) {

  if (status == OK) {
    os_printf("wifi_ap_scan_done():\n");
    struct bss_info *bss_item = (struct bss_info *) arg;

    while (bss_item != NULL) {
      os_printf("  ssid: %s\n", bss_item->ssid);
      bss_item = bss_item->next.stqe_next;
    }
  }
}

void ICACHE_FLASH_ATTR
system_ready(void) {
  if (wifi_get_opmode() != SOFTAP_MODE) {
    wifi_station_scan(NULL, wifi_ap_scan_done);
  }
}

/* ------------------------------------------------------------------------- */

static uint8_t timer_counter = 0;
static uint8_t timer_maximum = 4;

uint8_t ICACHE_FLASH_ATTR
timer_handler(
  void *timer_self) {

  printf("timer_handler(): %d\n", timer_counter ++);

  if (timer_counter == timer_maximum) {
    aiko_delete_timer((aiko_timer_t *) timer_self);
  }

  return(AIKO_HANDLED);
}

tExpression ICACHE_FLASH_ATTR
*primitiveAddTimer(
  tExpression *expression,
  tExpression *environment) {

  timer_counter = 0;
  timer_maximum = 4;            // TODO: Assign value using the first parameter

  aiko_time_t   period = { 1, 0 };  // 1 second
  aiko_timer_t *timer  = aiko_add_timer(& period, timer_handler);

  return(truth);
}

tExpression ICACHE_FLASH_ATTR
*primitiveDebug(
  tExpression *expression,
  tExpression *environment) {

  lispDebug = ! lispDebug;

  return(truth);
}

/* ------------------------------------------------------------------------- */

void ICACHE_FLASH_ATTR
initialize(
  uint8_t debug_flag) {

  tExpression *lisp_environment = lisp_initialize(debug_flag);

  aikoAppend(lisp_environment, aikoCreatePrimitive("debug", primitiveDebug));

  aikoAppend(
    lisp_environment, aikoCreatePrimitive("addTimer", primitiveAddTimer)
  );

// TODO: Ultimately, shouldn't need to do this ...
  aikoExpressionBookmark = aikoExpressionCurrent;
}

/* ------------------------------------------------------------------------- */

void user_rf_pre_init(void) {
}

void ICACHE_FLASH_ATTR
user_init(void) {
  ets_wdt_disable();

  initialize(LISP_DEBUG);

  aiko_add_handler(
    aiko_create_serial_source(NULL, 38400, '\r'), lisp_message_handler
  );

  os_delay_us(5000);
  os_printf("# ---------------\n");
  os_printf("# SDK version: %s\n", system_get_sdk_version());
  os_printf("# CPU clock:   %d\n", system_get_cpu_freq());
  os_printf("# Heap free:   %d\n", system_get_free_heap_size());

  wifi_set_opmode(STATION_MODE);
  system_init_done_cb(system_ready);

  char ssid[32] = SSID;
  char password[64] = SSID_PASSWORD;
  struct station_config station_configuration;

  os_memcpy(& station_configuration.ssid, ssid, 32);
  os_memcpy(& station_configuration.password, password, 64);
  wifi_station_set_config(& station_configuration);

  aiko_add_handler(
    aiko_create_socket_source(AIKO_SOURCE_SOCKET_UDP4, AIKO_SERVER_PORT),
    lisp_message_handler
  );
}
