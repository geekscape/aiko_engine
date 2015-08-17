/**
 * Please do not remove the following notices.
 *
 * \file       interface.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2014-2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#include "aiko_engine.h"
#include "lisp.h"

uint8_t  *aikoReaderBuffer;
uint16_t  aikoReaderBufferLength;
uint16_t  aikoReaderBufferIndex;

tExpression *aikoEnvironment = NULL;

/* ------------------------------------------------------------------------- */

int ATTRIBUTES
fileGetC() {
  int ch = EOF;

  if (aikoReaderBufferIndex < aikoReaderBufferLength) {
    ch = aikoReaderBuffer[aikoReaderBufferIndex ++];
  }

  return(ch);
}

void ATTRIBUTES
fileUngetC(
  int ch) {

  if (aikoReaderBufferIndex > 0) aikoReaderBufferIndex --;
}

int ATTRIBUTES
fileIsEmpty() {
  return(aikoReaderBufferIndex == aikoReaderBufferLength);
}

tReader aikoBufferReader = { fileGetC, fileIsEmpty, fileUngetC };

/* ------------------------------------------------------------------------- */

tExpression ATTRIBUTES
*lisp_initialize(void) {
  mmem_init();  // Lisp engine memory management

#ifdef AIKO_DEBUG
  printf(
    "Heap memory: %d, Expression memory: %lu, sizeof: %lu, available: %d\n",
    avail_memory, sizeof(aikoExpressions), sizeof(tExpression), AIKO_EXPRESSION_LIMIT
  );
#endif

  aikoError       = AIKO_ERROR_NONE;
  aikoEnvironment = aikoExpressionInitialize();
  return(aikoEnvironment);
}

uint8_t ATTRIBUTES
lisp_message_handler(
  uint8_t  *message,
  uint16_t  length) {

  uint8_t handled = AIKO_NOT_HANDLED;

  aikoError = AIKO_ERROR_NONE;

  aikoReaderBuffer        = message;
  aikoReaderBufferLength  = length;
  aikoReaderBufferIndex   = 0;

  tExpression *expression = aikoParse(& aikoBufferReader);

  if (aikoError != AIKO_ERROR_NONE) {
//  printf("Parse error: %d\n", aikoError);
  }
  else {
    expression = aikoEvaluate(expression, aikoEnvironment);

    if (expression == NULL) {
      printf("Evaluate error: %d\n", aikoError);
    }
    else {
      handled = AIKO_HANDLED;
      aikoEmit(expression);
      printf("\n");
    }
  }

  aikoReset(aikoExpressionBookmark);         // TODO: Breaks "primitiveLabel()"

//return(aikoError);
  return(handled);
}

/* ------------------------------------------------------------------------- */
