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

#include "aiko_compatibility.h"

#ifndef MMEM_CONF_SIZE
#define MMEM_CONF_SIZE  80                                       // Minimum: 80
#endif

#include "../memory/mmem.h"
extern uint16_t avail_memory;

#ifndef NULL
#define NULL 0
#endif

#ifndef LISP_DEBUG
#define LISP_DEBUG  0
#endif

/* ------------------------------------------------------------------------- */

#define AIKO_ERROR_NONE               0
#define AIKO_ERROR_END_OF_FILE        1
#define AIKO_ERROR_LIMIT_EXPRESSIONS  2
#define AIKO_ERROR_LIMIT_MEMORY       3
#define AIKO_ERROR_LIMIT_TOKEN        4
#define AIKO_ERROR_PARSE_TOKEN        5

extern uint8_t aikoError;                                       // expression.c

/* ------------------------------------------------------------------------- */

typedef struct {
  uint8_t (*getCh)();
  uint8_t (*isFileEOF)();
  void    (*unGetCh)(uint8_t);
}
  tReader;

/* ------------------------------------------------------------------------- */

static const uint8_t AIKO_ATOM_SIZE_LIMIT = 10;

#ifdef __ets__
#define AIKO_EXPRESSION_LIMIT  80                                // Minimum: 72
#else
#define AIKO_EXPRESSION_LIMIT 512
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

extern uint16_t     aikoExpressionCurrent;                      // expression.c
extern uint16_t     aikoExpressionBookmark;                     // expression.c
extern tExpression  aikoExpressions[AIKO_EXPRESSION_LIMIT];     // expression.c

extern tExpression *nil;                                        // expression.c
extern tExpression *parenthesisOpen;                            // expression.c
extern tExpression *parenthesisClose;                           // expression.c
extern tExpression *truth;                                      // expression.c

/* ------------------------------------------------------------------------- */

tExpression *lisp_initialize(uint8_t debugFlag);
uint8_t      lisp_message_handler(uint8_t *message, uint16_t length);

void         aikoAppend(tExpression *expression, tExpression *appendee);
tExpression *aikoCreateAtom(char* name, uint8_t size);
tExpression *aikoCreateLambda(tExpression *arguments, tExpression *expression);
tExpression *aikoCreateList(tExpression *car, tExpression *cdr);
tExpression *aikoCreatePrimitive(
               char *name, tExpression *(*handler)(tExpression *, tExpression *)
             );

void         aikoEmit(tExpression *expression);
tExpression *aikoEvaluate(tExpression *expression, tExpression *environment);
tExpression *aikoExpressionInitialize(void);
uint8_t      aikoIsAtom(tExpression *expression, char *name, uint8_t size);
uint8_t      aikoIsList(tExpression *expression);
tExpression *aikoParse(tReader *reader);

tExpression *aikoPrimitiveAtom(
               tExpression *expression, tExpression *environment);
tExpression *aikoPrimitiveCar(
               tExpression *expression, tExpression *environment);
tExpression *aikoPrimitiveCdr(
               tExpression *expression, tExpression *environment);
tExpression *aikoPrimitiveCond(
               tExpression *expression, tExpression *environment);
tExpression *aikoPrimitiveCons(
               tExpression *expression, tExpression *environment);
tExpression *aikoPrimitiveEqual(
               tExpression *expression, tExpression *environment);
tExpression *aikoPrimitiveLambda(
               tExpression *expression, tExpression *environment);
tExpression *aikoPrimitiveLabel(
               tExpression *expression, tExpression *environment);
tExpression *aikoPrimitiveQuote(
               tExpression *expression, tExpression *environment);

void         aikoReset(uint16_t expressionIndex);

int32_t      aikoToInteger(tExpression *expression);


/* ------------------------------------------------------------------------- */
