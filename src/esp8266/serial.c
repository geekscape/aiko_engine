/**
 * Please do not remove the following notices.
 *
 * \file       serial.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * Description
 * ~~~~~~~~~~~
 * Serial port: ESP8266 implementation
 *
 * ****************************************************
 * *** Ensure that ESP8266: include/driver/uart.h   ***
 * *** Is changed to ...    #define UART_BUFF_EN  1 ***
 * ****************************************************
 *
 * To Do
 * ~~~~~
 * - Refactor aiko_engine.c into common, Unix and ESP8266.
 *   - Don't directly reference static variables in common/aiko_engine.c
 * - Improve efficiency of serial_handler().
 */

#include "driver/uart.h"

#include "aiko_engine.h"  // define aiko_stream_t
#include "aiko_compatibility.h"
#include "aiko_serial.h"

extern aiko_stream_t *aiko_streams[];
extern uint8_t        aiko_stream_count;

static uint8_t  aiko_serial_buffer[AIKO_SERIAL_BUFFER_SIZE];
static uint16_t aiko_serial_length = 0;
static uint8_t  aiko_serial_record_delimiter = 0x00;

uint8_t ICACHE_FLASH_ATTR
aiko_serial_timer_handler(
  void *timer_self) {

  int index;

  uint16_t length = rx_buff_deq(
    & aiko_serial_buffer[aiko_serial_length],
    sizeof(aiko_serial_buffer) - aiko_serial_length
  );

  if (length > 0) {
    aiko_serial_length += length;

    if (aiko_serial_record_delimiter == 0x00  ||
        aiko_serial_length == AIKO_SERIAL_BUFFER_SIZE  ||
        aiko_serial_buffer[aiko_serial_length - 1] ==
          aiko_serial_record_delimiter) {

      for (index = 0;  index < aiko_stream_count;  index ++) {
        aiko_stream_t *aiko_stream = aiko_streams[index];
        if (aiko_stream->type == AIKO_STREAM_SERIAL) {
          aiko_handler_t *handler = aiko_streams[index]->handler;

          if (handler != NULL) {
            uint8_t handled = aiko_stream->handler(
              aiko_stream, aiko_serial_buffer, aiko_serial_length
            );
          }
        }
      }

      aiko_serial_length = 0;
    }
  }
}

int ICACHE_FLASH_ATTR
aiko_serial_port_open(
  const char *serial_port_name,
  speed_t     baud_rate,
  uint8_t     record_delimiter) {

  aiko_serial_record_delimiter = record_delimiter;

  if (baud_rate != BAUD_NO_CHANGE) {
    uart_init(baud_rate, baud_rate);
    os_delay_us(5000);             // Allow time to settle before using UART :(
  }

  aiko_time_t   period = { 0, 20000 };  // 20 milliseconds = 50 Hz
  aiko_timer_t *timer  = aiko_add_timer(& period, aiko_serial_timer_handler);

  return(0);
}
