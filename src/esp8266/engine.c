/**
 * Please do not remove the following notices.
 *
 * \file       engine.c
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

#include "aiko_engine.h"
#include "aiko_wifi.h"

uint8_t aiko_system_ready_state = FALSE;

void ICACHE_FLASH_ATTR
aiko_system_ready(void) {
  aiko_system_ready_state = TRUE;

  if (wifi_get_opmode() != SOFTAP_MODE) {
    wifi_station_scan(NULL, aiko_wifi_ap_scan_done);
  }
}
