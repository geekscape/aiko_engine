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

#ifdef ARDUINO
#include "vendor/aiko_engine/include/aiko_engine.h"
#include "vendor/aiko_engine/include/lisp.h"
#else
#include "aiko_engine.h"
#include "lisp.h"
#endif

uint8_t lispDebug = 0;

static uint8_t     *lispReaderBuffer;
static uint16_t     lispReaderBufferLength;
static uint16_t     lispReaderBufferIndex;

static tExpression *lispEnvironment = NULL;

/* ------------------------------------------------------------------------- */

uint8_t ATTRIBUTES
fileGetC() {
  uint8_t ch = EOF;

  if (lispReaderBufferIndex < lispReaderBufferLength) {
    ch = lispReaderBuffer[lispReaderBufferIndex ++];
  }

  return(ch);
}

void ATTRIBUTES
fileUngetC(
  uint8_t ch) {

  if (lispReaderBufferIndex > 0) lispReaderBufferIndex --;
}

uint8_t ATTRIBUTES
fileIsEmpty() {
  return(lispReaderBufferIndex == lispReaderBufferLength);
}

tReader lispBufferReader = { fileGetC, fileIsEmpty, fileUngetC };

/* ------------------------------------------------------------------------- */

tExpression ATTRIBUTES
*lisp_initialize(
  uint8_t debug_flag) {

  lispDebug = debug_flag;

  mmem_init();  // Lisp engine memory management

  if (lispDebug) {
#ifdef ARDUINO
    Serial.print("Heap memory: ");
    Serial.println(avail_memory);
    Serial.print("Expression memory: ");
    Serial.println(sizeof(lispExpressions));
    Serial.print("Expression sizeof: ");
    Serial.println(sizeof(tExpression));
    Serial.print("Expressions available: ");
    Serial.println(LISP_EXPRESSION_LIMIT);
#else
    printf(
      "Heap memory: %d, Expression memory: %lu, sizeof: %lu, available: %d\n",
      avail_memory, sizeof(lispExpressions), sizeof(tExpression), LISP_EXPRESSION_LIMIT
    );
#endif
  }

  lispError       = LISP_ERROR_NONE;
  lispEnvironment = lispExpressionInitialize();
  return(lispEnvironment);
}

/* ------------------------------------------------------------------------- */

uint8_t ATTRIBUTES
lisp_message_handler(
  void     *aiko_stream,
  uint8_t  *message,
  uint16_t  length) {

  uint8_t handled = AIKO_NOT_HANDLED;

  lispError = LISP_ERROR_NONE;

  lispReaderBuffer        = message;
  lispReaderBufferLength  = length;
  lispReaderBufferIndex   = 0;

  tExpression *expression = lispParse(& lispBufferReader);

  if (lispError != LISP_ERROR_NONE) {
#ifdef ARDUINO
//  Serial.print("Parse error: ");  Serial.println(lispError);
#else
//  printf("Parse error: %d\n", lispError);
#endif
  }
  else {
    expression = lispEvaluate(expression, lispEnvironment);

    if (expression == NULL) {
#ifdef ARDUINO
//    Serial.print("Evaluate error: ");  Serial.println(lispError);
#else
//    printf("Evaluate error: %d\n", lispError);
#endif
    }
    else {
      handled = AIKO_HANDLED;
      lispEmit(expression);
      PRINT("\n");
    }
  }

  lispReset(lispExpressionBookmark);         // TODO: Breaks "primitiveLabel()"

  if (lispDebug) {
#ifdef ARDUINO
    Serial.print("Heap used: ");
    Serial.println(MMEM_CONF_SIZE - avail_memory);
    Serial.print("Expressions used: ");
    Serial.println(lispExpressionCurrent);
#else
    printf(
      "Heap used: %d, Expressions used: %d\n",
      MMEM_CONF_SIZE - avail_memory, lispExpressionCurrent
    );
#endif
  }

//return(lispError);
  return(handled);
}

/* ------------------------------------------------------------------------- */
