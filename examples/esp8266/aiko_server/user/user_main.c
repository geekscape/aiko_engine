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
 * (3:cdr(1:a1:b))             // Lisp 1.5 with canonical S-expressions :)
 * (5:debug)                   // toggle lispDebug flag
 * (8:addTimer)                // add timer every 1 second  for a single count
 * (8:addTimer4:2000)          // add timer every 2 seconds for a single count
 * (8:addTimer2:20001:4)       // add timer every 2 seconds for 4 counts
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
#include "aiko_serial.h"

#include "lisp.h"
#include "lisp_extend.h"

#define BAUD_RATE  38400

aiko_store_t aiko_server_store;

/* ------------------------------------------------------------------------- */

void user_rf_pre_init(void) {
}

void ICACHE_FLASH_ATTR
initialize(
  uint8_t debug_flag) {

  uart_init(BAUD_RATE, BAUD_RATE);
  os_delay_us(5000);               // Allow time to settle before using UART :(

  printf("# ---------------\n");
  printf("[aiko_server %s]\n", AIKO_VERSION);
  printf("# SDK version: %s\n", system_get_sdk_version());
  printf("# CPU clock:   %d\n", system_get_cpu_freq());
  printf("# Heap free:   %d\n", system_get_free_heap_size());

  tExpression *lisp_environment = lisp_initialize(debug_flag);

  if (lispError) {
    printf("lisp_initialize(): %s\n", lispErrorMessage);
  }
  else {
    memset(& aiko_server_store, 0x00, sizeof(aiko_server_store));
    aiko_server_store.size = sizeof(aiko_server_store);
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
    aiko_create_serial_stream(NULL, BAUD_NO_CHANGE, '\r'), lisp_message_handler
  );

  aiko_add_handler(
    aiko_create_socket_stream(AIKO_STREAM_SOCKET_UDP4, TRUE, 0, AIKO_PORT),
    lisp_message_handler
  );
}

/* ------------------------------------------------------------------------- */
