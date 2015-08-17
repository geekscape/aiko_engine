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
extern unsigned int avail_memory;

//#define AIKO_DEBUG

#ifndef NULL
#define NULL 0
#endif

/* ------------------------------------------------------------------------- */

#define AIKO_ERROR_NONE               0
#define AIKO_ERROR_END_OF_FILE        1
#define AIKO_ERROR_LIMIT_EXPRESSIONS  2
#define AIKO_ERROR_LIMIT_MEMORY       3
#define AIKO_ERROR_LIMIT_TOKEN        4
#define AIKO_ERROR_PARSE_TOKEN        5

extern int aikoError;

/* ------------------------------------------------------------------------- */

typedef struct {
  int  (*getCh)();
  int  (*isFileEOF)();
  void (*unGetCh)(int);
}
  tReader;

/* ------------------------------------------------------------------------- */

static const int AIKO_ATOM_SIZE_LIMIT = 10;

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

extern int aikoExpressionCurrent;
extern int aikoExpressionBookmark;

extern tExpression  aikoExpressions[AIKO_EXPRESSION_LIMIT];

extern tExpression *nil;
extern tExpression *parenthesisOpen;
extern tExpression *parenthesisClose;
extern tExpression *truth;

/* ------------------------------------------------------------------------- */

tExpression *lisp_initialize(void);
uint8_t      lisp_message_handler(uint8_t *message, uint16_t length);

void         aikoAppend(tExpression *expression, tExpression *appendee);
tExpression *aikoCreateAtom(char* name, int size);
tExpression *aikoCreateLambda(tExpression *arguments, tExpression *expression);
tExpression *aikoCreateList(tExpression *car, tExpression *cdr);
tExpression *aikoCreatePrimitive(
               char *name, tExpression *(*handler)(tExpression *, tExpression *)
             );

void         aikoEmit(tExpression *expression);
tExpression *aikoEvaluate(tExpression *expression, tExpression *environment);
tExpression *aikoExpressionInitialize(void);
int          aikoIsAtom(tExpression *expression, char *name, int size);
int          aikoIsList(tExpression *expression);
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

void         aikoReset(int expressionIndex);

/* ------------------------------------------------------------------------- */
