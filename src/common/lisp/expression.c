/**
 * Please do not remove the following notices.
 *
 * \file       expression.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2014-2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#ifdef ARDUINO
#include "vendor/aiko_engine/include/aiko_compatibility.h"
#include "vendor/aiko_engine/include/lisp.h"
#else
#include "aiko_compatibility.h"
#include "lisp.h"
#endif

uint8_t lispError = LISP_ERROR_NONE;

uint16_t lispExpressionCurrent  = 0;
uint16_t lispExpressionBookmark = 0;

tExpression lispExpressions[LISP_EXPRESSION_LIMIT];

tExpression *nil;
tExpression *parenthesisOpen;
tExpression *parenthesisClose;
tExpression *truth;

/* ------------------------------------------------------------------------- */
tExpression ATTRIBUTES
*lispAllocateExpression(
  tType   type,
  uint8_t extra) {

  tExpression *expression = NULL;

  if (lispExpressionCurrent + extra >= LISP_EXPRESSION_LIMIT) {
    PRINTLN("lispAllocateExpression(): Error: No more expressions available");
    lispError = LISP_ERROR_LIMIT_EXPRESSIONS;
  }
  else {
    expression = & lispExpressions[lispExpressionCurrent ++];
    expression->type = type;
  }

  return(expression);
}

tExpression ATTRIBUTES
*lispCreateAtom(
  const char *name,
  uint8_t     size) {

  tExpression *expression = lispAllocateExpression(ATOM, 0);

  if (expression != NULL) {
    if (mmem_alloc(& expression->atom.name, size)) {
      memcpy(expression->atom.name.ptr, name, size);
    }
    else {
      PRINTLN("lispCreateAtom(): Error: No more memory available");
      lispExpressionCurrent --;
      expression = NULL;
      lispError = LISP_ERROR_LIMIT_MEMORY;
    }
  }

  return(expression);
}

tExpression ATTRIBUTES
*lispCreateLambda(
  tExpression *arguments,
  tExpression *expression) {

  tExpression *lambdaExpression = lispAllocateExpression(LAMBDA, 0);

  if (lambdaExpression != NULL) {
    lambdaExpression->lambda.arguments  = arguments;
    lambdaExpression->lambda.expression = expression;
  }

  return(lambdaExpression);
}

tExpression ATTRIBUTES
*lispCreateList(
  tExpression *car,
  tExpression *cdr) {

  tExpression *expression = lispAllocateExpression(LIST, 0);

  if (expression != NULL) {
    expression->list.car = car;
    expression->list.cdr = cdr;
  }

  return(expression);
}

tExpression ATTRIBUTES
*lispCreatePrimitive(
  const char  *name,
  tExpression *(*handler)(tExpression *, tExpression *)) {

  tExpression *expression = NULL;
  tExpression *primitiveName = lispCreateAtom(name, strlen(name));

  if (primitiveName != NULL) {
    expression = lispAllocateExpression(PRIMITIVE, 2);

    if (expression != NULL) {
      expression->primitive.handler = handler;
      tExpression *primitiveList = lispCreateList(expression, NULL);
      expression = NULL;

      if (primitiveList != NULL) {
        expression = lispCreateList(primitiveName, primitiveList);
      }
    }
  }

  return(expression);
}

void ATTRIBUTES
lispAppend(
  tExpression *expression,
  tExpression *appendee) {

  while (expression->list.cdr != NULL) expression = expression->list.cdr;

  expression->list.cdr = lispCreateList(appendee, NULL);
}

uint8_t ATTRIBUTES
lispIsAtom(
  tExpression *expression,
  const char  *name,
  uint8_t      size) {

  if (expression == NULL  ||  expression->type != ATOM) return(0);
  if (expression->atom.name.size != size) return(0);
  return(memcmp(expression->atom.name.ptr, name, size) == 0);
}

uint8_t ATTRIBUTES
lispIsList(
  tExpression *expression) {

  return(expression != NULL  &&  expression->type == LIST);
}

/* ------------------------------------------------------------------------- */
tExpression ATTRIBUTES
*lispLookup(
  tExpression *expression,
  tExpression *environment) {

  char    *name = (char *) expression->atom.name.ptr;
  uint8_t  size = expression->atom.name.size;

  tExpression *result = NULL;

  while (lispIsList(environment)) {
    tExpression *item = environment->list.car;

    if (lispIsAtom(item->list.car, name, size)) {
      result = item->list.cdr->list.car;
      break;
    }

    environment = environment->list.cdr;
  }

  return(result);
}

tExpression ATTRIBUTES
*lispEvaluateFunction(
  tExpression *expression,
  tExpression *environment) {

  tExpression *symbol = expression->list.car;

  if (symbol->type == LAMBDA) {
    expression = lispPrimitiveLambda(expression, environment);
  }
  else if (symbol->type == PRIMITIVE) {
    expression = expression->list.cdr;
    expression = ((symbol->primitive.handler)(expression, environment));
  }

  return(expression);
}

tExpression ATTRIBUTES
*lispEvaluate(
  tExpression *expression,
  tExpression *environment) {

  if (expression == NULL) {
    expression = nil;
  }
  else if (expression->type == LIST) {
    if (lispIsAtom(expression->list.car, "lambda", 6)) {// TODO: Use Lamdba atom
      tExpression *lambda = expression->list.cdr;
      expression =
        lispCreateLambda(lambda->list.car, lambda->list.cdr->list.car);
    }
    else {
      tExpression *result =
        lispCreateList(lispEvaluate(expression->list.car, environment), NULL);

      if (result != NULL) {
        expression = expression->list.cdr;

        while(lispIsList(expression)) {
          if (expression->list.car != NULL) {
            lispAppend(result, lispEvaluate(expression->list.car, environment));
          }

          expression = expression->list.cdr;
        }

        expression = lispEvaluateFunction(result, environment);
      }
    }
  }
  else {
    tExpression *value = lispLookup(expression, environment);
    expression = (value != NULL)  ?  value  :  expression;
  }

  return(expression);
}

/* ------------------------------------------------------------------------- */
void ATTRIBUTES
lispEmit(
  tExpression *expression) {

  if (expression == NULL) {
//  PRINT("NULL");
  }
  else if (expression->type == ATOM) {
    uint8_t name[LISP_ATOM_SIZE_LIMIT + 1];
    uint8_t size = expression->atom.name.size;
    memcpy(name, expression->atom.name.ptr, size);
    name[size] = 0x00;
#ifdef ARDUINO
    Serial.print(size);
    Serial.print(":");
    Serial.print((char *) name);
#else
    printf("%d:%s", size, name);
#endif
  }
  else if (expression->type == LAMBDA) {
    PRINT("LAMBDA ");
    lispEmit(expression->lambda.arguments);
    PRINT(" ");
    lispEmit(expression->lambda.expression);
  }
  else if (expression->type == LIST) {
    PRINT("(");
    lispEmit(expression->list.car);
    expression = expression->list.cdr;

    while (lispIsList(expression)) {
      if (expression->list.car != NULL) {
//      PRINT(" ");
        lispEmit(expression->list.car);
      }
      expression = expression->list.cdr;
    }
    PRINT(")");
  }
  else if (expression->type == PRIMITIVE) {
    PRINT("PRIMITIVE");
  }
  else {
    PRINT("UNKNOWN");
  }
}

/* ------------------------------------------------------------------------- */
tExpression ATTRIBUTES
*lispExpressionInitialize(void) {
  nil              = lispCreateList(NULL, NULL);
  parenthesisOpen  = lispCreateAtom("(",  1);
  parenthesisClose = lispCreateAtom(")",  1);
  truth            = lispCreateAtom("#t", 2);

  tExpression *environment =
       lispCreateList(lispCreatePrimitive("quote",  lispPrimitiveQuote), NULL);
  lispAppend(environment, lispCreatePrimitive("car",    lispPrimitiveCar));
  lispAppend(environment, lispCreatePrimitive("cdr",    lispPrimitiveCdr));
  lispAppend(environment, lispCreatePrimitive("cons",   lispPrimitiveCons));
  lispAppend(environment, lispCreatePrimitive("equal",  lispPrimitiveEqual));
  lispAppend(environment, lispCreatePrimitive("atom",   lispPrimitiveAtom));
  lispAppend(environment, lispCreatePrimitive("cond",   lispPrimitiveCond));
  lispAppend(environment, lispCreatePrimitive("lambda", lispPrimitiveLambda));
  lispAppend(environment, lispCreatePrimitive("label",  lispPrimitiveLabel));

  lispExpressionBookmark = lispExpressionCurrent;
  return(environment);
}

void ATTRIBUTES
lispReset(
  uint16_t expressionIndex) {

  while (lispExpressionCurrent > lispExpressionBookmark) {
    tExpression *expression = & lispExpressions[-- lispExpressionCurrent];
    if (expression->type == ATOM) mmem_free(& expression->atom.name);
  }
}

/* ------------------------------------------------------------------------- */
