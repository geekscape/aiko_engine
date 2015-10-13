/**
 * Please do not remove the following notices.
 *
 * \file       wifi_configure.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * Description
 * ~~~~~~~~~~~
 * Wi-Fi configuration example: Unix implementation
 *
 * Usage
 * ~~~~~
 * ./wifi_configuration WIFI_SSID WIFI_PASSWORD
 *
 * To Do
 * ~~~~~
 * - https://en.wikipedia.org/wiki/Datagram_Transport_Layer_Security ?
 */

#include <stdio.h>
#include <string.h>

#include "aiko_engine.h"
#include "aiko_network.h"

/* ------------------------------------------------------------------------- */

int main(
  int   argc,
  char *argv[]) {

  printf("[%s %s]\n", argv[0], AIKO_VERSION);

  if (argc != 3) {
    fprintf(stderr, "Usage: %s WIFI_SSID WIFI_PASSWORD\n\n", argv[0]);
    exit(-1);
  }

  char *wifi_ssid     = argv[1];
  char *wifi_password = argv[2];

  if (strlen(wifi_ssid) > 32) {
    fprintf(stderr, "Maximum Wi-Fi SSID length is 32 characters\n");
    exit(-1);
  }

  if (strlen(wifi_password) > 64) {
    fprintf(stderr, "Maximum Wi-Fi password length is 64 characters\n");
    exit(-1);
  }

  char message[256];

  printf("Warning: Transmit Wi-Fi password in PLAIN-TEXT over LAN (N/y) ? ");

  if (fgets(message, sizeof(message), stdin)) {
    if (strcmp(message, "y\n") != 0) {
      printf("Canceling Wi-Fi configuration\n");
    }
    else {
      printf("Sending Wi-Fi configuration\n");

      sprintf(
        message, "(4:wifi(%ld:%s%ld:%s))\n",
        strlen(wifi_ssid),     wifi_ssid,
        strlen(wifi_password), wifi_password
      );

      aiko_stream_t *aiko_stream = aiko_create_socket_stream(
        AIKO_STREAM_SOCKET_UDP4, TRUE, 0, AIKO_PORT
      );

      aiko_socket_send_broadcast(                       // aiko_socket_send() ?
        aiko_stream, (uint8_t *) message, strlen(message)
      );
    }
  }

  return(0);
}

/* ------------------------------------------------------------------------- */
