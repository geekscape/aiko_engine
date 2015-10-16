/**
 * Please do not remove the following notices.
 *
 * \file       aiko_state.h.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#pragma once

#include "aiko_engine.h"

#include "lisp.h"

#define AIKO_STATE_RETRY_MAXIMUM   5
#define AIKO_STATE_TIMEOUT_COUNT  20  // 2 seconds (0.1 second ticks)

typedef enum {
  AIKO_STATE_ERROR,
  AIKO_STATE_IDLE,
  AIKO_STATE_USER_DEFINED = 64
}
  aiko_state_type;

typedef enum {
  AIKO_MATCH_ANY,
  AIKO_MATCH_ERROR_PARSE,
  AIKO_MATCH_NIL,
  AIKO_MATCH_TRUE,
  AIKO_MATCH_ATOM,
  AIKO_MATCH_ATOM_NUMBER,
  AIKO_MATCH_ATOM_SYMBOL,
  AIKO_MATCH_LIST,
  AIKO_MATCH_LIST_NUMBER,
  AIKO_MATCH_LIST_SYMBOL
}
  aiko_match_type;

typedef aiko_state_type (aiko_action_t)(
  aiko_stream_t *aiko_stream, tExpression *expression
);

typedef struct {
  aiko_state_type  state;
  aiko_match_type  match_type;
  char            *match_data;
  aiko_action_t   *action;
}
  aiko_state_t;

/* ------------------------------------------------------------------------- */

void aiko_state_machine(
  aiko_state_t  *states,
  uint8_t        states_count,
  aiko_stream_t *aiko_stream,
  aiko_action_t *aiko_action
);
