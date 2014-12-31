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

#include "aiko_engine.h"

/* ------------------------------------------------------------------------- */

tExpression *aikoPrimitiveAtom(
  tExpression *expression,
  tExpression *environment) {

  return(expression->list.car->type == ATOM  ?  truth  :  nil);
}

tExpression *aikoPrimitiveCar(
  tExpression *expression,
  tExpression *environment) {

  return(expression->list.car->list.car);
}

tExpression *aikoPrimitiveCdr(
  tExpression *expression,
  tExpression *environment) {

  return(expression->list.car->list.cdr);
}

tExpression *aikoPrimitiveCond(
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

tExpression *aikoPrimitiveCons(
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

tExpression *aikoPrimitiveEqual(
  tExpression *expression,
  tExpression *environment) {

  char *name = (char *) expression->list.car->atom.name.ptr;
  int   size = expression->list.car->atom.name.size;

  return(aikoIsAtom(expression->list.cdr->list.car, name, size) ? truth : nil);
}

tExpression *aikoInterleave(                   // TODO: Refactor duplicate code
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

tExpression *aikoReplace(                      // TODO: Refactor duplicate code
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

      char *name = (char *) expression->atom.name.ptr;
      int   size = expression->atom.name.size;

      if (aikoIsAtom(atom, name, size)) return(item->list.cdr->list.car);

      replacement = replacement->list.cdr;
    }

    return(expression);
  }
}

tExpression *aikoPrimitiveLambda(
  tExpression *expression,
  tExpression *environment) {

  tExpression *lambda = expression->list.car;
  tExpression *values = expression->list.cdr;

  tExpression *arguments = aikoInterleave(lambda->lambda.arguments, values);
  tExpression *result    = aikoReplace(lambda->lambda.expression, arguments);

  return(aikoEvaluate(result, environment));
}

tExpression *aikoPrimitiveLabel(
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

tExpression *aikoPrimitiveQuote(
  tExpression *expression,
  tExpression *environment) {

  return(expression->list.car);
}

/* ------------------------------------------------------------------------- */
