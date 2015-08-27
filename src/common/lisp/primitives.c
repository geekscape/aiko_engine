/**
 * Please do not remove the following notices.
 *
 * \file       primitives.c
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

/* ------------------------------------------------------------------------- */

tExpression ATTRIBUTES
*aikoPrimitiveAtom(
  tExpression *expression,
  tExpression *environment) {

  return(expression->list.car->type == ATOM  ?  truth  :  nil);
}

tExpression ATTRIBUTES
*aikoPrimitiveCar(
  tExpression *expression,
  tExpression *environment) {

  return(expression->list.car->list.car);
}

tExpression ATTRIBUTES
*aikoPrimitiveCdr(
  tExpression *expression,
  tExpression *environment) {

  return(expression->list.car->list.cdr);
}

tExpression ATTRIBUTES
*aikoPrimitiveCond(
  tExpression *expression,
  tExpression *environment) {

  while (aikoIsList(expression)) {
    tExpression *list = expression->list.car;
    tExpression *predicate = nil;

    if (list->list.car != nil) {
      predicate = aikoEvaluate(list->list.car, environment);
    }

    tExpression *result = list->list.cdr->list.car;

    if (predicate != nil) return(aikoEvaluate(result, environment));

    expression = expression->list.cdr;
  }

  return(nil);
}

tExpression ATTRIBUTES
*aikoPrimitiveCons(
  tExpression *expression,
  tExpression *environment) {

  tExpression *list = aikoCreateList(expression->list.car, NULL);

  if (list != NULL) {
    expression = expression->list.cdr->list.car;

    while (aikoIsList(expression)) {
      aikoAppend(list, expression->list.car);
      expression = expression->list.cdr;
    }
  }

  return(list);
}

tExpression ATTRIBUTES
*aikoPrimitiveEqual(
  tExpression *expression,
  tExpression *environment) {

  char    *name = (char *) expression->list.car->atom.name.ptr;
  uint8_t  size = expression->list.car->atom.name.size;

  return(aikoIsAtom(expression->list.cdr->list.car, name, size) ? truth : nil);
}

tExpression ATTRIBUTES
*aikoInterleave(                   // TODO: Refactor duplicate code
  tExpression *list1,
  tExpression *list2) {

  tExpression *result = aikoCreateList(
    aikoCreateList(list1->list.car, aikoCreateList(list2->list.car, NULL)),
    NULL
  );

  list1 = list1->list.cdr;
  list2 = list2->list.cdr;

  while (aikoIsList(list1)) {
    aikoAppend(
      result,
      aikoCreateList(list1->list.car, aikoCreateList(list2->list.car, NULL))
    );

    list1 = list1->list.cdr;
    list2 = list2->list.cdr;
  }

  return(result);
}

tExpression ATTRIBUTES
*aikoReplace(                                  // TODO: Refactor duplicate code
  tExpression *expression,
  tExpression *replacement) {

  if (aikoIsList(expression)) {
    tExpression *result = aikoCreateList(
      aikoReplace(expression->list.car, replacement), NULL
    );

    expression = expression->list.cdr;

    while (aikoIsList(expression)) {
      aikoAppend(result, aikoReplace(expression->list.car, replacement));
      expression = expression->list.cdr;
    }

    return(result);
  }
  else {
    while (aikoIsList(replacement)) {
      tExpression *item = replacement->list.car;
      tExpression *atom = item->list.car;

      char    *name = (char *) expression->atom.name.ptr;
      uint8_t  size = expression->atom.name.size;

      if (aikoIsAtom(atom, name, size)) return(item->list.cdr->list.car);

      replacement = replacement->list.cdr;
    }

    return(expression);
  }
}

tExpression ATTRIBUTES
*aikoPrimitiveLambda(
  tExpression *expression,
  tExpression *environment) {

  tExpression *lambda = expression->list.car;
  tExpression *values = expression->list.cdr;

  tExpression *arguments = aikoInterleave(lambda->lambda.arguments, values);
  tExpression *result    = aikoReplace(lambda->lambda.expression, arguments);

  return(aikoEvaluate(result, environment));
}

tExpression ATTRIBUTES
*aikoPrimitiveLabel(
  tExpression *expression,
  tExpression *environment) {

  tExpression *labelName = aikoCreateAtom(
    (char *) expression->list.car->atom.name.ptr,  // TODO: Check argument type
    expression->list.car->atom.name.size
  );

  aikoAppend(                             // TODO: Replace existing "labelName"
    environment,
    aikoCreateList(
      labelName, aikoCreateList(expression->list.cdr->list.car, NULL)
    )
  );

  aikoExpressionBookmark = aikoExpressionCurrent;  // TODO: Memory management ?
  return(truth);
}

tExpression ATTRIBUTES
*aikoPrimitiveQuote(
  tExpression *expression,
  tExpression *environment) {

  return(expression->list.car);
}

/* ------------------------------------------------------------------------- */
