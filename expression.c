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

#include "aiko_engine.h"

tExpression aikoExpressions[AIKO_EXPRESSION_LIMIT];

int aikoExpressionCurrent  = 0;
int aikoExpressionBookmark = 0;

tExpression *nil;
tExpression *parenthesisOpen;
tExpression *parenthesisClose;
tExpression *truth;

/* ------------------------------------------------------------------------- */

tExpression *aikoAllocateExpression(
  tType type,
  int   extra) {

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

tExpression *aikoCreateAtom(
  char *name,
  int   size) {

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

tExpression *aikoCreateLambda(
  tExpression *arguments,
  tExpression *expression) {

  tExpression *lambdaExpression = aikoAllocateExpression(LAMBDA, 0);

  if (lambdaExpression != NULL) {
    lambdaExpression->lambda.arguments  = arguments;
    lambdaExpression->lambda.expression = expression;
  }

  return(lambdaExpression);
}

tExpression *aikoCreateList(
  tExpression *car,
  tExpression *cdr) {

  tExpression *expression = aikoAllocateExpression(LIST, 0);

  if (expression != NULL) {
    expression->list.car = car;
    expression->list.cdr = cdr;
  }

  return(expression);
}

tExpression *aikoCreatePrimitive(
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

void aikoAppend(
  tExpression *expression,
  tExpression *appendee) {

  while (expression->list.cdr != NULL) expression = expression->list.cdr;

  expression->list.cdr = aikoCreateList(appendee, NULL);
}

int aikoIsAtom(
  tExpression *expression,
  char        *name,
  int          size) {

  if (expression == NULL  ||  expression->type != ATOM) return(0);
  if (expression->atom.name.size != size) return(0);
  return(memcmp(expression->atom.name.ptr, name, size) == 0);
}

int aikoIsList(
  tExpression *expression) {

  return(expression != NULL  &&  expression->type == LIST);
}

/* ------------------------------------------------------------------------- */

tExpression *aikoLookup(
  tExpression *expression,
  tExpression *environment) {

  char *name = (char *) expression->atom.name.ptr;
  int   size = expression->atom.name.size;

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

tExpression *aikoEvaluateFunction(
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

tExpression *aikoEvaluate(
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

tExpression *aikoExpressionInitialize(void) {
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

void aikoReset(
  int expressionIndex) {

  while (aikoExpressionCurrent > aikoExpressionBookmark) {
    tExpression *expression = & aikoExpressions[-- aikoExpressionCurrent];
    if (expression->type == ATOM) mmem_free(& expression->atom.name);
  }
}

/* ------------------------------------------------------------------------- */
