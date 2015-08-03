/**
 * Please do not remove the following notices.
 *
 * \file       io_serial.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2014-2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#include "../aiko_engine.h"

FILE *aikoReaderFile;

int fileGetC() {
  return(getc(aikoReaderFile));
}

void fileUngetC(
  int ch) {

  ungetc(ch, aikoReaderFile);
}

int fileIsEmpty() {
  int ch = fileGetC();

  if (ch != EOF) fileUngetC(ch);

  return(ch == EOF);
}

tReader aikoBufferReader = { fileGetC, fileIsEmpty, fileUngetC };

/* ------------------------------------------------------------------------- */
tReader *aikoIoInitialize(
  FILE *inputFile) {

  aikoReaderFile = inputFile;
  return(& aikoBufferReader);
}

/* ------------------------------------------------------------------------- */
