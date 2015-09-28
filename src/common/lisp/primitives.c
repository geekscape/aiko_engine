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
 * - All primitives should check for valid arguments, e.g (3:car) fails.
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
*lispPrimitiveAtom(
  tExpression *expression,
  tExpression *environment) {

  return(expression->list.car->type == ATOM  ?  truth  :  nil);
}

tExpression ATTRIBUTES
*lispPrimitiveCar(
  tExpression *expression,
  tExpression *environment) {

  return(expression->list.car->list.car);
}

tExpression ATTRIBUTES
*lispPrimitiveCdr(
  tExpression *expression,
  tExpression *environment) {

  return(expression->list.car->list.cdr);
}

tExpression ATTRIBUTES
*lispPrimitiveCond(
  tExpression *expression,
  tExpression *environment) {

  while (lispIsList(expression)) {
    tExpression *list = expression->list.car;
    tExpression *predicate = nil;

    if (list->list.car != nil) {
      predicate = lispEvaluate(list->list.car, environment);
    }

    tExpression *result = list->list.cdr->list.car;

    if (predicate != nil) return(lispEvaluate(result, environment));

    expression = expression->list.cdr;
  }

  return(nil);
}

tExpression ATTRIBUTES
*lispPrimitiveCons(
  tExpression *expression,
  tExpression *environment) {

  tExpression *list = lispCreateList(expression->list.car, NULL);

  if (list != NULL) {
    expression = expression->list.cdr->list.car;

    while (lispIsList(expression)) {
      lispAppend(list, expression->list.car);
      expression = expression->list.cdr;
    }
  }

  return(list);
}

tExpression ATTRIBUTES
*lispPrimitiveEqual(
  tExpression *expression,
  tExpression *environment) {

  char    *name = (char *) expression->list.car->atom.name.ptr;
  uint8_t  size = expression->list.car->atom.name.size;

  return(lispIsAtom(expression->list.cdr->list.car, name, size) ? truth : nil);
}

tExpression ATTRIBUTES
*lispInterleave(                               // TODO: Refactor duplicate code
  tExpression *list1,
  tExpression *list2) {

  tExpression *result  = NULL;
  tExpression *newList = lispCreateList(list2->list.car, NULL);

  if (newList != NULL) {
    newList = lispCreateList(list1->list.car, newList);

    if (newList != NULL) {
      result = lispCreateList(newList, NULL);

      if (result != NULL) {
        list1 = list1->list.cdr;
        list2 = list2->list.cdr;

        while (lispIsList(list1)) {
          newList = lispCreateList(list2->list.car, NULL);
          if (newList == NULL) break;

          newList = lispCreateList(list1->list.car, newList);
          if (newList == NULL) break;

          lispAppend(result, newList);

          list1 = list1->list.cdr;
          list2 = list2->list.cdr;
        }
      }
    }
  }

  return(result);
}

tExpression ATTRIBUTES
*lispReplace(                                  // TODO: Refactor duplicate code
  tExpression *expression,
  tExpression *replacement) {

  if (lispIsList(expression)) {
    tExpression *result = lispCreateList(
      lispReplace(expression->list.car, replacement), NULL
    );

    expression = expression->list.cdr;

    while (lispIsList(expression)) {
      lispAppend(result, lispReplace(expression->list.car, replacement));
      expression = expression->list.cdr;
    }

    return(result);
  }
  else {
    while (lispIsList(replacement)) {
      tExpression *item = replacement->list.car;
      tExpression *atom = item->list.car;

      char    *name = (char *) expression->atom.name.ptr;
      uint8_t  size = expression->atom.name.size;

      if (lispIsAtom(atom, name, size)) return(item->list.cdr->list.car);

      replacement = replacement->list.cdr;
    }

    return(expression);
  }
}

tExpression ATTRIBUTES
*lispPrimitiveLambda(
  tExpression *expression,
  tExpression *environment) {

  tExpression *lambda = expression->list.car;
  tExpression *values = expression->list.cdr;
  tExpression *result = NULL;

  tExpression *arguments = lispInterleave(lambda->lambda.arguments, values);

  if (lispError == LISP_ERROR_NONE) {
    tExpression *input = lispReplace(lambda->lambda.expression, arguments);

    if (lispError == LISP_ERROR_NONE) {
      result = lispEvaluate(input, environment);
    }
  }

  return(result);
}

tExpression ATTRIBUTES
*lispPrimitiveLabel(
  tExpression *expression,
  tExpression *environment) {

  tExpression *labelName = lispCreateAtom(
    (char *) expression->list.car->atom.name.ptr,  // TODO: Check argument type
    expression->list.car->atom.name.size
  );

  lispAppend(                             // TODO: Replace existing "labelName"
    environment,
    lispCreateList(
      labelName, lispCreateList(expression->list.cdr->list.car, NULL)
    )
  );

  lispExpressionBookmark = lispExpressionCurrent;  // TODO: Memory management ?
  return(truth);
}

tExpression ATTRIBUTES
*lispPrimitiveQuote(
  tExpression *expression,
  tExpression *environment) {

  return(expression->list.car);
}

/* ------------------------------------------------------------------------- */
