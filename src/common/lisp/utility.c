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

#ifdef ARDUINO
#include "vendor/aiko_engine/include/aiko_compatibility.h"
#include "vendor/aiko_engine/include/lisp.h"
#else
#include "aiko_compatibility.h"
#include "lisp.h"
#endif

/* ------------------------------------------------------------------------- */

int32_t ATTRIBUTES
lispToInteger(
  tExpression *expression) {

  uint8_t buffer[LISP_ATOM_SIZE_LIMIT + 1];
  int32_t result = 0;

  if (lispToString(expression, & buffer, LISP_ATOM_SIZE_LIMIT + 1)) {
    result = atoi((const char *) buffer);
  }

  return(result);
}

uint8_t ATTRIBUTES
lispToString(
  tExpression *expression,
  void        *output,
  uint8_t      size) {

  uint8_t *buffer = output;

  if (expression != NULL  &&  expression->type == ATOM) {
    if (expression->atom.name.size < size) {
      size = expression->atom.name.size + 1;
    }

    size --;                           // leave extra byte for null-termination
    memcpy(buffer, expression->atom.name.ptr, size);
    buffer[size] = 0x00;
  }
  else {
    size = 0;
  }

  return(size);
}

/* ------------------------------------------------------------------------- */
