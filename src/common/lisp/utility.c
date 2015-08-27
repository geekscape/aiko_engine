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

  uint8_t name[LISP_ATOM_SIZE_LIMIT + 1];
  int32_t result = 0;

  if (expression != NULL  &&  expression->type == ATOM) {
    uint8_t size = expression->atom.name.size;
    memcpy(name, expression->atom.name.ptr, expression->atom.name.size);
    name[size] = 0x00;
    result = atoi((const char *) name);
  }

  return(result);
}

/* ------------------------------------------------------------------------- */
