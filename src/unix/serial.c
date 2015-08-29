/**
 * Please do not remove the following notices.
 *
 * \file       serial.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * Description
 * ~~~~~~~~~~~
 * Serial port: Unix implementation
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>

#include "aiko_serial.h"

static uint8_t        aiko_termios_saved = FALSE;
static struct termios aiko_termios_backup;

int aiko_serial_port_open(
  const char *serial_port_name,
  speed_t     baud_rate,
  uint8_t     record_delimiter) {

  int fd = open(serial_port_name, O_RDWR | O_NOCTTY | O_NONBLOCK);

  if (fd == -1) {
    fprintf(stderr, "Error opening serial port: ");
    perror(serial_port_name);
  }
  else {
    struct termios options;
    tcgetattr(fd, & options);

    memcpy(& aiko_termios_backup, & options, sizeof(options));
    aiko_termios_saved  = TRUE;

    cfsetispeed(& options, baud_rate);
    cfsetospeed(& options, baud_rate);

    options.c_cflag |= (CLOCAL | CREAD);

/* Turn off :echo, canonical mode, extended input processing, signal chars */
    options.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

/* Turn off: SIGINT on BREAK, CR-toNL, input parity check,
   strip the 8th bit on input, output flow control */
//  options.c_iflag &= ~(BRKINT | ICRNL | ISTRIP | IXON);
    options.c_iflag &= ~(ICRNL | ISTRIP | IXON);

/* clear size bits, parity checking off */
    options.c_cflag &= ~(CSIZE | PARENB);

    options.c_cflag     |= CS8;       /* set 8 bits per char   */
    options.c_oflag     &= ~(OPOST);  /* output processing off */
    options.c_cc[VMIN]   = 1;         /* one byte at a time    */
    options.c_cc[VTIME]  = 0;         /* no timer on input     */

//  options.c_cflag &= ~CSTOPB;
//  options.c_cflag |=  (IXON | IXOFF | IXANY);

    tcsetattr(fd, TCSANOW, & options);
  }

  return(fd);
}
