/**
 * Please do not remove the following notices.
 *
 * \file       aiko_engine.c
 * \author     Andy Gelme <andyg@geekscape.org>
 * \copyright  (c) 2014-2015 by Geekscape Pty. Ltd.
 * \license    AGPLv3 http://geekscape.org/static/aiko_license.html
 *
 * Usage
 * ~~~~~
 * - ./aiko_engine [ source_file ]
 *
 * To Do
 * ~~~~~
 * - None, yet.
 */

#include <stdlib.h>

#include "../aiko_engine.h"

tExpression *aikoEnvironment;
tReader     *aikoIoInitialize(FILE *inputFile);
//tReader   *aikoIoInitialize(char *buffer, int size);     // Contiki / Arduino

#define LOCAL_PORT 4000

int aikoError;

void aikoUsage1(void);
void aikoUsage2(void);

/*---------------------------------------------------------------------------*/
static tReader *
aikoInitialize(
  int   argc,
  char *argv[]) {

  mmem_init();
#ifdef AIKO_DEBUG
  aikoUsage1();
#endif

  aikoError = AIKO_ERROR_NONE;
  aikoEnvironment = aikoExpressionInitialize();
  if (aikoError != 0) {
    printf("Initialization error: %d\n", aikoError);
    exit(-1);
  }

  FILE *inputFile = stdin;
  if (argc > 1) inputFile = fopen(argv[1], "r");
  tReader *reader = aikoIoInitialize(inputFile);

  return(reader);
}

/*---------------------------------------------------------------------------*/
int main(
  int   argc,
  char *argv[]) {

  tReader *reader = aikoInitialize(argc, argv);

  while (aikoError == 0) {
#ifdef AIKO_DEBUG
    aikoUsage2();
#endif
    printf("> ");
    tExpression *expression = aikoParse(reader);

    if (aikoError != AIKO_ERROR_NONE) {
//    printf("Parse error: %d\n", aikoError);
      break;
    }
    else {
      expression = aikoEvaluate(expression, aikoEnvironment);

      if (expression == NULL) {
        printf("Evaluate error: %d\n", aikoError);
      }
      else {
        aikoEmit(expression);
        printf("\n");
      }
    }

    aikoReset(aikoExpressionBookmark);       // TODO: Breaks "primitiveLabel()"
    aikoError = AIKO_ERROR_NONE;
  }

  return(0);
}

/*---------------------------------------------------------------------------*/
void
aikoUsage1(void) {
  printf(
    "Heap memory: %d, Expression memory: %lu, sizeof: %lu, available: %d\n",
    avail_memory, sizeof(aikoExpressions), sizeof(tExpression), AIKO_EXPRESSION_LIMIT
  );
}

/*---------------------------------------------------------------------------*/
void
aikoUsage2(void) {
  printf(
    "Heap used: %d, Expressions used: %d\n",
    MMEM_CONF_SIZE - avail_memory, aikoExpressionCurrent
  );
}

/*---------------------------------------------------------------------------*/
