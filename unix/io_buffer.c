/**
 * Please do not remove the following notices.
 *
 * \file       io_buffer.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2014-2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#include "../aiko_engine.h"

char *aikoReaderBuffer;
int   aikoReaderBufferSize;
int   aikoReaderBufferIndex;

int fileGetC() {
  int ch = EOF;

  if (aikoReaderBufferIndex < aikoReaderBufferSize) {
    ch = aikoReaderBuffer[aikoReaderBufferIndex ++];
  }

  return(ch);
}

void fileUngetC(
  int ch) {

  if (aikoReaderBufferIndex > 0) aikoReaderBufferIndex --;
}

int fileIsEmpty() {
  return(aikoReaderBufferIndex == aikoReaderBufferSize);
}

tReader aikoBufferReader = { fileGetC, fileIsEmpty, fileUngetC };

/* ------------------------------------------------------------------------- */
tReader *aikoIoInitialize(
  char *buffer,
  int   size) {

  aikoError = AIKO_ERROR_NONE;

  aikoReaderBuffer      = buffer;
  aikoReaderBufferSize  = size;
  aikoReaderBufferIndex = 0;

  return(& aikoBufferReader);
}

/* ------------------------------------------------------------------------- */
