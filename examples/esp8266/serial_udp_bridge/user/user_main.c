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
 * Serial-UDP bridge: ESP8266 implementation
 *
 * To Do
 * ~~~~~
 * - Wi-Fi Soft-AP web-server for configuration ...
 *   - Wi-Fi Station SSID and password.
 *   - UDP port number, default 1025.
 *   - Serial baud rate, number of bits, number of stop bits, parity.
 *   - Serial receive sample rate, default: 20 milliseconds.
 *   - Serial buffer size, use malloc() / free().
 *   - Serial record delimiter, default: none (send immediately).
 * - Flash storage for configuration parameters.
 */

#include "user_interface.h"

#include "aiko_engine.h"
#include "lisp.h"

#include "serial_udp.h"

/* ------------------------------------------------------------------------- */

void user_rf_pre_init(void) {
}

void ICACHE_FLASH_ATTR
initialize(
  uint16_t udp_port,
  uint8_t  debug_flag) {

  uart_init(BAUD_RATE, BAUD_RATE);
  os_delay_us(5000);               // Allow time to settle before using UART :(
#if 0
  printf("# ---------------\n");
  printf("[serial_udp_bridge %s]\n", AIKO_VERSION);
  printf("# SDK version: %s\n", system_get_sdk_version());
  printf("# CPU clock:   %d\n", system_get_cpu_freq());
  printf("# Heap free:   %d\n", system_get_free_heap_size());
#endif
  tExpression *lisp_environment = serial_udp_initialize(
    NULL, udp_port, debug_flag
  );

  aiko_wifi_softap_configure();

  system_init_done_cb(aiko_system_ready);
}

/* ------------------------------------------------------------------------- */

void ICACHE_FLASH_ATTR
user_init(void) {
  ets_wdt_disable();                                   // TODO: Reinstate WDT ?

  initialize(UDP_SERVER_PORT, LISP_DEBUG);
}

/* ------------------------------------------------------------------------- */
