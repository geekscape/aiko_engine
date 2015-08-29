/**
 * Please do not remove the following notices.
 *
 * \file       wifi.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * Description
 * ~~~~~~~~~~~
 * Wi-Fi: ESP8266 implementation
 *
 * To Do
 * ~~~~~
 * - If not connected to AP, then match known SSID and connect.
 * - Kick off another system ready -> Wi-Fi station scan ?
 */

#include "user_interface.h"

#include "aiko_engine.h"
#include "aiko_wifi.h"

void ICACHE_FLASH_ATTR
aiko_wifi_ap_scan_done(
  void   *arg,
  STATUS  status) {

  if (status == OK) {
    printf("wifi_ap_scan_done():\n");
    struct bss_info *bss_item = (struct bss_info *) arg;

    while (bss_item != NULL) {
      printf("  ssid: %s\n", bss_item->ssid);
      bss_item = bss_item->next.stqe_next;
    }
  }
}

void ICACHE_FLASH_ATTR
aiko_wifi_softap_configure(void) {
  wifi_set_opmode(STATIONAP_MODE);

  struct softap_config wifi_softap_configuration;

  wifi_softap_get_config(& wifi_softap_configuration);
  wifi_softap_configuration.authmode = AUTH_OPEN;
  ETS_UART_INTR_DISABLE();
  wifi_softap_set_config(& wifi_softap_configuration);
  ETS_UART_INTR_ENABLE();
}

void ICACHE_FLASH_ATTR
aiko_wifi_station_configure(
  char ssid[33],
  char password[65]) {

  struct station_config wifi_station_configuration;

  wifi_station_get_config(& wifi_station_configuration);

  os_memcpy(& wifi_station_configuration.ssid, ssid, 32);
  os_memcpy(& wifi_station_configuration.password, password, 64);

  ETS_UART_INTR_DISABLE();
  wifi_station_set_config(& wifi_station_configuration);
  ETS_UART_INTR_ENABLE();

  if (aiko_system_ready_state) wifi_station_connect();
}
