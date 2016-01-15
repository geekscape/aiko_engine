/**
 * Please do not remove the following notices.
 *
 * \file       machine.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * Description
 * ~~~~~~~~~~~
 * Aiko state machine: Cross-platform implementation
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#include "aiko_engine.h"
#include "aiko_state.h"

#define AIKO_STATE_RETRY_MAXIMUM   5
#define AIKO_STATE_TIMEOUT_COUNT  20  // 2 seconds (0.1 second ticks)

static aiko_action_t   *aiko_state_last_action;
static aiko_stream_t   *aiko_state_last_stream;
static aiko_state_type  aiko_state_now;
static uint8_t          aiko_state_retry_count;
static uint8_t          aiko_state_timeout;

static aiko_state_t    *aiko_states       = NULL;
static uint8_t          aiko_states_count = 0;

/* ------------------------------------------------------------------------- */

uint8_t aiko_state_lisp_message_handler(
  void     *void_aiko_stream,
  uint8_t  *message,
  uint16_t  length) {

//printf("Message: %.*s\n", length, message);

  uint8_t handled = AIKO_NOT_HANDLED;
  uint8_t index;
  uint8_t matched = FALSE;

  aiko_stream_t *aiko_stream = void_aiko_stream;

  tExpression *expression = lisp_message_parser(message, length);

  for (index = 0;  index < aiko_states_count;  index ++) {
    if (aiko_states[index].state == aiko_state_now) {
      aiko_state_t *aiko_state = & (aiko_states[index]);

      switch(aiko_state->match_type) {
        case AIKO_MATCH_ANY:
          matched = TRUE;
          break;

        case AIKO_MATCH_ERROR_PARSE:
          matched = (lispError != LISP_ERROR_NONE);
          break;

        case AIKO_MATCH_NIL:
          matched = (lispError == LISP_ERROR_NONE  &&  expression == NULL);
          break;

        case AIKO_MATCH_TRUE:
          matched = lispIsAtom(expression, "#t", 2);
          break;

        case AIKO_MATCH_ATOM:
          matched = (expression != NULL  &&  expression->type == ATOM);
          break;

        case AIKO_MATCH_ATOM_NUMBER:
          matched = lispExpressionIsNumber(
            expression, aiko_state->match_data ? 16 : 10
          );
          break;

        case AIKO_MATCH_ATOM_SYMBOL:
          matched = lispIsAtom(
            expression, aiko_state->match_data, strlen(aiko_state->match_data)
          );
          break;

        case AIKO_MATCH_LIST:
          matched = lispIsList(expression);
          break;

        case AIKO_MATCH_LIST_NUMBER:
          if (lispIsList(expression)) {
            matched = lispExpressionIsNumber(
              expression->list.car, aiko_state->match_data ? 16 : 10
            );
          }
          break;

        case AIKO_MATCH_LIST_SYMBOL:
          if (lispIsList(expression)) {
            matched = lispIsAtom(
              expression->list.car,
              aiko_state->match_data, strlen(aiko_state->match_data)
            );
          }
          break;
      }

      if (matched) {
        aiko_state_last_action = aiko_state->action;
        aiko_state_last_stream = aiko_stream;
        aiko_state_retry_count = 0;
        aiko_state_timeout     = AIKO_STATE_TIMEOUT_COUNT;

        aiko_state_data_t aiko_state_data;
        aiko_state_data.expression = expression;
        aiko_state_now = aiko_state->action(aiko_stream, & aiko_state_data);

        handled = AIKO_HANDLED;
        break;
      }
    }
  }

  lispReset(lispExpressionBookmark);         // TODO: Breaks "primitiveLabel()"

  return(handled);
}

/* ------------------------------------------------------------------------- */

uint8_t aiko_state_timer_handler(
  void *timer_self) {

  if (aiko_state_timeout == 1) {
    if (aiko_state_retry_count ++ < AIKO_STATE_RETRY_MAXIMUM) {
//    printf("Warning: timeout.  Retry message send\n");

      aiko_state_now     = aiko_state_last_action(aiko_state_last_stream, NULL);
      aiko_state_timeout = AIKO_STATE_TIMEOUT_COUNT;
    }
    else {
      if (aiko_states[0].state == AIKO_STATE_ERROR) {
        aiko_states[0].action(aiko_state_last_stream, NULL);
      }
      else {
        printf("Error: Retry message maximum limit\n");
      }
    }
  }

  if (aiko_state_timeout > 0) aiko_state_timeout --;

  return(AIKO_HANDLED);
}

/* ------------------------------------------------------------------------- */

void aiko_state_machine(
  aiko_handler_t *message_handler,
  aiko_state_t   *states,
  uint8_t         states_count,
  aiko_stream_t  *aiko_stream,
  aiko_action_t  *aiko_action) {

  aiko_states       = states;
  aiko_states_count = states_count;

  aiko_state_timeout   = 0;
  aiko_time_t   period = { 0, 10000 };  // 0.1 seconds
  aiko_timer_t *timer  = aiko_add_timer(& period, aiko_state_timer_handler);

  aiko_state_now = AIKO_STATE_IDLE;
  aiko_add_handler(aiko_stream, message_handler);

  aiko_state_last_action = aiko_action;
  aiko_state_last_stream = aiko_stream;
  aiko_state_retry_count = 0;
  aiko_state_timeout     = AIKO_STATE_TIMEOUT_COUNT;

  aiko_state_now = aiko_action(aiko_stream, NULL);
}

/* ------------------------------------------------------------------------- */
