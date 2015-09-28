/**
 * Please do not remove the following notices.
 *
 * \file       aiko_serial.h
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#pragma once

#include "aiko_engine.h"

#define AIKO_SERIAL_BUFFER_SIZE  128

#define BAUD_NO_CHANGE  0

int aiko_serial_port_open(
  const char *serial_port_name, speed_t baud_rate, uint8_t record_delimiter);

int aiko_serial_send(
  aiko_stream_t *aiko_stream, uint8_t *buffer, uint16_t size
);
