/**
 * Please do not remove the following notices.
 *
 * \file       serial_udp.h
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#pragma once

#define BAUD_RATE        B57600
#define SERIAL_PORT      "/dev/ttyUSB0"
#define UDP_SERVER_PORT  1025

tExpression *serial_udp_initialize(
  char *serial_port, uint16_t udp_port, uint8_t debug_flag
);
