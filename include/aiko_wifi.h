/**
 * Please do not remove the following notices.
 *
 * \file       aiko_wifi.h
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

#ifndef __ets__
typedef enum {
  OK = 0,
  FAIL,
  PENDING,
  BUSY,
  CANCEL,
}
  STATUS;
#endif

void aiko_wifi_ap_scan_done(void *arg, STATUS status);

void aiko_wifi_softap_configure(void);

void aiko_wifi_station_configure(char ssid[33], char password[65]);
