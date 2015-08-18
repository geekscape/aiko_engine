/* ToDo
 * ----
 * - Refactor emit() print atom name (see also expression.c).
 */

#include <stdio.h>

#define DEBUG DEBUG_PRINT                         // Used by net/ip/uip-debug.h
#include "net/ip/uip-debug.h"                     // TODO: Just use printf ?

#include "lisp.h"

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

int fileIsEmpty() {
  return(readerBufferIndex == readerBufferSize);
}

tReader bufferReader = { fileGetC, fileIsEmpty, fileUngetC };

/* ------------------------------------------------------------------------- */

tReader *ioInitialize(
  char *buffer,
  int   size) {

  aiko_error = ERROR_NONE;

  readerBuffer      = buffer;
  readerBufferSize  = size;
  readerBufferIndex = 0;

  return(& bufferReader);
}

/* ------------------------------------------------------------------------- */

void emit(
  tExpression *expression) {

  if (expression == NULL) {
    PRINTF("null");
  }
  else if (expression->type == ATOM) {
//  PRINTF("%.*s", expression->atom.name.size, expression->atom.name.ptr);
    char *name = (char *) expression->atom.name.ptr;
    int   size = expression->atom.name.size;
    int index;
    for (index = 0;  index < size;  index ++) PRINTF("%c", name[index]);
  }
  else if (expression->type == LAMBDA) {
    PRINTF("# ");
    emit(expression->lambda.arguments);
    PRINTF(" ");
    emit(expression->lambda.expression);
  }
  else if (expression->type == LIST) {
    PRINTF("(");
    emit(expression->list.car);
    expression = expression->list.cdr;

    while (isList(expression)) {
      if (expression->list.car != NULL) {
        PRINTF(" ");
        emit(expression->list.car);
      }
      expression = expression->list.cdr;
    }
    PRINTF(")");
  }
  else if (expression->type == PRIMITIVE) {
    PRINTF("primitive");
  }
  else {
    PRINTF("unknown");
  }
}

/* ------------------------------------------------------------------------- */
