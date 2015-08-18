/**
 * \file
 *         Lisp engine
 * \author
 *         Andy Gelme <andyg@geekscape.org>
 *
 * Usage
 * ~~~~~
 * - TBD
 *
 * To Do
 * ~~~~~
 * - Port back to Mac OS X (for Andrew) and Arduino.
 * - Don't crash on parser (or other) error.
 * - Prefix all functions and variables with "aiko_".
 * - Monitor expressions available / used and heap available / used.
 * - Make into a library or "contiki/apps/lisp_engine".
 */

#include "contiki.h"

#include "dev/rs232.h"
#include "dev/serial-line.h"                      // Serial console input

#define DEBUG DEBUG_PRINT                         // Used by net/ip/uip-debug.h
#include "net/ip/uip-debug.h"                     // TODO: Just use printf ?

#include <stdio.h>
#include <string.h>

#include "lisp.h"

tExpression *environment;
tReader     *ioInitialize(char *buffer, int size);

#define LOCAL_PORT 4000

static struct uip_udp_conn *socket;

#define UIP_IP_BUF ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])

extern uint8_t rf230_last_rssi;

void aiko_usage1(void);
void aiko_usage2(void);

/*---------------------------------------------------------------------------*/
PROCESS(aiko_engine_process, "Aiko");
AUTOSTART_PROCESSES(&aiko_engine_process);
/*---------------------------------------------------------------------------*/
static void
aiko_initialize(void)
{
  mmem_init();
#ifdef LISP_DEBUG
  aiko_usage1();
#endif

  aiko_error = ERROR_NONE;
  environment = expressionInitialize();
  if (aiko_error != 0) {
    PRINTF("Initialization error: %d\n", aiko_error);
    while (1) {}
  }
}

/*---------------------------------------------------------------------------*/
static void
message_handler(char *message)
{
  PRINTF("## message_handler()\n");
  PRINTF("%s\n", message);

PRINTF("### aiko_parse()\n");
//tReader *reader = ioInitialize("1:a", 3);
  tReader *reader = ioInitialize(message, strlen(message));

  tExpression *expression = parse(reader);
  if (expression == NULL) {
    PRINTF("Parse error: ", aiko_error);
    while (1) {}
  }
  else {
#ifdef LISP_DEBUG
    aiko_usage2();
#endif
PRINTF("### aiko_evaluate()\n");
    expression = evaluate(expression, environment);

    if (expression == NULL) {
      PRINTF("Evaluate error: %d\n", aiko_error);
      while (1) {}
    }
    else {
PRINTF("### aiko_emit()\n");
      emit(expression);
      PRINTF("\n");
    }
  }

#ifdef LISP_DEBUG
  aiko_usage2();
#endif
  PRINTF("\n");

  reset(expressionInitial);
}

/*---------------------------------------------------------------------------*/
static void
udp_handler(void)
{
  char *appdata;

  if(uip_newdata()) {
    appdata = (char *)uip_appdata;
    appdata[uip_datalen()] = 0;

    PRINTF("## udp_handler()\n");
    PRINTF("Remote IP: ");
    uip_debug_ipaddr_print(&UIP_IP_BUF->srcipaddr);
    PRINTF(", RSSI: %d\n", -91 + (rf230_last_rssi - 1));

    message_handler(appdata);
  }
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(aiko_engine_process, ev, data)
{
  static struct etimer et;
  static clock_time_t  rate = CLOCK_SECOND / 16;  // 16 Hz

  PROCESS_BEGIN();

  DDRD  = 0xff;  // MeshThing v0.0 Green LED
  PORTD = 0;

  DDRB  = 0xff;  // MeshThing v0.1 and V0.2 Green LED
  PORTB = 0;

PRINTF("### aiko_initialize()\n");
  aiko_initialize();
#ifdef LISP_DEBUG
  aiko_usage2();
#endif

  etimer_set(&et, rate);

  socket = udp_new(NULL, UIP_HTONS(0), NULL);

  if(socket == NULL) {
    PRINTF("Create socket failed.  Process exit.\n");
    PROCESS_EXIT();
  }

  udp_bind(socket, UIP_HTONS(LOCAL_PORT));

  PRINTF("UDP local/remote port %u/%u\n",
    UIP_HTONS(socket->lport), UIP_HTONS(socket->rport));

  while(1) {
    PROCESS_WAIT_EVENT();

    if(ev == tcpip_event) udp_handler();

    if(ev == serial_line_event_message) message_handler((char *) data);

//  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    if (ev == PROCESS_EVENT_TIMER  &&  etimer_expired(&et)) {
      etimer_reset(&et);                   // Reset the etimer to trigger again
    }
  }
  
  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
void
aiko_usage1(void) {
  PRINTF("Heap memory: %d\n", avail_memory);
  PRINTF("Expression sizeof: %d\n", sizeof(tExpression));
  PRINTF("Expressions available: %d\n", EXPRESSION_LIMIT);
}

/*---------------------------------------------------------------------------*/
void
aiko_usage2(void) {
  PRINTF("Heap used: %d\n", 4096 - avail_memory);
  PRINTF("Expressions used: %d\n", expressionCurrent);
}

/*---------------------------------------------------------------------------*/
