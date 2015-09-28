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
#include "aiko_serial.h"

#include "lisp.h"
#include "lisp_extend.h"

#define BAUD_RATE        38400
#define UDP_SERVER_PORT   3000

aiko_store_t aiko_server_store;

/* ------------------------------------------------------------------------- */

uint8_t ICACHE_FLASH_ATTR
serial_handler(
  void     *aiko_stream,
  uint8_t  *message,
  uint16_t  length) {

  int index;

  for (index = 0;  index < length;  index ++) os_printf("%c", message[index]);

  return(AIKO_HANDLED);
}

uint8_t ICACHE_FLASH_ATTR
udp_handler(
  void     *aiko_stream,
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
initialize(
  uint8_t debug_flag) {

  uart_init(BAUD_RATE, BAUD_RATE);
  os_delay_us(5000);               // Allow time to settle before using UART :(

  printf("# ---------------\n");
  printf("[serial_udp_bridge %s]\n", AIKO_VERSION);
  printf("# SDK version: %s\n", system_get_sdk_version());
  printf("# CPU clock:   %d\n", system_get_cpu_freq());
  printf("# Heap free:   %d\n", system_get_free_heap_size());

  tExpression *lisp_environment = lisp_initialize(debug_flag);

  if (lispError) {
    printf("lisp_initialize(): %s\n", lispErrorMessage);
  }
  else {
    memset(& aiko_server_store, 0x00, sizeof(aiko_server_store));
    aiko_server_store.size    = sizeof(aiko_server_store);
    aiko_server_store.magic   = AIKO_STORE_MAGIC;
    aiko_server_store.version = AIKO_STORE_VERSION;

    lisp_extend(lisp_environment, & aiko_server_store);
    if (lispError) printf("lisp_extend(): %s\n", lispErrorMessage);
  }

  aiko_wifi_softap_configure();

  system_init_done_cb(aiko_system_ready);
}

/* ------------------------------------------------------------------------- */

void ICACHE_FLASH_ATTR
user_init(void) {
  ets_wdt_disable();                                   // TODO: Reinstate WDT ?

  initialize(LISP_DEBUG);

  aiko_add_handler(
    aiko_create_serial_stream(NULL, BAUD_NO_CHANGE, 0x00), serial_handler
  );

  aiko_add_handler(
    aiko_create_socket_stream(
      AIKO_STREAM_SOCKET_UDP4, TRUE, 0, UDP_SERVER_PORT
    ),
    udp_handler
  );
}

/* ------------------------------------------------------------------------- */
