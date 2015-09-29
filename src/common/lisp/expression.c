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
#include "vendor/aiko_engine/include/aiko_engine.h"
#include "vendor/aiko_engine/include/lisp.h"
#else
#include "aiko_engine.h"
#include "lisp.h"
#endif

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
    lispError = LISP_ERROR_LIMIT_EXPRESSIONS;
    lispErrorMessage = "(5:error16:expression limit)\r\n";
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
      lispExpressionCurrent --;
      expression = NULL;
      lispError = LISP_ERROR_LIMIT_MEMORY;
      lispErrorMessage = "(5:error12:memory limit)\r\n";
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
            if (lispError) return(NULL);
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
uint16_t ATTRIBUTES
lispEmit(
  tExpression *expr,
  uint8_t     *output,
  uint16_t     remain) {

  const uint8_t lisp_atom_size_width = 2;
  uint16_t used = 0;

  if (expr == NULL) {
//  if ((used = remain >= 5 ? 5 : 0)) memcpy(output, "3:nil", used);
    return(used);
  }
  else if (expr->type == ATOM) {
    uint8_t size = expr->atom.name.size;
    if (remain >= (lisp_atom_size_width + size + 1)) {
      used = lispIntegerToString(size, output, lisp_atom_size_width);
      output[used ++] = ':';
      memcpy(& output[used], expr->atom.name.ptr, size);
      used += size;
    }
  }
  else if (expr->type == LAMBDA) {
    if ((used = remain >= 10 ? 9 : 0)) {
      memcpy(output, "(6:lambda", used);
      used += lispEmit(expr->lambda.arguments, & output[used], remain - used);
      if (lispError == LISP_ERROR_NONE) {
        used += lispEmit(expr->lambda.expression, & output[used], remain-used);
        if (lispError == LISP_ERROR_NONE) output[used ++] = ')';
      }
    }
  }
  else if (expr->type == LIST) {
    if (remain >= 2) {
      output[used ++] = '(';
      used += lispEmit(expr->list.car, & output[used], remain - used);
      if (lispError == LISP_ERROR_NONE) {
        expr = expr->list.cdr;

        while (lispIsList(expr)) {
          if (expr->list.car != NULL) {
            used += lispEmit(expr->list.car, & output[used], remain - used);
            if (lispError) break;
          }
          expr = expr->list.cdr;
        }
        if (lispError == LISP_ERROR_NONE) output[used ++] = ')';
      }
    }
  }
  else if (expr->type == PRIMITIVE) {
    if ((used = remain >= 13 ? 13 : 0)) memcpy(output, "(9:primitive)", used);
  }
  else {
    if ((used = remain >= 11 ? 11 : 0)) memcpy(output, "(7:unknown)", used);
  }

  if (used == 0) {
    lispError = LISP_ERROR_LIMIT_EMIT;
    lispErrorMessage = "(5:error17:emit output limit)\r\n";
  }

  return(used);
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
