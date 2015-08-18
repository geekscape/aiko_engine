/**
 * Please do not remove the following notices.
 *
 * \file       utility.c
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

/* ------------------------------------------------------------------------- */

int32_t ATTRIBUTES
aikoToInteger(
  tExpression *expression) {

  uint8_t name[AIKO_ATOM_SIZE_LIMIT + 1];
  int32_t result = 0;

  if (expression != NULL  &&  expression->type == ATOM) {
    uint8_t size = expression->atom.name.size;
    memcpy(name, expression->atom.name.ptr, expression->atom.name.size);
    name[size] = 0x00;
    result = atoi(name);
  }

  return(result);
}

/* ------------------------------------------------------------------------- */
