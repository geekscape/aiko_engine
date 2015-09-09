/**
 * Please do not remove the following notices.
 *
 * \file       aiko_engine.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * Description
 * ~~~~~~~~~~~
 * Aiko engine: Cross-platform implementation
 *
 * To Do
 * ~~~~~
 * - Ensure aiko_destroy_source() cleans up completely for all source types.
 */

#ifndef __ets__
#include <assert.h>
#endif

#ifdef ARDUINO
#include "vendor/aiko_engine/include/aiko_engine.h"
#include "vendor/aiko_engine/include/aiko_network.h"
#include "vendor/aiko_engine/include/aiko_serial.h"
#else
#include "aiko_engine.h"
#include "aiko_network.h"
#include "aiko_serial.h"
#endif

aiko_source_t *aiko_sources[AIKO_SOURCE_MAXIMUM];
uint8_t        aiko_source_count = 0;

void ATTRIBUTES
aiko_add_handler(
  aiko_source_t  *aiko_source,
  aiko_handler_t *aiko_handler) {

  aiko_source->handler = aiko_handler;
}

aiko_source_t ATTRIBUTES
*aiko_create_source(
  aiko_source_type type,
  int              fd) {

  aiko_source_t *aiko_source = (aiko_source_t *) malloc(sizeof(aiko_source_t));
  aiko_source->type    = type;
  aiko_source->handler = NULL;
  aiko_source->fd      = fd;

  aiko_sources[aiko_source_count ++] = aiko_source;
  return(aiko_source);
}

aiko_source_t ATTRIBUTES
*aiko_create_serial_source(
  const char *serial_port_name,
  speed_t     baud_rate,
  uint8_t     record_delimiter) {

#ifndef __ets__
  assert(aiko_source_count < AIKO_SOURCE_MAXIMUM);
#endif

  aiko_source_t *aiko_source = NULL;

  int fd = aiko_serial_port_open(serial_port_name, baud_rate, record_delimiter);
  if (fd >= 0) aiko_source = aiko_create_source(AIKO_SOURCE_SERIAL, fd);

  return(aiko_source);
}

aiko_source_t ATTRIBUTES
*aiko_create_socket_source(
  aiko_source_type type,
  uint32_t         address_ipv4,
  uint16_t         port) {

#ifndef __ets__
  assert(aiko_source_count < AIKO_SOURCE_MAXIMUM);
#endif

  aiko_source_t *aiko_source = NULL;
  int fd = -1;

  if (type == AIKO_SOURCE_SOCKET_TCP4) {
    fd = aiko_create_socket_tcp(TRUE, address_ipv4, port);
  }

  if (type == AIKO_SOURCE_SOCKET_UDP4) {
    fd = aiko_create_socket_udp(TRUE, port);
  }

  if (fd >= 0) {
    aiko_source                         = aiko_create_source(type, fd);
    aiko_source->id.socket.address_ipv4 = address_ipv4;
    aiko_source->id.socket.port         = port;
  }

  return(aiko_source);
}

void aiko_destroy_source(
  aiko_source_t *aiko_source) {

  if (aiko_source->type == AIKO_SOURCE_SOCKET_TCP4  ||
      aiko_source->type == AIKO_SOURCE_SOCKET_UDP4) {

    aiko_destroy_socket(aiko_source->fd);
    aiko_source->fd = -1;
  }

  free(aiko_source);
}

int aiko_source_send(
  aiko_source_t *aiko_source,
  uint8_t       *message,
  uint16_t       length) {

  int result = -1;

  if (aiko_source->type == AIKO_SOURCE_SOCKET_TCP4  ||
      aiko_source->type == AIKO_SOURCE_SOCKET_UDP4) {

    result = aiko_socket_send(
      aiko_source->fd,
      aiko_source->id.socket.address_ipv4, aiko_source->id.socket.port,
      message, length
    );
  }

  return(result);
}

#ifndef ARDUINO
void ATTRIBUTES
aiko_buffer_dump(
  const char *label,
  uint8_t    *buffer,
  uint16_t    length) {

  int index;

  printf("%s:", label);
  for (index = 0;  index < length;  index ++) printf(" %02x", buffer[index]);
  printf("\n");
}
#endif
