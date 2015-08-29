/**
 * Please do not remove the following notices.
 *
 * \file       lisp_extend.h
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#pragma once

#include "lisp.h"

#define AIKO_STORE_MAGIC  0x41494b4f

typedef struct {
  uint16_t size;
  uint32_t magic;
  uint8_t  wifi_ssid[33];                                    // null-terminated
  uint8_t  wifi_password[65];                                // null-terminated

// Additional storage may follow ...
}
  aiko_store_t;

void lisp_extend(tExpression *lisp_environment, aiko_store_t *store);
