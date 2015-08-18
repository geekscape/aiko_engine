/**
 * Please do not remove the following notices.
 *
 * \file       network.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * Description
 * ~~~~~~~~~~~
 * UDP: Unix implementation
 *
 * To Do
 * ~~~~~
 * - Better error handling (replace assert).
 */

#include <arpa/inet.h>
#include <assert.h>
#include <ifaddrs.h>
#include <net/if.h>

#include "aiko_network.h"

int aiko_udp_create_socket(
  uint8_t  bind_flag,
  uint16_t port) {

  int fd = socket(PF_INET, SOCK_DGRAM, 0);

  if (fd >= 0) {
    assert(
      setsockopt(fd, SOL_SOCKET, SO_BROADCAST, & (int) {1}, sizeof(int)) == 0);

    if (bind_flag) {
      assert(
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == 0);
      assert(
        setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO,
          & (struct timeval) {.tv_sec = 1}, sizeof(struct timeval)) == 0);

      struct sockaddr_in sockaddr = {
        .sin_family = AF_INET,
        .sin_port   = htons(port)
      };

      assert(bind(fd, (struct sockaddr *) & sockaddr, sizeof(sockaddr)) == 0);
    }
  }

  return(fd);
}

int aiko_udp_read(
  int       fd,
  uint8_t  *buffer,
  uint16_t  buffer_size) {

  int       flags = 0;
  int       length;
  struct    sockaddr clientAddr;
  socklen_t clientAddrLen;

  length = recvfrom(
    fd, buffer, buffer_size, flags, & clientAddr, & clientAddrLen
  );

  return(length);
}

int aiko_udp_send_unicast(
  int       fd,
  uint32_t  address_ipv4,
  uint16_t  port,
  uint8_t  *buffer,
  uint16_t  size) {

  struct sockaddr_in sockaddr = {
    .sin_family      = AF_INET,
    .sin_port        = htons(port),
    .sin_addr.s_addr = htonl(address_ipv4),
  };

  int length = sendto(
    fd, buffer, size, MSG_DONTWAIT,
    (struct sockaddr *) & sockaddr, sizeof(sockaddr)
  );

  return(length);
}

void aiko_udp_send_broadcast(
  int       fd,
  uint16_t  port,
  uint8_t  *buffer,
  uint16_t  size) {

  struct ifaddrs *ifaddr, *ifaddrs_list;
  uint32_t netmask, ipb, ipl;

  getifaddrs(& ifaddrs_list);

  for (ifaddr = ifaddrs_list; ifaddr; ifaddr = ifaddr->ifa_next) {
    if (ifaddr->ifa_addr  &&  ifaddr->ifa_addr->sa_family == AF_INET) {
      if (! (ifaddr->ifa_flags & IFF_UP))   continue;
      if (ifaddr->ifa_flags & IFF_LOOPBACK) continue;

      netmask =
        ntohl(((struct sockaddr_in *) ifaddr->ifa_netmask)->sin_addr.s_addr);
      ipl = ntohl(((struct sockaddr_in *) ifaddr->ifa_addr)->sin_addr.s_addr);
      ipb = (ipl & netmask) | ~ netmask;

      aiko_udp_send_unicast(fd, ipb, port, buffer, size);
    }
  }

  freeifaddrs(ifaddrs_list);
}
