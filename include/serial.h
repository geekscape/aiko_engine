/**
 * Please do not remove the following notices.
 *
 * \file       serial.h
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

int open_serial_port(const char *serial_port_name, speed_t baud_rate);
