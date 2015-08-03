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

#include "compatibility.h"

#define AIKO_SOURCE_MAXIMUM    4
#define MESSAGE_BUFFER_SIZE  256

typedef uint8_t (aiko_handler_t)(uint8_t *message, uint16_t length);

typedef enum {
  AIKO_SOURCE_FILE,
  AIKO_SOURCE_SERIAL,
  AIKO_SOURCE_SOCKET_UDP4,
}
  aiko_source_type;

typedef struct {
  aiko_source_type  type;
  aiko_handler_t   *handler;
  int               fd;
}
  aiko_source_t;

void aiko_add_handler(aiko_source_t *source, aiko_handler_t *handler);

aiko_source_t *aiko_create_source(aiko_source_type type, int fd);

#ifndef __ets__
aiko_source_t *aiko_create_file_source(FILE *file);
#endif

aiko_source_t *aiko_create_serial_source(
  const char *serial_port_name, speed_t baud_rate);

aiko_source_t *aiko_create_socket_source(
  aiko_source_type type, uint16_t port);

void aiko_loop(void);

void dump_buffer(const char *label, uint8_t *buffer, uint16_t length);
