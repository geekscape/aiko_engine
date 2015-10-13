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
 * - Ensure aiko_destroy_stream() cleans up completely for all stream types.
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

aiko_stream_t *aiko_streams[AIKO_STREAM_MAXIMUM];
uint8_t        aiko_stream_count = 0;

void ATTRIBUTES
aiko_add_handler(
  aiko_stream_t  *aiko_stream,
  aiko_handler_t *aiko_handler) {

  aiko_stream->handler = aiko_handler;
}

aiko_stream_t ATTRIBUTES
*aiko_create_stream(
  aiko_stream_type type) {

#ifdef __ets__
  if (aiko_stream_count >= AIKO_STREAM_MAXIMUM) {
    printf("Error: Aiko stream limit\n");
    return(NULL);
  }
#else
  assert(aiko_stream_count < AIKO_STREAM_MAXIMUM);
#endif

  aiko_stream_t *aiko_stream = (aiko_stream_t *) malloc(sizeof(aiko_stream_t));
  aiko_stream->type    = type;
  aiko_stream->fd      = -1;
  aiko_stream->handler = NULL;

  aiko_streams[aiko_stream_count ++] = aiko_stream;
  return(aiko_stream);
}

void aiko_destroy_stream(
  aiko_stream_t *aiko_stream) {

  if (aiko_stream->type == AIKO_STREAM_SOCKET_TCP4  ||
      aiko_stream->type == AIKO_STREAM_SOCKET_UDP4) {

    aiko_destroy_socket(aiko_stream);
    aiko_stream->fd = -1;
  }

  free(aiko_stream);
}

aiko_stream_t ATTRIBUTES
*aiko_create_serial_stream(
  const char *serial_port_name,
  speed_t     baud_rate,
  uint8_t     record_delimiter) {

  aiko_stream_t *aiko_stream = NULL;

  int fd = aiko_serial_port_open(serial_port_name, baud_rate, record_delimiter);

  if (fd >= 0) {
    aiko_stream     = aiko_create_stream(AIKO_STREAM_SERIAL);
    aiko_stream->fd = fd;
  }

  return(aiko_stream);
}

aiko_stream_t ATTRIBUTES
*aiko_create_socket_stream(
  aiko_stream_type type,
  uint8_t          bind_flag,
  uint32_t         address_ipv4,
  uint16_t         port) {

#ifndef __ets__
  assert(aiko_stream_count < AIKO_STREAM_MAXIMUM);
#endif

  aiko_stream_t *aiko_stream = aiko_create_stream(type);
  int fd = -1;

  if (type == AIKO_STREAM_SOCKET_TCP4) {
    fd = aiko_create_socket_tcp(aiko_stream, bind_flag, address_ipv4, port);
  }

  if (type == AIKO_STREAM_SOCKET_UDP4) {
    fd = aiko_create_socket_udp(aiko_stream, bind_flag, port);
  }

  if (fd < 0) {
    aiko_destroy_stream(aiko_stream);
    aiko_stream_count --;
  }
  else {
    aiko_stream->fd = fd;

    aiko_stream->id.socket.bind_flag = bind_flag;
    aiko_stream->id.socket.remote_address_ipv4 = address_ipv4;

    aiko_stream->id.socket.local_port  = bind_flag ? port :    0;
    aiko_stream->id.socket.remote_port = bind_flag ?    0 : port;
  }

  return(aiko_stream);
}

int aiko_stream_send(
  aiko_stream_t *aiko_stream,
  uint8_t       *message,
  uint16_t       length) {

  int result = -1;

  switch(aiko_stream->type) {             // TODO: Array of function pointers !
    case AIKO_STREAM_FILE:
#ifdef ARDUINO
#elif __ets__
#else
      result = aiko_file_write(aiko_stream, message, length);
#endif
      break;

    case AIKO_STREAM_SERIAL:
      result = aiko_serial_send(aiko_stream, message, length);
      break;

    case AIKO_STREAM_SOCKET_TCP4:
    case AIKO_STREAM_SOCKET_UDP4:
      result = aiko_socket_send(aiko_stream, message, length);
      break;

    default:
      break;
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
