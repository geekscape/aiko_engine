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
 */

#include "ip_addr.h"
#include "espconn.h"
#include "ets_sys.h"

#include "aiko_compatibility.h"

static struct espconn *udp_conn;

void ICACHE_FLASH_ATTR
udp_handler(
  void           *arg,
  char           *buffer,
  unsigned short  length) {

  if (buffer == NULL) return;
}

int ICACHE_FLASH_ATTR
udp_create_socket(
  uint8_t  bind_flag,
  uint16_t port) {

  udp_conn = (struct espconn *) os_zalloc(sizeof(struct espconn));
  udp_conn->type  = ESPCONN_UDP;
  udp_conn->state = ESPCONN_NONE;
  udp_conn->proto.udp = (esp_udp *) os_zalloc(sizeof(esp_udp));

  if (bind_flag) {
    udp_conn->proto.udp->local_port  = port;  // espconn_port();
    udp_conn->proto.udp->remote_port = port;
    uint32_t server_ip = 0xffffff00;  // 0xffffffff;
    os_memcpy(udp_conn->proto.udp->remote_ip, & server_ip, sizeof(server_ip));
    espconn_regist_recvcb(udp_conn, udp_handler);
  }

  sint8 result = espconn_create(udp_conn);

  return(0);
}

int ICACHE_FLASH_ATTR
udp_read(
  int       fd,
  uint8_t  *buffer,
  uint16_t  buffer_size) {

  return(-1);
}

int ICACHE_FLASH_ATTR
udp_send_unicast(
  int       fd,
  uint32_t  address_ipv4,
  uint16_t  port,
  uint8_t  *buffer,
  uint16_t  size) {

  espconn_sent(udp_conn, buffer, size);
  return(0);
}

void ICACHE_FLASH_ATTR
udp_send_broadcast(
  int       fd,
  uint16_t  port,
  uint8_t  *buffer,
  uint16_t  size) {

  udp_send_unicast(fd, 0xffffff00, port, buffer, size);
}
