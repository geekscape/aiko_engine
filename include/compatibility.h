/**
 * Please do not remove the following notices.
 *
 * \file       compatibility.h
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#pragma once

#ifndef __ets__
#define ICACHE_FLASH_ATTR
#endif

#ifdef __ets__
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
#else
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#ifdef __ets__
#include <osapi.h>
#define usleep(microseconds)  os_delay_us(microseconds)
#else
#include <unistd.h>
#endif

#ifdef __ets__
typedef unsigned long speed_t;
#else
#include <termios.h>
#endif
