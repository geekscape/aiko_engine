/**
 * Please do not remove the following notices.
 *
 * \file       lisp.h
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2014-2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * Dedication
 * ~~~~~~~~~~
 * This little Lisp implementation is dedicated to Steve Russell ...
 * First Lisp implementations, SpaceWar and MIT TMRC (first proto-hackerspace)
 * - https://en.wikipedia.org/wiki/Steve_Russell
 * - http://henryjenkins.org/2007/10/spacewars_and_beyond_how_the_t.html
 * - http://venturebeat.com/2011/01/12/fifty-years-later-video-game-pioneer-steve-russell-demos-spacewar-video-interview
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#pragma once

#ifdef ARDUINO
#include "vendor/aiko_engine/include/aiko_compatibility.h"
#else
#include "aiko_compatibility.h"
#endif

// Bare minimum heap for Lisp environment is 68 bytes, so start with 80 bytes.
// Wi-Fi configuration requires around +100 bytes and +20 expressions.

#ifndef MMEM_CONF_SIZE          // Makefile may have overridden these constants
#ifdef ARDUINO
#define MMEM_CONF_SIZE   80                                      // Minimum: 80
#elif __ets__
#define MMEM_CONF_SIZE  256
#else
#define MMEM_CONF_SIZE  256
#endif
#endif

#include "memory/mmem.h"
extern unsigned int avail_memory;

#ifndef NULL
#define NULL 0
#endif

#ifndef LISP_DEBUG
#define LISP_DEBUG  0
#endif

#define LISP_WRITER_BUFFER_SIZE  256                             // interface.c

/* ------------------------------------------------------------------------- */

typedef enum {
  LISP_ERROR_NONE,
  LISP_ERROR_END_OF_FILE,
  LISP_ERROR_LIMIT_EXPRESSIONS,
  LISP_ERROR_LIMIT_MEMORY,
  LISP_ERROR_LIMIT_TOKEN,
  LISP_ERROR_PARSE_TOKEN,
  LISP_ERROR_LIMIT_EMIT
}
  tLispError;

extern tLispError  lispError;                                    // interface.c
extern char       *lispErrorMessage;                             // ditto

/* ------------------------------------------------------------------------- */

typedef struct {
  uint8_t (*getCh)();
  uint8_t (*isFileEOF)();
  void    (*unGetCh)(uint8_t);
}
  tReader;

/* ------------------------------------------------------------------------- */

#ifdef ARDUINO
static const uint8_t LISP_ATOM_SIZE_LIMIT = 10;
#else
static const uint8_t LISP_ATOM_SIZE_LIMIT = 64;       // Wi-Fi password maximum
#endif

// Bare minimum expression count for Lisp environment is 72.

#ifdef ARDUINO
#define LISP_EXPRESSION_LIMIT   80                               // Minimum: 72
#elif __ets__
#define LISP_EXPRESSION_LIMIT  128
#else
#define LISP_EXPRESSION_LIMIT  128
#endif

typedef enum {
  ATOM,
  LAMBDA,
  LIST,
  PRIMITIVE
}
  tType;

typedef struct sExpression {
  tType type;

  union {
    struct {
      struct mmem name;
    }
      atom;

    struct {
      struct sExpression *arguments;
      struct sExpression *expression;
    }
      lambda;

    struct {
      struct sExpression *car;
      struct sExpression *cdr;
    }
      list;

    struct {
      struct sExpression *(*handler)(struct sExpression*, struct sExpression*);
    }
      primitive;
  };
}
  tExpression;              // Intel: 32 bytes, ESP8266: 16 bytes, AVR: 8 bytes

extern uint8_t      lispDebug;                                  // interface.c

extern uint16_t     lispExpressionCurrent;                      // expression.c
extern uint16_t     lispExpressionBookmark;                     // expression.c
extern tExpression  lispExpressions[LISP_EXPRESSION_LIMIT];     // expression.c

extern tExpression *nil;                                        // expression.c
extern tExpression *parenthesisOpen;                            // expression.c
extern tExpression *parenthesisClose;                           // expression.c
extern tExpression *truth;                                      // expression.c

/* ------------------------------------------------------------------------- */

tExpression *lisp_initialize(uint8_t debugFlag);
uint8_t      lisp_message_handler(
               void *aiko_stream, uint8_t *message, uint16_t length
             );

void         lispAppend(tExpression *expression, tExpression *appendee);
tExpression *lispCreateAtom(const char* name, uint8_t size);
tExpression *lispCreateLambda(tExpression *arguments, tExpression *expression);
tExpression *lispCreateList(tExpression *car, tExpression *cdr);
tExpression *lispCreatePrimitive(
               const char *name,
               tExpression *(*handler)(tExpression *, tExpression *)
             );

uint16_t     lispEmit(tExpression *expression, uint8_t *output, uint16_t size);
tExpression *lispEvaluate(tExpression *expression, tExpression *environment);
tExpression *lispExpressionInitialize(void);
uint8_t      lispIsAtom(
               tExpression *expression, const char *name, uint8_t size
             );
uint8_t      lispIsList(tExpression *expression);
tExpression *lispParse(tReader *reader);

tExpression *lispPrimitiveAtom(
               tExpression *expression, tExpression *environment
             );
tExpression *lispPrimitiveCar(
               tExpression *expression, tExpression *environment
             );
tExpression *lispPrimitiveCdr(
               tExpression *expression, tExpression *environment
             );
tExpression *lispPrimitiveCond(
               tExpression *expression, tExpression *environment
             );
tExpression *lispPrimitiveCons(
               tExpression *expression, tExpression *environment
             );
tExpression *lispPrimitiveEqual(
               tExpression *expression, tExpression *environment
             );
tExpression *lispPrimitiveLambda(
               tExpression *expression, tExpression *environment
             );
tExpression *lispPrimitiveLabel(
               tExpression *expression, tExpression *environment
             );
tExpression *lispPrimitiveQuote(
               tExpression *expression, tExpression *environment
             );

void         lispReset(uint16_t expressionIndex);

int32_t      lispExpressionToInteger(tExpression *expression);
uint8_t      lispExpressionToString(
               tExpression *expression, void *output, uint8_t size
             );
uint8_t      lispIntegerToString(uint8_t value, uint8_t *output, uint8_t size);

/* ------------------------------------------------------------------------- */
