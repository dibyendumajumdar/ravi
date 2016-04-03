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
  const char *expected =
      "Content-Length: "
      "99\r\n\r\n{\"type\":\"response\",\"seq\":1,\"command\":\"initialize\","
      "\"success\":false,\"message\":\"unable to initialize\"}";
  if (strcmp(expected, buf) != 0) return 1;
  vscode_make_success_response(&msg, &res, VSCODE_INITIALIZE_RESPONSE);
  vscode_serialize_response(buf, sizeof buf, &res);
  printf("%s\n", buf);
  return 0;
}

int main(int argc, const char *argv[]) {
  setbuf(stdout, NULL);
  int rc = 0;
  rc += test_initreq();
  if (rc == 0) printf("OK\n");
  return rc == 0 ? 0 : 1;
}
