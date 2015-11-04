/**
 * Please do not remove the following notices.
 *
 * \file       serial_udp.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * Description
 * ~~~~~~~~~~~
 * Serial-UDP bridge: Cross-platform implementation
 *
 * To Do
 * ~~~~~
 * - Output serial-udp packets on Aiko UDP port
 *   - Provide packet disassembly for human readable output
 *
 * - Implement Lisp extensions and persistence for ...
 *   - Set UDP port for bridge
 *   - Set serial baud rate
 */

#include "aiko_engine.h"
#include "aiko_serial.h"

#include "lisp.h"
#include "../aiko_server/lisp_extend.h"

#include "serial_udp.h"

aiko_store_t aiko_server_store;

aiko_stream_t *aiko_server_stream = NULL;
aiko_stream_t *serial_stream      = NULL;
aiko_stream_t *udp_stream         = NULL;

uint8_t serial_udp_log = 0;

/* ------------------------------------------------------------------------- */

tExpression ATTRIBUTES
*primitiveLog(
  tExpression *expression,
  tExpression *environment) {

  serial_udp_log = ! serial_udp_log;

  return(truth);
}

/* ------------------------------------------------------------------------- */

uint8_t ATTRIBUTES
serial_handler(
  void     *aiko_stream,
  uint8_t  *message,
  uint16_t  length) {

  if (serial_udp_log) aiko_buffer_dump("s", message, length);

  if (udp_stream != NULL) aiko_stream_send(udp_stream, message, length);

  return(AIKO_HANDLED);
}

/* ------------------------------------------------------------------------- */

uint8_t ATTRIBUTES
udp_handler(
  void     *aiko_stream,
  uint8_t  *message,
  uint16_t  length) {

  if (serial_udp_log) aiko_buffer_dump("u", message, length);

  aiko_serial_send(serial_stream, message, length);

  return(AIKO_HANDLED);
}

/* ------------------------------------------------------------------------- */

tExpression ATTRIBUTES
*serial_udp_initialize(
  char     *serial_port,
  uint16_t  udp_port,
  uint8_t   debug_flag) {

  tExpression *lisp_environment = lisp_initialize(debug_flag);
  if (lispError) return(NULL);

  lispAppend(lisp_environment, lispCreatePrimitive("log", primitiveLog));
// TODO: Ultimately, shouldn't need to do this ...
  lispExpressionBookmark = lispExpressionCurrent;

  memset(& aiko_server_store, 0x00, sizeof(aiko_server_store));
  aiko_server_store.size    = sizeof(aiko_server_store);
  aiko_server_store.magic   = AIKO_STORE_MAGIC;
  aiko_server_store.version = AIKO_STORE_VERSION;

  lisp_extend(lisp_environment, & aiko_server_store);
  if (lispError) return(NULL);

  serial_stream = aiko_create_serial_stream(serial_port, BAUD_RATE, 0x00);
  if (serial_stream == NULL) return(NULL);

  aiko_add_handler(serial_stream, serial_handler);

  udp_stream = aiko_create_socket_stream(
    AIKO_STREAM_SOCKET_UDP4, TRUE, 0, udp_port
  );

  aiko_add_handler(udp_stream, udp_handler);

  aiko_server_stream = aiko_create_socket_stream(
    AIKO_STREAM_SOCKET_UDP4, TRUE, 0, AIKO_PORT
  );

  aiko_add_handler(aiko_server_stream, lisp_message_handler);

  return(lisp_environment);
}

/* ------------------------------------------------------------------------- */
