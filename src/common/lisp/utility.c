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
 * - lispExpressionIsNumber() ony handles positive integers and base-16.
 */

#include <string.h>

#ifdef ARDUINO
#include "vendor/aiko_engine/include/aiko_engine.h"
#include "vendor/aiko_engine/include/lisp.h"
#else
#include "aiko_engine.h"
#include "lisp.h"
#endif

/* ------------------------------------------------------------------------- */

uint8_t ATTRIBUTES
lispExpressionIsNumber(
  tExpression *expression,
  uint8_t      radix) {

  if (expression != NULL  &&  expression->type == ATOM) {
    uint8_t  index;
    char    *name = (char *) expression->atom.name.ptr;

    for (index = 0;  index < expression->atom.name.size;  index ++) {
      char ch = name[index];

      if (radix == 16) {
        if (ch >= 'A'  &&  ch <= 'F') continue;
        if (ch >= 'a'  &&  ch <= 'f') continue;
      }

      if (ch < '0'  ||  ch > '9') return(FALSE);
    }
  }

  return(TRUE);
}

int32_t ATTRIBUTES
lispExpressionToInteger(
  tExpression *expression) {

  uint8_t buffer[LISP_ATOM_SIZE_LIMIT + 1];
  int32_t result = 0;

  if (lispExpressionToString(expression, & buffer, LISP_ATOM_SIZE_LIMIT + 1)) {
    result = atoi((const char *) buffer);
  }

  return(result);
}

uint8_t ATTRIBUTES
lispExpressionToString(
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

uint8_t ATTRIBUTES
lispIntegerToString(
  uint8_t  value,
  uint8_t *output,
  uint8_t  output_size) {

  uint8_t const digit[] = "0123456789";
  uint8_t result_size = 0;
  uint8_t value_shift = value;
  int8_t  index;

  do {
    result_size ++;
    value_shift = value_shift / 10;
  }
    while (value_shift > 0);

  if (result_size <= output_size) {
    for (index = result_size - 1;  index >= 0;  index --) {
      output[index] = digit[value % 10];
      value = value / 10;
    }
  }
  else {
    memset(output, '*', output_size);
    result_size = output_size;
  }

  return(result_size);
}

/* ------------------------------------------------------------------------- */
