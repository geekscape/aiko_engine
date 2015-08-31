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

#define UDP_PORT0  (AIKO_PORT)
#define UDP_PORT1  (AIKO_PORT + 1)

/* ------------------------------------------------------------------------- */

uint8_t ICACHE_FLASH_ATTR
udp_message_handler0(
  uint8_t  *message,
  uint16_t  length) {

  printf("udp_message_handler0(): %d:%s\n", length, message);
  return(AIKO_HANDLED);
}

uint8_t ICACHE_FLASH_ATTR
udp_message_handler1(
  uint8_t  *message,
  uint16_t  length) {

  printf("udp_message_handler1(): %d:%s\n", length, message);
  return(AIKO_HANDLED);
}

/* ------------------------------------------------------------------------- */

void user_rf_pre_init(void) {
}

void ICACHE_FLASH_ATTR
user_init(void) {
  uart_init(BIT_RATE_38400, BIT_RATE_38400);

  printf("# ---------------\n");
  printf("[aiko_udp %s]\n", AIKO_VERSION);
  printf("# SDK version: %s\n", system_get_sdk_version());
  printf("# CPU clock:   %d\n", system_get_cpu_freq());
  printf("# Heap free:   %d\n", system_get_free_heap_size());

  aiko_add_handler(
    aiko_create_socket_source(AIKO_SOURCE_SOCKET_UDP4, UDP_PORT0),
    udp_message_handler0
  );
  printf("Listening on port %d\n", UDP_PORT0);

  aiko_add_handler(
    aiko_create_socket_source(AIKO_SOURCE_SOCKET_UDP4, UDP_PORT1),
    udp_message_handler1
  );
  printf("Listening on port %d\n", UDP_PORT1);
}
