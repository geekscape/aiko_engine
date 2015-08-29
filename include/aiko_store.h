/**
 * Please do not remove the following notices.
 *
 * \file       aiko_store.h
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

uint8_t aiko_store_load(void *store, uint16_t size);

uint8_t aiko_store_save(void *store, uint16_t length);
