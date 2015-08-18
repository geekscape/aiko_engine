/**
 * Please do not remove the following notices.
 *
 * \file       lisp_engine.c
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

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>

#include "lisp.h"

#define MAXIMUM_FILE_COUNT    2
#define INPUT_BUFFER_SIZE   128
#define AIKO_SERVER_PORT   4000

tExpression *aikoEnvironment;
tReader     *aikoIoInitialize(char *buffer, int size);

void aikoUsage1(void);
void aikoUsage2(void);

/*---------------------------------------------------------------------------*/
static FILE *
aikoInitialize(
  int   argc,
  char *argv[]) {

  mmem_init();
#ifdef LISP_DEBUG
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

  return(inputFile);
}

/*---------------------------------------------------------------------------*/
static int
aikoMessageHandler(tReader *reader) {
  tExpression *expression = aikoParse(reader);

  if (aikoError != AIKO_ERROR_NONE) {
//  printf("Parse error: %d\n", aikoError);
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

  aikoReset(aikoExpressionBookmark);         // TODO: Breaks "primitiveLabel()"

  return(aikoError);
}

/*---------------------------------------------------------------------------*/
int main(
  int   argc,
  char *argv[]) {

  char      buffer[INPUT_BUFFER_SIZE];
  int       count, flags, index, fdLargest;
  int       fd[MAXIMUM_FILE_COUNT];
  fd_set    readSet;
  struct    sockaddr_in6 clientAddr, serverAddr;
  socklen_t clientAddrLen;

  for (index = 0;  index < MAXIMUM_FILE_COUNT;  index ++) fd[index] = -1;

  FILE *inputFile = aikoInitialize(argc, argv);
  fd[0] = 0;

  clientAddrLen = sizeof(clientAddr);
  flags = 0;
  fd[1] = socket(PF_INET6, SOCK_DGRAM, 0);

  if (fd[1] < 0) {
    printf("ERROR: Opening socket\n");
    exit(1);
  }


  memset(& serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin6_family = AF_INET6;
  serverAddr.sin6_addr   = in6addr_any;

  if (bind(fd[1], (struct sockaddr *) & serverAddr, sizeof(serverAddr)) < 0) {
    printf("Cannot bind port number %d\n", AIKO_SERVER_PORT);
    exit(1);
  }

  while (aikoError == 0) {
#ifdef LISP_DEBUG
    aikoUsage2();
#endif
    fdLargest = 0;
    FD_ZERO(& readSet);

    for (index = 0; index < MAXIMUM_FILE_COUNT; index ++) {
      if (fd[index] != -1) {
        if (fd[index] > fdLargest) fdLargest = fd[index];
        FD_SET(fd[index], & readSet);
      }
    }

// TODO: Need to check select() return value
    select(fdLargest + 1, & readSet, NULL, NULL, NULL);

    for (index = 0; index < MAXIMUM_FILE_COUNT; index ++) {
      if (fd[index] != -1  &&  FD_ISSET(fd[index], & readSet)) {
        if (fd[index] == 0) {
          if (fgets(buffer, sizeof(buffer), inputFile) == NULL) exit(0);
        }
        else {
          count = recvfrom(
            fd[index], buffer, sizeof(buffer), flags,
            (struct sockaddr *) & clientAddr, & clientAddrLen
          );
printf("buffer: %s\n", buffer);
        }
      }
    }

    tReader *reader = aikoIoInitialize(buffer, strlen(buffer));

    aikoError = aikoMessageHandler(reader);
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
