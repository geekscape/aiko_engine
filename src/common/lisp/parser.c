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
*aikoParseToken(
  tReader *reader) {

  char token[AIKO_ATOM_SIZE_LIMIT];  // TODO: Use mmem memory directly, no copy
  uint8_t size = 0;

  uint8_t ch = reader->getCh();

  if (ch == '\n') ch = reader->getCh();

  if (reader->isFileEOF()) {
    PRINTLN("aikoParseToken(): End of file");
    aikoError = AIKO_ERROR_END_OF_FILE;
    return(NULL);
  }

  if (ch == '(') return(parenthesisOpen);
  if (ch == ')') return(parenthesisClose);

  while (ch >= '0'  &&  ch <= '9') {
    size = size * 10 + (ch - '0');    // TODO: What if size too large for "int"

    if (reader->isFileEOF()) {
      PRINTLN("aikoParseToken(): Error: Truncated token size");
      aikoError = AIKO_ERROR_PARSE_TOKEN;
      return(NULL);
    }

    ch = reader->getCh();
  }

  if (ch != ':') {
    PRINTLN("aikoParseToken(): Error: Should be 'n:token'");
    aikoError = AIKO_ERROR_PARSE_TOKEN;
    return(NULL);
  }

// If AIKO_ATOM_SIZE_LIMIT exceeded, still attempt to read token, throw it away

  if (size > AIKO_ATOM_SIZE_LIMIT) {
#if ARDUINO
    Serial.print("aikoParseToken(): Error: Token length >");
    Serial.println(AIKO_ATOM_SIZE_LIMIT);
#else
    printf("aikoParseToken(): Error: Token length >%d\n", AIKO_ATOM_SIZE_LIMIT);
#endif
    aikoError = AIKO_ERROR_LIMIT_TOKEN;
  }

  uint8_t count;
  for (count = 0;  count < size;  count ++) {
    if (reader->isFileEOF()) {
      PRINTLN("aikoParseToken(): Error: Truncated token");
      aikoError = AIKO_ERROR_PARSE_TOKEN;
      return(NULL);
    }

    token[count] = reader->getCh();
  }

  if (aikoError != 0) return(NULL);

  return(aikoCreateAtom(token, size));
}

tExpression ATTRIBUTES
*aikoParseList(
  tReader *reader) {

  tExpression *expression = aikoParseToken(reader);

  if (expression != NULL) {
    if (expression == parenthesisClose) {
      expression = NULL;
    }
    else {
      if (expression == parenthesisOpen) expression = aikoParseList(reader);

      tExpression *list = aikoParseList(reader);
      expression = aikoCreateList(expression, list);
    }
  }

  return(expression);
}

tExpression ATTRIBUTES
*aikoParse(
  tReader *reader) {

  tExpression *expression = aikoParseToken(reader);

  if (expression != NULL  &&  expression == parenthesisOpen) {
    expression = aikoParseList(reader);
  }

  return(expression);
}

/* ------------------------------------------------------------------------- */
