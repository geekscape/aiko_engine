/**
 * Please do not remove the following notices.
 *
 * \file       io.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2014-2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#include "../aiko_engine.h"

FILE *readerFile;

int fileGetC() {
  return(getc(readerFile));
}

void fileUngetC(
  int ch) {

  ungetc(ch, readerFile);
}

int fileIsEmpty() {
  int ch = fileGetC();

  if (ch != EOF) fileUngetC(ch);

  return(ch == EOF);
}

tReader reader = { fileGetC, fileIsEmpty, fileUngetC };

/* ------------------------------------------------------------------------- */
tReader *aikoIoInitialize(
  FILE *inputFile) {

  readerFile = inputFile;
  return(& reader);
}

/* ------------------------------------------------------------------------- */
