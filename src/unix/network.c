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
 * - Implement TCP socket server.
 * - Implement aiko_create_socket_tcp/udp() for IPv6.
 * - Better error handling (replace assert).
 *   - aiko_get_ip_address(hostname) shouldn't have fprintf(stderr); exit(-1);
 */

#include <assert.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <net/if.h>

#include "aiko_network.h"

int aiko_create_socket_tcp(
  uint8_t  bind_flag,
  uint32_t address_ipv4,
  uint16_t port) {

  int fd = socket(PF_INET, SOCK_STREAM, 0);

  if (fd >= 0) {
    struct sockaddr_in sockaddr = {
      .sin_family      = AF_INET,
      .sin_port        = htons(port),
      .sin_addr.s_addr = htonl(address_ipv4)
    };

    assert(connect(fd, (struct sockaddr *) & sockaddr, sizeof(sockaddr)) == 0);
  }

  return(fd);
}

int aiko_create_socket_udp(
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

void aiko_destroy_socket(
  int fd) {

  close(fd);
}

uint32_t aiko_get_ip_address(
  char *hostname) {

  struct in_addr in_addr_ipv4;
  struct addrinfo hints, *result;

  memset(& hints, 0, sizeof (hints));
  hints.ai_family = PF_UNSPEC;

  if (getaddrinfo (hostname, NULL, & hints, & result) != 0) {
    fprintf(stderr, "Error: Could not resolve hostname: %s\n", hostname);
    exit(-1);
  }

  while (result) {
    switch (result->ai_family) {
      case AF_INET:
        in_addr_ipv4 = ((struct sockaddr_in *) result->ai_addr)->sin_addr;
        break;
//    case AF_INET6:
//      ptr = & ((struct sockaddr_in6 *) result->ai_addr)->sin6_addr;
//      break;
    }

//  result = result->ai_next;
    break;
  }

  freeaddrinfo(result);

  return(ntohl(in_addr_ipv4.s_addr));
}

int aiko_socket_receive(
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

int aiko_socket_send(
  int       fd,
  uint32_t  address_ipv4,
  uint16_t  port,
  uint8_t  *buffer,
  uint16_t  size) {

  struct sockaddr_in sockaddr = {
    .sin_family      = AF_INET,
    .sin_port        = htons(port),
    .sin_addr.s_addr = htonl(address_ipv4)
  };

  int length = sendto(
    fd, buffer, size, MSG_DONTWAIT,
    (struct sockaddr *) & sockaddr, sizeof(sockaddr)
  );

  return(length);
}

void aiko_socket_send_broadcast(
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

      aiko_socket_send(fd, ipb, port, buffer, size);
    }
  }

  freeifaddrs(ifaddrs_list);
}
