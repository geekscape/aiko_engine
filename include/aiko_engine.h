/**
 * Please do not remove the following notices.
 *
 * \file       aiko_engine.h
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
#include "aiko_version.h"           // auto-magically generated by the makefile

#define AIKO_PORT            4149
#define AIKO_STREAM_MAXIMUM     4
#define AIKO_TIMER_MAXIMUM      4
#define MESSAGE_BUFFER_SIZE   256

#define AIKO_LOOP_FOREVER      0
#define AIKO_HANDLED           1
#define AIKO_NOT_HANDLED       0

typedef uint8_t (aiko_handler_t)(
  void *aiko_stream, uint8_t *message, uint16_t length
);

typedef uint8_t (aiko_timer_handler_t)(void *aiko_timer);

typedef enum {
  AIKO_STREAM_FILE,
  AIKO_STREAM_SERIAL,
  AIKO_STREAM_SOCKET_UDP4,
  AIKO_STREAM_SOCKET_TCP4,
  AIKO_STREAM_USER_DEFINED = 64
}
  aiko_stream_type;

struct aiko_socket {
  uint32_t local_address_ipv4;
  uint16_t local_port;
  uint32_t remote_address_ipv4;     // valid for most recent;y received message
  uint16_t remote_port;             // ditto
};

typedef struct {
  aiko_stream_type  type;
  aiko_handler_t   *handler;
  int               fd;
  union {
    struct aiko_socket socket;
  }                 id;
}
  aiko_stream_t;

extern aiko_stream_t *aiko_streams[];
extern uint8_t        aiko_stream_count;

typedef struct {
  uint32_t seconds;
  uint32_t microseconds;
}
  aiko_time_t;

typedef struct {
#ifdef __ets__
  os_timer_t            esp8266_timer;
#else
  aiko_time_t           period;
  aiko_timer_handler_t *handler;
#ifdef ARDUINO
  long                  timeout;
#else
  long long             timeout;
#endif
#endif
}
  aiko_timer_t;

extern uint8_t aiko_timer_count;

aiko_timer_t *aiko_add_timer(
  aiko_time_t *period, aiko_timer_handler_t *handler
);

void aiko_delete_timer(aiko_timer_t *aiko_timer);

aiko_timer_t *aiko_next_timer(void);

void aiko_timer_update(aiko_timer_t *aiko_timer);

void aiko_add_handler(aiko_stream_t *aiko_stream, aiko_handler_t *handler);

aiko_stream_t *aiko_create_stream(aiko_stream_type type, int fd);

#ifdef ARDUINO
#elif __ets__
extern uint8_t aiko_system_ready_state;

void aiko_system_ready(void);
#else
aiko_stream_t *aiko_create_file_stream(FILE *file);

uint8_t aiko_file_write(
  aiko_stream_t *aiko_stream, uint8_t *message, uint16_t length
);
#endif

aiko_stream_t *aiko_create_serial_stream(
  const char *serial_port_name, speed_t baud_rate, uint8_t record_delimiter
);

aiko_stream_t *aiko_create_socket_stream(
  aiko_stream_type type,
  uint8_t          bind_flag,
  uint32_t         address_ipv4,
  uint16_t         port
);

void aiko_destroy_stream(aiko_stream_t *aiko_stream);

void aiko_loop(uint16_t loop_limit);

int aiko_stream_send(
  aiko_stream_t *aiko_stream, uint8_t *message, uint16_t length
);

void aiko_buffer_dump(const char *label, uint8_t *buffer, uint16_t length);
