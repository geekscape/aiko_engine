/**
 * Please do not remove the following notices.
 *
 * \file       serial_serial.h
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2016 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#pragma once

#define BAUD_RATE        B19200
#define SERIAL_PORT_0    "/dev/ttyUSB0"
#define SERIAL_PORT_1    "/dev/ttyUSB1"

tExpression *serial_serial_initialize(
  char *serial_port_0, char *serial_port_1, uint8_t debug_flag
);
