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
 * Usage
 * ~~~~~
 * nc -u ESP8266_IP_ADDRESS 4149
 * (3:cdr(1:a1:b))
 * (5:debug)                   // toggle lispDebug flag
 * (8:addTimer)                // add timer every 1 second  for a single count
 * (8:addTimer4:2000)          // add timer every 2 seconds for a single count
 * (8:addtimer4:2000:1:4)      // add timer every 2 seconds for 4 counts
 * (4:load)                    // load "aikoStore" from persistant storage
 * (4:save)                    // save "aikoStore" to persistent storage
 * (4:wifi(4:ssid8:password))  // set Wi-Fi station SSID and password
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#include "user_interface.h"

#include "aiko_engine.h"
#include "lisp.h"

#include "lisp_extend.h"

aiko_store_t aiko_store;

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

void ICACHE_FLASH_ATTR
initialize(
  uint8_t debug_flag) {

  tExpression *lisp_environment = lisp_initialize(debug_flag);

  aiko_store.size = sizeof(aiko_store);
  lisp_extend(lisp_environment, & aiko_store);
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

  wifi_set_opmode(STATIONAP_MODE);
  system_init_done_cb(system_ready);
#if 0
  char ssid[32] = SSID;
  char password[64] = SSID_PASSWORD;
  struct station_config station_configuration;

  os_memcpy(& station_configuration.ssid, ssid, 32);
  os_memcpy(& station_configuration.password, password, 64);
  wifi_station_set_config(& station_configuration);
#endif
  aiko_add_handler(
    aiko_create_socket_source(AIKO_SOURCE_SOCKET_UDP4, AIKO_PORT),
    lisp_message_handler
  );
}

/* ------------------------------------------------------------------------- */
