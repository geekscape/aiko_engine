/**
 * Please do not remove the following notices.
 *
 * \file       aiko_engine.h
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#pragma once

#include "aiko_compatibility.h"

#define AIKO_SOURCE_MAXIMUM    2
#define AIKO_TIMER_MAXIMUM     2
#define MESSAGE_BUFFER_SIZE  256

#define AIKO_LOOP_FOREVER      0
#define AIKO_HANDLED           1
#define AIKO_NOT_HANDLED       0

typedef uint8_t (aiko_handler_t)(uint8_t *message, uint16_t length);
typedef uint8_t (aiko_timer_handler_t)(void *aiko_timer);

typedef enum {
  AIKO_SOURCE_FILE,
  AIKO_SOURCE_SERIAL,
  AIKO_SOURCE_SOCKET_UDP4,
  AIKO_SOURCE_USER_DEFINED = 64
}
  aiko_source_type;

typedef struct {
  aiko_source_type  type;
  aiko_handler_t   *handler;
  int               fd;
}
  aiko_source_t;

typedef struct {
  uint32_t seconds;
  uint32_t microseconds;
}
  aiko_time_t;

typedef struct {
#ifdef __ets__
  os_timer_t            esp8266_timer;
#else
  aiko_time_t           period;
  aiko_timer_handler_t *handler;
  long long             timeout;
#endif
}
  aiko_timer_t;

extern uint8_t aiko_timer_count;

aiko_timer_t *aiko_add_timer(
  aiko_time_t *period, aiko_timer_handler_t *handler);

void aiko_delete_timer(aiko_timer_t *aiko_timer);

aiko_timer_t *aiko_next_timer(void);

void aiko_timer_update(aiko_timer_t *aiko_timer);

void aiko_add_handler(aiko_source_t *source, aiko_handler_t *handler);

aiko_source_t *aiko_create_source(aiko_source_type type, int fd);

#ifndef __ets__
aiko_source_t *aiko_create_file_source(FILE *file);
#endif

aiko_source_t *aiko_create_serial_source(
  const char *serial_port_name, speed_t baud_rate, uint8_t record_delimiter);

aiko_source_t *aiko_create_socket_source(
  aiko_source_type type, uint16_t port);

void aiko_loop(uint16_t loop_limit);

void aiko_buffer_dump(const char *label, uint8_t *buffer, uint16_t length);
