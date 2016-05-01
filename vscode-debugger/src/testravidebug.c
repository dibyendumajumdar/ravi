#include "protocol.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int test_initreq() {
  char testdata[200] =
    "{\"type\":\"request\",\"seq\":1,\"command\":\"initialize\","
    "\"arguments\":{\"adapterID\":\"lua\",\"pathFormat\":\"path\","
    "\"linesStartAt1\":true,\"columnsStartAt1\":true}}";
  ProtocolMessage msg;
  int msgtype = vscode_parse_message(testdata, strlen(testdata), &msg, stderr);
  if (msgtype != VSCODE_INITIALIZE_REQUEST || msg.seq != 1 ||
    msg.type != VSCODE_REQUEST ||
    strcmp(msg.u.Request.command, "initialize") != 0 ||
    msg.u.Request.request_type != VSCODE_INITIALIZE_REQUEST ||
    strcmp(msg.u.Request.u.InitializeRequest.adapterID, "lua") != 0 ||
    msg.u.Request.u.InitializeRequest.columnsStartAt1 != 1 ||
    msg.u.Request.u.InitializeRequest.linesStartAt1 != 1 ||
    strcmp(msg.u.Request.u.InitializeRequest.pathFormat, "path") != 0) {
    return 1;
  }
  ProtocolMessage res;
  vscode_make_error_response(&msg, &res, VSCODE_INITIALIZE_RESPONSE,
                             "unable to initialize");
  char buf[1024];
  vscode_serialize_response(buf, sizeof buf, &res);
  //printf(buf);
  const char *expected =
      "Content-Length: "
      "115\r\n\r\n{\"type\":\"response\",\"seq\":1,\"command\":\"initialize\",\"request_seq\":1,"
      "\"success\":false,\"message\":\"unable to initialize\"}";
  if (strcmp(expected, buf) != 0) return 1;
  vscode_make_success_response(&msg, &res, VSCODE_INITIALIZE_RESPONSE);
  vscode_serialize_response(buf, sizeof buf, &res);
  const char *expected2 =
      "Content-Length: "
    "240\r\n\r\n{\"type\":\"response\",\"seq\":2,\"command\":\"initialize\",\"request_seq\":1,"
      "\"success\":true,\"body\":{\"supportsConfigurationDoneRequest\":false,"
      "\"supportsFunctionBreakpoints\":false,"
      "\"supportsConditionalBreakpoints\":false,\"supportsEvaluateForHovers\":"
      "false}}";
  if (strcmp(expected2, buf) != 0) return 1;

  char testdata2[] = "{\"type\":\"request\",\"seq\":2,\"command\":\"setBreakpoints\",\"arguments\":{\"source\":{\"path\":\"c:\\\\github\\\\ravi\\\\ravi-tests\\\\simple.lua\"},\"lines\" : [6],\"breakpoints\" : [{\"line\":6}]} }";
  msgtype = vscode_parse_message(testdata2, strlen(testdata2), &msg, stderr);
  if (msgtype != VSCODE_SET_BREAKPOINTS_REQUEST || msg.seq != 2 ||
    msg.type != VSCODE_REQUEST ||
    strcmp(msg.u.Request.command, "setBreakpoints") != 0 ||
    msg.u.Request.request_type != VSCODE_SET_BREAKPOINTS_REQUEST ||
    strcmp(msg.u.Request.u.SetBreakpointsRequest.source.path, "c:/github/ravi/ravi-tests/simple.lua") != 0 ||
    msg.u.Request.u.SetBreakpointsRequest.breakpoints[0].line != 6) {
    return 1;
  }
  return 0;
}

void experiment() {
  uint32_t i = 0;
  uint32_t MASK0 = 0x7F;
  uint32_t MASK = 0xFF;
  uint32_t MASK1 = 0x1;
  
#define MAKENUM(t,a,b,c,d) \
  ( ((t)&MASK1) | (((a)&MASK0) << 1) ) | \
  ( ((b)&MASK) << 8 ) | \
  ( ((c)&MASK) << 16 ) | \
  ( ((d)&MASK) << 24 )
  
  i = MAKENUM(1, 1, 1, 1, 1);
  printf("NUM = %d\n", i);
  
#define EXTRACT_T(x) ( (x)&MASK1 )
#define EXTRACT_A(x) ( ((x)>>1) & MASK0 )
#define EXTRACT_B(x) ( ((x)>>8) & MASK )
#define EXTRACT_C(x) ( ((x)>>16) & MASK )
#define EXTRACT_D(x) ( ((x)>>24) & MASK )
  
  printf("%d,%d,%d,%d,%d\n", EXTRACT_T(i), EXTRACT_A(i), EXTRACT_B(i),
         EXTRACT_C(i), EXTRACT_D(i));

}


int main(void) {
  setbuf(stdout, NULL);
  int rc = 0;
  rc += test_initreq();
  experiment();
  if (rc == 0) printf("OK\n");
  return rc == 0 ? 0 : 1;
}
