/**
 * Please do not remove the following notices.
 *
 * \file       network.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * Description
 * ~~~~~~~~~~~
 * UDP: ESP8266 implementation
 *
 * To Do
 * ~~~~~
 * - Refactor aiko_engine.c into common, Unix and ESP8266.
 *   - Don't directly reference static variables in common/aiko_engine.c
 * - Handle multiple UDP socket connections.
 * - Improve efficiency of aiko_udp_handler().
 */

#include "ip_addr.h"
#include "espconn.h"
#include "ets_sys.h"
#include "user_interface.h"

#include "aiko_engine.h"  // define aiko_source_t
#include "aiko_compatibility.h"

extern aiko_source_t *aiko_sources[];
extern uint8_t        aiko_source_count;

static uint32_t aiko_broadcast_ipv4 = 0;

static struct espconn *aiko_udp_conn;

void ICACHE_FLASH_ATTR
aiko_udp_handler(
  void           *arg,
  char           *buffer,
  unsigned short  length) {

  if (buffer == NULL) return;

  struct espconn *udp_conn = (struct espconn *) arg;

  int index;
  for (index = 0;  index < aiko_source_count;  index ++) {
    aiko_source_t *aiko_source = aiko_sources[index];
    if (aiko_source->type == AIKO_SOURCE_SOCKET_UDP4) {
      if (aiko_source->id.port == udp_conn->proto.udp->local_port) {
        uint8_t handled = aiko_source->handler(buffer, length);
      }
    }
  }
}

int ICACHE_FLASH_ATTR
aiko_udp_create_socket(
  uint8_t  bind_flag,
  uint16_t port) {

  aiko_udp_conn = (struct espconn *) os_zalloc(sizeof(struct espconn));
  aiko_udp_conn->type  = ESPCONN_UDP;
  aiko_udp_conn->state = ESPCONN_NONE;
  aiko_udp_conn->proto.udp = (esp_udp *) os_zalloc(sizeof(esp_udp));

  if (bind_flag) {
    aiko_udp_conn->proto.udp->local_port  = port;  // espconn_port();
    aiko_udp_conn->proto.udp->remote_port = port;
    espconn_regist_recvcb(aiko_udp_conn, aiko_udp_handler);
  }

  int8_t result = espconn_create(aiko_udp_conn);

  return(0);
}

int ICACHE_FLASH_ATTR
aiko_udp_read(
  int       fd,
  uint8_t  *buffer,
  uint16_t  buffer_size) {

  return(-1);
}

int ICACHE_FLASH_ATTR
aiko_udp_send_unicast(
  int       fd,
  uint32_t  address_ipv4,
  uint16_t  port,
  uint8_t  *buffer,
  uint16_t  size) {

  os_memcpy(
    aiko_udp_conn->proto.udp->remote_ip, & address_ipv4, sizeof(address_ipv4)
  );

  aiko_udp_conn->proto.udp->remote_port = port;

  espconn_sent(aiko_udp_conn, buffer, size);

  return(0);
}

void ICACHE_FLASH_ATTR
aiko_udp_send_broadcast(
  int       fd,
  uint16_t  port,
  uint8_t  *buffer,
  uint16_t  size) {

  if (aiko_broadcast_ipv4 == 0) {
    struct ip_info wifi_ip_info;
    wifi_get_ip_info(STATION_IF, & wifi_ip_info);
    aiko_broadcast_ipv4 = wifi_ip_info.ip.addr | (~ wifi_ip_info.netmask.addr);
  }

  aiko_udp_send_unicast(fd, aiko_broadcast_ipv4, port, buffer, size);
}
