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

#include "aiko_engine.h"  // define aiko_source_t
#include "aiko_compatibility.h"
#include "aiko_serial.h"

extern aiko_source_t *aiko_sources[];
extern uint8_t        aiko_source_count;

static uint8_t  serial_buffer[AIKO_SERIAL_BUFFER_SIZE];
static uint16_t serial_length = 0;
static uint8_t  serial_record_delimiter = 0x00;

uint8_t ICACHE_FLASH_ATTR
serial_timer_handler(
  void *timer_self) {

  int index;

  uint16_t length = rx_buff_deq(
    & serial_buffer[serial_length], sizeof(serial_buffer) - serial_length
  );

  if (length > 0) {
    serial_length += length;

    if (serial_record_delimiter == 0x00  ||
        serial_buffer[serial_length - 1] == serial_record_delimiter  ||
        serial_length == AIKO_SERIAL_BUFFER_SIZE) {

      for (index = 0;  index < aiko_source_count;  index ++) {
        aiko_source_t *aiko_source = aiko_sources[index];
        if (aiko_source->type == AIKO_SOURCE_SERIAL) {
          uint8_t handled = aiko_source->handler(serial_buffer, serial_length);
        }
      }

      serial_length = 0;
    }
  }
}

int ICACHE_FLASH_ATTR
serial_port_open(
  const char *serial_port_name,
  speed_t     baud_rate,
  uint8_t     record_delimiter) {

  serial_record_delimiter = record_delimiter;

  uart_init(baud_rate, baud_rate);

  aiko_time_t   period = { 0, 20000 };  // 20 milliseconds = 50 Hz
  aiko_timer_t *timer  = aiko_add_timer(& period, serial_timer_handler);

  return(0);
}
