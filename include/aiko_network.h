/**
 * Please do not remove the following notices.
 *
 * \file       aiko_network.h
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#pragma once

#include "aiko_engine.h"

int aiko_create_socket_tcp(
  uint8_t bind_flag, uint32_t address_ipv4, uint16_t port
);

int aiko_create_socket_udp(uint8_t bind_flag, uint16_t port);

void aiko_destroy_socket(int fd);

uint32_t aiko_get_ip_address(char *hostname);

int aiko_socket_receive(
  aiko_stream_t *aiko_stream, uint8_t *buffer, uint16_t size
);

int aiko_socket_send(
  aiko_stream_t *aiko_stream, uint8_t *buffer, uint16_t size
);

void aiko_socket_send_broadcast(
  aiko_stream_t *aiko_stream, uint8_t *buffer, uint16_t size
);
