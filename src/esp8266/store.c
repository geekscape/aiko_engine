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
 * Persistent storage: ESP8266 implementation
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#include "ets_sys.h"
#include "spi_flash.h"

#include "aiko_store.h"

#define ESP8266_FLASH_START_SECTOR  0x3c
#define AIKO_FLASH_SECTOR           (ESP8266_FLASH_START_SECTOR + 1)

uint8_t aiko_store_load(
  void     *store,
  uint16_t  size) {

  spi_flash_read(
    AIKO_FLASH_SECTOR * SPI_FLASH_SEC_SIZE, (uint32 *) store, size
  );

  return(TRUE);
}

uint8_t aiko_store_save(
  void     *store,
  uint16_t  length) {

  spi_flash_erase_sector(AIKO_FLASH_SECTOR);

  spi_flash_write(
    AIKO_FLASH_SECTOR * SPI_FLASH_SEC_SIZE, (uint32 *) store, length
  );

  return(TRUE);
}
