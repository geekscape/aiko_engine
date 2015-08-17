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
 *  (3:cdr(1:a1:b))
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#include "user_interface.h"

#include "driver/uart.h"

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

void user_rf_pre_init(void) {
}

void ICACHE_FLASH_ATTR
user_init(void) {
  ets_wdt_disable();
  uart_init(BIT_RATE_38400, BIT_RATE_38400);

  os_delay_us(5000);
  os_printf("# ---------------\n");
  os_printf("# SDK version: %s\n", system_get_sdk_version());
  os_printf("# Heap free: %d\n", system_get_free_heap_size());
  os_printf("# CPU clock: %d\n", system_get_cpu_freq());

  wifi_set_opmode(STATION_MODE);
  system_init_done_cb(system_ready);

  char ssid[32] = SSID;
  char password[64] = SSID_PASSWORD;
  struct station_config station_configuration;

  os_memcpy(& station_configuration.ssid, ssid, 32);
  os_memcpy(& station_configuration.password, password, 64);
  wifi_station_set_config(& station_configuration);

  lisp_initialize();

  aiko_add_handler(
    aiko_create_socket_source(AIKO_SOURCE_SOCKET_UDP4, AIKO_SERVER_PORT),
    lisp_message_handler
  );
}
