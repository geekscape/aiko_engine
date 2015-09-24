/**
 * Please do not remove the following notices.
 *
 * \file       engine.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * Description
 * ~~~~~~~~~~~
 * Aiko engine: Unix implementation
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#include <assert.h>
#include <sys/select.h>

#include "aiko_engine.h"
#include "aiko_network.h"
#include "aiko_serial.h"

aiko_source_t ATTRIBUTES
*aiko_create_file_source(
  FILE *file) {

  assert(aiko_source_count < AIKO_SOURCE_MAXIMUM);

  return(aiko_create_source(AIKO_SOURCE_FILE, fileno(file)));
}

void ATTRIBUTES
aiko_loop(
  uint16_t loop_limit) {

  uint8_t  buffer[MESSAGE_BUFFER_SIZE];
  int      fd, fdLargest, index, length;
  fd_set   readSet;

  int     flags = 0;
  uint8_t no_loop_limit = (loop_limit == AIKO_LOOP_FOREVER);

  while ((no_loop_limit  ||  loop_limit > 0)  &&
         (aiko_source_count > 0  ||  aiko_timer_count > 0)) {

    if (loop_limit > 0) loop_limit --;

    if (aiko_timer_count > 0) {
      struct timeval time_value;
      gettimeofday(& time_value, NULL);
      long long time_now = time_value.tv_sec * 1000000 + time_value.tv_usec;

      aiko_timer_t *aiko_timer = aiko_next_timer();

      if (time_now >= aiko_timer->timeout) {
        uint8_t handled = aiko_timer->handler(aiko_timer);
        aiko_timer_update(aiko_timer);
      }
    }

    fdLargest = 0;
    FD_ZERO(& readSet);

    for (index = 0; index < aiko_source_count; index ++) {
      fd = aiko_sources[index]->fd;

      if (fd >= 0) {
        if (fd > fdLargest) fdLargest = fd;
        FD_SET(fd, & readSet);
      }
    }

    struct timeval timeout = { 0, 50000 };  // 50 milliseconds

    if (select(fdLargest + 1, & readSet, NULL, NULL, & timeout) > 0) {
      uint8_t aiko_source_destroyed = FALSE;

      for (index = 0; index < aiko_source_count; index ++) {
        fd = aiko_sources[index]->fd;

        if (FD_ISSET(fd, & readSet)) {
          switch (aiko_sources[index]->type) {
            case AIKO_SOURCE_FILE:
                if (fgets((char *) buffer, sizeof(buffer), stdin) == NULL) {
                  return;                                     // TODO: Clean-up
                }

                length = strlen((const char *) buffer);
              break;

            case AIKO_SOURCE_SERIAL:
              length = read(fd, buffer, sizeof(buffer));
              break;

            case AIKO_SOURCE_SOCKET_TCP4:
            case AIKO_SOURCE_SOCKET_UDP4:
              length = aiko_socket_receive(fd, buffer, sizeof(buffer));
              break;

            default:
              printf("aiko_engine(): Error: Unknown aiko_source type\n");
              break;
          }

          if (length == 0) {
            aiko_destroy_source(aiko_sources[index]);
            aiko_sources[index]   = NULL;
            aiko_source_destroyed = TRUE;
          }
          else {
            aiko_handler_t *handler = aiko_sources[index]->handler;

            if (handler != NULL) {
              uint8_t handled = aiko_sources[index]->handler(
                aiko_sources[index], buffer, length
              );
            }
          }
        }
      }

      if (aiko_source_destroyed) {                    // Compact aiko_sources[]
        int index1, index2;
        for (index1 = 0;  index1 < aiko_source_count; index1 ++) {
          if (aiko_sources[index1] == NULL) {
            for (index2 = index1;  index2 + 1 < aiko_source_count;  index2 ++) {
              aiko_sources[index2] = aiko_sources[index2 + 1];
            }

            aiko_sources[-- aiko_source_count] = NULL;
          }
        }
      }
    }
  }
}
