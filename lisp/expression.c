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

#include "aiko_compatibility.h"
#include "lisp.h"

uint8_t aikoError = AIKO_ERROR_NONE;

uint16_t aikoExpressionCurrent  = 0;
uint16_t aikoExpressionBookmark = 0;

tExpression aikoExpressions[AIKO_EXPRESSION_LIMIT];

tExpression *nil;
tExpression *parenthesisOpen;
tExpression *parenthesisClose;
tExpression *truth;

/* ------------------------------------------------------------------------- */
tExpression ATTRIBUTES
*aikoAllocateExpression(
  tType   type,
  uint8_t extra) {

  tExpression *expression = NULL;

  if (aikoExpressionCurrent + extra >= AIKO_EXPRESSION_LIMIT) {
    printf("aikoAllocateExpression(): Error: No more expressions available\n");
    aikoError = AIKO_ERROR_LIMIT_EXPRESSIONS;
  }
  else {
    expression = & aikoExpressions[aikoExpressionCurrent ++];
    expression->type = type;
  }

  return(expression);
}

tExpression ATTRIBUTES
*aikoCreateAtom(
  char    *name,
  uint8_t  size) {

  tExpression *expression = aikoAllocateExpression(ATOM, 0);

  if (expression != NULL) {
    if (mmem_alloc(& expression->atom.name, size)) {
      memcpy(expression->atom.name.ptr, name, size);
    }
    else {
      printf("aikoCreateAtom(): Error: No more memory available\n");
      aikoExpressionCurrent --;
      expression = NULL;
      aikoError = AIKO_ERROR_LIMIT_MEMORY;
    }
  }

  return(expression);
}

tExpression ATTRIBUTES
*aikoCreateLambda(
  tExpression *arguments,
  tExpression *expression) {

  tExpression *lambdaExpression = aikoAllocateExpression(LAMBDA, 0);

  if (lambdaExpression != NULL) {
    lambdaExpression->lambda.arguments  = arguments;
    lambdaExpression->lambda.expression = expression;
  }

  return(lambdaExpression);
}

tExpression ATTRIBUTES
*aikoCreateList(
  tExpression *car,
  tExpression *cdr) {

  tExpression *expression = aikoAllocateExpression(LIST, 0);

  if (expression != NULL) {
    expression->list.car = car;
    expression->list.cdr = cdr;
  }

  return(expression);
}

tExpression ATTRIBUTES
*aikoCreatePrimitive(
  char        *name,
  tExpression *(*handler)(tExpression *, tExpression *)) {

  tExpression *expression = NULL;
  tExpression *primitiveName = aikoCreateAtom(name, strlen(name));

  if (primitiveName != NULL) {
    expression = aikoAllocateExpression(PRIMITIVE, 2);

    if (expression != NULL) {
      expression->primitive.handler = handler;
      tExpression *primitiveList = aikoCreateList(expression, NULL);
      expression = NULL;

      if (primitiveList != NULL) {
        expression = aikoCreateList(primitiveName, primitiveList);
      }
    }
  }

  return(expression);
}

void ATTRIBUTES
aikoAppend(
  tExpression *expression,
  tExpression *appendee) {

  while (expression->list.cdr != NULL) expression = expression->list.cdr;

  expression->list.cdr = aikoCreateList(appendee, NULL);
}

uint8_t ATTRIBUTES
aikoIsAtom(
  tExpression *expression,
  char        *name,
  uint8_t      size) {

  if (expression == NULL  ||  expression->type != ATOM) return(0);
  if (expression->atom.name.size != size) return(0);
  return(memcmp(expression->atom.name.ptr, name, size) == 0);
}

uint8_t ATTRIBUTES
aikoIsList(
  tExpression *expression) {

  return(expression != NULL  &&  expression->type == LIST);
}

/* ------------------------------------------------------------------------- */
tExpression ATTRIBUTES
*aikoLookup(
  tExpression *expression,
  tExpression *environment) {

  char    *name = (char *) expression->atom.name.ptr;
  uint8_t  size = expression->atom.name.size;

  tExpression *result = NULL;

  while (aikoIsList(environment)) {
    tExpression *item = environment->list.car;

    if (aikoIsAtom(item->list.car, name, size)) {
      result = item->list.cdr->list.car;
      break;
    }

    environment = environment->list.cdr;
  }

  return(result);
}

tExpression ATTRIBUTES
*aikoEvaluateFunction(
  tExpression *expression,
  tExpression *environment) {

  tExpression *symbol = expression->list.car;

  if (symbol->type == LAMBDA) {
    expression = aikoPrimitiveLambda(expression, environment);
  }
  else if (symbol->type == PRIMITIVE) {
    expression = expression->list.cdr;
    expression = ((symbol->primitive.handler)(expression, environment));
  }

  return(expression);
}

tExpression ATTRIBUTES
*aikoEvaluate(
  tExpression *expression,
  tExpression *environment) {

  if (expression == NULL) {
    expression = nil;
  }
  else if (expression->type == LIST) {
    if (aikoIsAtom(expression->list.car, "lambda", 6)) {// TODO: Use Lamdba atom
      tExpression *lambda = expression->list.cdr;
      expression =
        aikoCreateLambda(lambda->list.car, lambda->list.cdr->list.car);
    }
    else {
      tExpression *result =
        aikoCreateList(aikoEvaluate(expression->list.car, environment), NULL);

      if (result != NULL) {
        expression = expression->list.cdr;

        while(aikoIsList(expression)) {
          if (expression->list.car != NULL) {
            aikoAppend(result, aikoEvaluate(expression->list.car, environment));
          }

          expression = expression->list.cdr;
        }

        expression = aikoEvaluateFunction(result, environment);
      }
    }
  }
  else {
    tExpression *value = aikoLookup(expression, environment);
    expression = (value != NULL)  ?  value  :  expression;
  }

  return(expression);
}

/* ------------------------------------------------------------------------- */
void ATTRIBUTES
aikoEmit(
  tExpression *expression) {

  if (expression == NULL) {
//  printf("NULL");
  }
  else if (expression->type == ATOM) {
    uint8_t name[AIKO_ATOM_SIZE_LIMIT + 1];
    uint8_t size = expression->atom.name.size;
    memcpy(name, expression->atom.name.ptr, size);
    name[size] = 0x00;
    printf("%d:%s", size, name);
  }
  else if (expression->type == LAMBDA) {
    printf("LAMBDA ");
    aikoEmit(expression->lambda.arguments);
    printf(" ");
    aikoEmit(expression->lambda.expression);
  }
  else if (expression->type == LIST) {
    printf("(");
    aikoEmit(expression->list.car);
    expression = expression->list.cdr;

    while (aikoIsList(expression)) {
      if (expression->list.car != NULL) {
//      printf(" ");
        aikoEmit(expression->list.car);
      }
      expression = expression->list.cdr;
    }
    printf(")");
  }
  else if (expression->type == PRIMITIVE) {
    printf("PRIMITIVE");
  }
  else {
    printf("UNKNOWN");
  }
}

/* ------------------------------------------------------------------------- */
tExpression ATTRIBUTES
*aikoExpressionInitialize(void) {
  nil              = aikoCreateList(NULL, NULL);
  parenthesisOpen  = aikoCreateAtom("(",  1);
  parenthesisClose = aikoCreateAtom(")",  1);
  truth            = aikoCreateAtom("#t", 2);

  tExpression *environment =
       aikoCreateList(aikoCreatePrimitive("quote",  aikoPrimitiveQuote), NULL);
  aikoAppend(environment, aikoCreatePrimitive("car",    aikoPrimitiveCar));
  aikoAppend(environment, aikoCreatePrimitive("cdr",    aikoPrimitiveCdr));
  aikoAppend(environment, aikoCreatePrimitive("cons",   aikoPrimitiveCons));
  aikoAppend(environment, aikoCreatePrimitive("equal",  aikoPrimitiveEqual));
  aikoAppend(environment, aikoCreatePrimitive("atom",   aikoPrimitiveAtom));
  aikoAppend(environment, aikoCreatePrimitive("cond",   aikoPrimitiveCond));
  aikoAppend(environment, aikoCreatePrimitive("lambda", aikoPrimitiveLambda));
  aikoAppend(environment, aikoCreatePrimitive("label",  aikoPrimitiveLabel));

  aikoExpressionBookmark = aikoExpressionCurrent;
  return(environment);
}

void ATTRIBUTES
aikoReset(
  uint16_t expressionIndex) {

  while (aikoExpressionCurrent > aikoExpressionBookmark) {
    tExpression *expression = & aikoExpressions[-- aikoExpressionCurrent];
    if (expression->type == ATOM) mmem_free(& expression->atom.name);
  }
}

/* ------------------------------------------------------------------------- */
