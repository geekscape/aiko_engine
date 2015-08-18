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
 * - UDP implementation.
 * - Wi-Fi Soft-AP web-server for configuration ...
 *   - Wi-Fi Station SSID and password.
 *   - UDP port number.
 *   - Serial baud rate, number of bits, number of stop bits, parity.
 *   - Serial receive sample rate, default: 20 milliseconds.
 *   - Serial buffer size, use malloc() / free().
 *   - Serial record delimiter, default: none (send immediately).
 * - Flash storage for configuration parameters.
 */

#include "user_interface.h"

#include "user_config.h"

#include "aiko_engine.h"

/* ------------------------------------------------------------------------- */

uint8_t ICACHE_FLASH_ATTR
serial_handler(
  uint8_t  *message,
  uint16_t  length) {

  int index;

  for (index = 0;  index < length;  index ++) os_printf("%c", message[index]);

  return(AIKO_HANDLED);
}

/* ------------------------------------------------------------------------- */

void user_rf_pre_init(void) {
}

void ICACHE_FLASH_ATTR
user_init(void) {
  aiko_add_handler(
    aiko_create_serial_source(NULL, 38400, 0x00), serial_handler
  );

  os_delay_us(5000);
  os_printf("# ---------------\n");
  os_printf("# SDK version: %s\n", system_get_sdk_version());
  os_printf("# CPU clock:   %d\n", system_get_cpu_freq());
  os_printf("# Heap free:   %d\n", system_get_free_heap_size());
}
