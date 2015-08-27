#include "lisp.h"

#define EOF -1

char *readerBuffer;
int   readerBufferSize;
int   readerBufferIndex;

int fileGetC() {
  int ch = EOF;

  if (readerBufferIndex < readerBufferSize) {
    ch = readerBuffer[readerBufferIndex ++];
  }

  return(ch);
}

void fileUngetC(
  int ch) {

  if (readerBufferIndex > 0) readerBufferIndex --;
}

int fileIsEOF() {
  return(readerBufferIndex == readerBufferSize);
}

tReader bufferReader = { fileGetC, fileIsEOF, fileUngetC };

/* ------------------------------------------------------------------------- */

tReader *initializeIO(
  char *buffer,
  int   size) {

  readerBuffer = buffer;
  readerBufferSize = size;
  readerBufferIndex = 0;

  return(& bufferReader);
}

/* ------------------------------------------------------------------------- */
