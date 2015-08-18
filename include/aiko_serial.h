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

#include "aiko_compatibility.h"

#define AIKO_SERIAL_BUFFER_SIZE 128

int serial_port_open(
  const char *serial_port_name, speed_t baud_rate, uint8_t record_delimiter);
