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

#include "user_config.h"

#include "aiko_engine.h"
#include "lisp.h"

#define AIKO_SERVER_PORT  4000

/* ------------------------------------------------------------------------- */

void user_rf_pre_init(void) {
}

void ICACHE_FLASH_ATTR
user_init(void) {
  uart_init(BIT_RATE_38400, BIT_RATE_38400);

  os_printf("# ---------------\n");
  os_printf("# SDK version: %s\n", system_get_sdk_version());
  os_printf("# Heap free: %d\n", system_get_free_heap_size());
  os_printf("# CPU clock: %d\n", system_get_cpu_freq());

  aiko_add_handler(
    aiko_create_socket_source(AIKO_SOURCE_SOCKET_UDP4, AIKO_SERVER_PORT),
    lisp_message_handler
  );
}
