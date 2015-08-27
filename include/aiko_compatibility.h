/**
 * Please do not remove the following notices.
 *
 * \file       aiko_compatibility.h
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#pragma once

#ifdef __ets__
#define ATTRIBUTES  ICACHE_FLASH_ATTR
#else
#define ATTRIBUTES
#endif

#ifdef ARDUINO
#define PRINT(output)    Serial.print(output)
#define PRINTLN(output)  Serial.print(output"\n")
#elif __ets__
#define EOF (-1)

#include <c_types.h>
#include <mem.h>

#define free     os_free
#define malloc   os_malloc
#define memcmp   os_memcmp
#define memcpy   os_memcpy
#define memset   os_memset
#define printf   os_printf
#define sprintf  os_sprintf
#define strlen   os_strlen

#define PRINT(output)    printf(output)
#define PRINTLN(output)  printf(output"\n")
#else
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PRINT(output)    printf(output)
#define PRINTLN(output)  printf(output"\n")
#endif

#ifdef ARDUINO
#elif __ets__
// #ifdef ARDUINO
// typedef unsigned int size_t;        // Arduino IDE ESP8266 c_types.h fail :(
// #endif
#include <os_type.h>
#else
#include <sys/time.h>
#endif

#ifdef ARDUINO
#elif __ets__
#include <osapi.h>
#define usleep(microseconds)  os_delay_us(microseconds)
#else
#include <unistd.h>
#endif

#ifdef ARDUINO
typedef unsigned long speed_t;
#elif __ets__
typedef unsigned long speed_t;
#else
#include <termios.h>
#endif
