/**
 * Please do not remove the following notices.
 *
 * \file       parser.c
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
*lispParseToken(
  tReader *reader) {

  char token[LISP_ATOM_SIZE_LIMIT];  // TODO: Use mmem memory directly, no copy
  uint8_t size = 0;

  uint8_t ch = reader->getCh();

  if (ch == '\n') ch = reader->getCh();

  if (reader->isFileEOF()) {
    lispError = LISP_ERROR_END_OF_INPUT;
    lispErrorMessage = "(5:error12:end of input)";
    return(NULL);
  }

  if (ch == '(') return(parenthesisOpen);
  if (ch == ')') return(parenthesisClose);

  while (ch >= '0'  &&  ch <= '9') {
    size = size * 10 + (ch - '0');    // TODO: What if size too large for "int"

    if (reader->isFileEOF()) {
      lispError = LISP_ERROR_PARSE_TOKEN;
      lispErrorMessage = "(5:error15:token truncated)";
      return(NULL);
    }

    ch = reader->getCh();
  }

  if (ch != ':') {
    lispError = LISP_ERROR_PARSE_TOKEN;
    lispErrorMessage = "(5:error17:must be 'n:token')";
    return(NULL);
  }

// If LISP_ATOM_SIZE_LIMIT exceeded, still attempt to read token, throw it away

  if (size > LISP_ATOM_SIZE_LIMIT) {
    lispError = LISP_ERROR_LIMIT_TOKEN;
    lispErrorMessage =
      "(5:error13:atom size >"STRINGIFY(LISP_ATOM_SIZE_LIMIT)")";
  }

  uint8_t count;
  for (count = 0;  count < size;  count ++) {
    if (reader->isFileEOF()) {
      lispError = LISP_ERROR_PARSE_TOKEN;
      lispErrorMessage = "(5:error15:token truncated)";
      return(NULL);
    }

    token[count] = reader->getCh();
  }

  if (lispError) return(NULL);

  return(lispCreateAtom(token, size));
}

tExpression ATTRIBUTES
*lispParseList(
  tReader *reader) {

  tExpression *expression = lispParseToken(reader);

  if (expression != NULL) {
    if (expression == parenthesisClose) {
      expression = NULL;
    }
    else {
      if (expression == parenthesisOpen) expression = lispParseList(reader);

      tExpression *list = lispParseList(reader);
      expression = lispCreateList(expression, list);
    }
  }

  return(expression);
}

tExpression ATTRIBUTES
*lispParse(
  tReader *reader) {

  tExpression *expression = lispParseToken(reader);

  if (expression != NULL  &&  expression == parenthesisOpen) {
    expression = lispParseList(reader);
  }

  return(expression);
}

/* ------------------------------------------------------------------------- */
