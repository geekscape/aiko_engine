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
 * - If Wi-Fi connection lost, then clear "aiko_broadcast_ipv4", so that it is
 *     recalculated when Wi-Fi reestablished.
 */

#include "ip_addr.h"
#include "espconn.h"
#include "ets_sys.h"
#include "user_interface.h"

#include "aiko_network.h"

extern aiko_stream_t *aiko_streams[];
extern uint8_t        aiko_stream_count;

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
  for (index = 0;  index < aiko_stream_count;  index ++) {
    aiko_stream_t *aiko_stream = aiko_streams[index];
    if (aiko_stream->type == AIKO_STREAM_SOCKET_UDP4) {
      if (aiko_stream->id.socket.local_port==udp_conn->proto.udp->local_port) {
        os_memcpy(
          & aiko_stream->id.socket.remote_address_ipv4,
            udp_conn->proto.udp->remote_ip,
          sizeof(aiko_stream->id.socket.remote_address_ipv4)
        );

        aiko_stream->id.socket.remote_port = udp_conn->proto.udp->remote_port;

        if (aiko_stream->handler != NULL) {
          uint8_t handled = aiko_stream->handler(aiko_stream, buffer, length);
        }
      }
    }
  }
}

int ICACHE_FLASH_ATTR
aiko_create_socket_tcp(
  uint8_t  bind_flag,
  uint32_t address_ipv4,
  uint16_t port) {

  return(-1);
}

int ICACHE_FLASH_ATTR
aiko_create_socket_udp(
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


void ICACHE_FLASH_ATTR
aiko_destroy_socket(
  int fd) {

//espconn_delete(struct espconn *espconn);
//espconn_disconnect(struct espconn *espconn);  // Don't invoke during callback
}

int ICACHE_FLASH_ATTR
aiko_socket_receive(
  aiko_stream_t *aiko_stream,
  uint8_t       *buffer,
  uint16_t       buffer_size) {

  return(-1);
}

int ICACHE_FLASH_ATTR
aiko_socket_send(
  aiko_stream_t *aiko_stream,
  uint8_t       *buffer,
  uint16_t       size) {

  os_memcpy(
      aiko_udp_conn->proto.udp->remote_ip,
    & aiko_stream->id.socket.remote_address_ipv4,
    sizeof(aiko_udp_conn->proto.udp->remote_ip)
  );

  aiko_udp_conn->proto.udp->remote_port = aiko_stream->id.socket.remote_port;

  espconn_sent(aiko_udp_conn, buffer, size);

  return(0);
}

void ICACHE_FLASH_ATTR
aiko_socket_send_broadcast(
  aiko_stream_t *aiko_stream,
  uint8_t       *buffer,
  uint16_t       size) {

  if (aiko_broadcast_ipv4 == 0) {
    struct ip_info wifi_ip_info;
    wifi_get_ip_info(STATION_IF, & wifi_ip_info);
    aiko_broadcast_ipv4 = wifi_ip_info.ip.addr | (~ wifi_ip_info.netmask.addr);
  }

  aiko_stream->id.socket.remote_address_ipv4 = aiko_broadcast_ipv4;

  aiko_socket_send(aiko_stream, buffer, size);
}
