/**
 * Please do not remove the following notices.
 *
 * \file       network.h
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#pragma once

#include "compatibility.h"

int udp_create_socket(uint8_t bind_flag, uint16_t port);

int udp_read(int fd, uint8_t *buffer, uint16_t size);

int udp_send_unicast(
  int fd, uint32_t address_ipv4, uint16_t port, uint8_t *buffer, uint16_t size);

void udp_send_broadcast(int fd, uint16_t port, uint8_t *buffer, uint16_t size);
