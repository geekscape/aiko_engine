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

static uint8_t     *aikoReaderBuffer;
static uint16_t     aikoReaderBufferLength;
static uint16_t     aikoReaderBufferIndex;

static tExpression *aikoEnvironment = NULL;

/* ------------------------------------------------------------------------- */

uint8_t ATTRIBUTES
fileGetC() {
  uint8_t ch = EOF;

  if (aikoReaderBufferIndex < aikoReaderBufferLength) {
    ch = aikoReaderBuffer[aikoReaderBufferIndex ++];
  }

  return(ch);
}

void ATTRIBUTES
fileUngetC(
  uint8_t ch) {

  if (aikoReaderBufferIndex > 0) aikoReaderBufferIndex --;
}

uint8_t ATTRIBUTES
fileIsEmpty() {
  return(aikoReaderBufferIndex == aikoReaderBufferLength);
}

tReader aikoBufferReader = { fileGetC, fileIsEmpty, fileUngetC };

/* ------------------------------------------------------------------------- */

tExpression ATTRIBUTES
*lisp_initialize(
  uint8_t debug_flag) {

  lispDebug = debug_flag;

  mmem_init();  // Lisp engine memory management

  if (lispDebug) {
#if ARDUINO
    Serial.print("Heap memory: ");
    Serial.println(avail_memory);
    Serial.print("Expression memory: ");
    Serial.println(sizeof(aikoExpressions));
    Serial.print("Expression sizeof: ");
    Serial.println(sizeof(tExpression));
    Serial.print("Expressions available: ");
    Serial.println(AIKO_EXPRESSION_LIMIT);
#else
    printf(
      "Heap memory: %d, Expression memory: %lu, sizeof: %lu, available: %d\n",
      avail_memory, sizeof(aikoExpressions), sizeof(tExpression), AIKO_EXPRESSION_LIMIT
    );
#endif
  }

  aikoError       = AIKO_ERROR_NONE;
  aikoEnvironment = aikoExpressionInitialize();
  return(aikoEnvironment);
}

/* ------------------------------------------------------------------------- */

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
#ifdef ARDUINO
//  Serial.print("Parse error: ");  Serial.println(aikoError);
#else
//  printf("Parse error: %d\n", aikoError);
#endif
  }
  else {
    expression = aikoEvaluate(expression, aikoEnvironment);

    if (expression == NULL) {
#if ARDUINO
//    Serial.print("Evaluate error: ");  Serial.println(aikoError);
#else
//    printf("Evaluate error: %d\n", aikoError);
#endif
    }
    else {
      handled = AIKO_HANDLED;
      aikoEmit(expression);
      PRINT("\n");
    }
  }

  aikoReset(aikoExpressionBookmark);         // TODO: Breaks "primitiveLabel()"

  if (lispDebug) {
#if ARDUINO
    Serial.print("Heap used: ");
    Serial.println(MMEM_CONF_SIZE - avail_memory);
    Serial.print("Expressions used: ");
    Serial.println(aikoExpressionCurrent);
#else
    printf(
      "Heap used: %d, Expressions used: %d\n",
      MMEM_CONF_SIZE - avail_memory, aikoExpressionCurrent
    );
#endif
  }

//return(aikoError);
  return(handled);
}

/* ------------------------------------------------------------------------- */
