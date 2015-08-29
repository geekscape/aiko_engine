/**
 * Please do not remove the following notices.
 *
 * \file       store.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * Description
 * ~~~~~~~~~~~
 * Persistent storage: Unix implementation
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#include <fcntl.h>
#include <stdio.h>

#include "aiko_store.h"

#define AIKO_STORE_PATHNAME  "/tmp/aiko_store"

uint8_t aiko_store_load(
  void     *store,
  uint16_t  size) {

  uint8_t result = FALSE;
  int fd = open(AIKO_STORE_PATHNAME, O_RDONLY);

  if (fd < 0) {
    perror("Error: Opening "AIKO_STORE_PATHNAME);
  }
  else {
    ssize_t length = read(fd, store, size);
    close(fd);

    if (length != size) {
      printf("Error: Reading "AIKO_STORE_PATHNAME": Incorrect length\n");
    }
    else {
      result = TRUE;
    }
  }
  return(result);
}

uint8_t aiko_store_save(
  void     *store,
  uint16_t  length) {

  uint8_t result = FALSE;

  int fd = open(AIKO_STORE_PATHNAME, O_CREAT | O_WRONLY | O_TRUNC, 0600);

  if (fd < 0) {
    perror("Error: Opening "AIKO_STORE_PATHNAME);
  }
  else {
    write(fd, store, length);
    close(fd);
    result = TRUE;
  }

  return(result);
}
