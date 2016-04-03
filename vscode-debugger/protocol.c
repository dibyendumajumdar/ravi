#include "protocol.h"
#include <stdio.h>
#include <string.h>

static int vscode_message_type(json_value *js, FILE *log) {
  if (js->type != json_object) {
    fprintf(log, "Bad JSON message - expected object\n");
    return VSCODE_UNKNOWN;
  }
  for (int i = 0; i < js->u.object.length; i++) {
    json_object_entry *value = &js->u.object.values[i];
    if (strncmp(value->name, "type", value->name_length) == 0) {
      if (value->value->type != json_string) {
        fprintf(log, "Bad JSON message - type is not a string\n");
        return VSCODE_UNKNOWN;
      }
      if (strncmp(value->value->u.string.ptr, "request",
                  value->value->u.string.length) == 0) {
        return VSCODE_REQUEST;
      }
      else if (strncmp(value->value->u.string.ptr, "response",
                       value->value->u.string.length) == 0) {
        return VSCODE_RESPONSE;
      }
      else if (strncmp(value->value->u.string.ptr, "event",
                       value->value->u.string.length) == 0) {
        return VSCODE_EVENT;
      }
      else {
        fprintf(log, "Bad JSON message - unknown type\n");
        return VSCODE_UNKNOWN;
      }
    }
  }
  fprintf(log, "Bad JSON message - unknown type\n");
  return VSCODE_UNKNOWN;
}

static const char *get_string_value(json_value *js, const char *key,
                                    FILE *log) {
  if (js->type != json_object) { return NULL; }
  for (int i = 0; i < js->u.object.length; i++) {
    json_object_entry *value = &js->u.object.values[i];
    if (strncmp(value->name, key, value->name_length) == 0) {
      if (value->value->type == json_string) {
        return value->value->u.string.ptr;
      }
      else {
        return NULL;
      }
    }
  }
  return NULL;
}

static int get_int_value(json_value *js, const char *key, FILE *log,
                         int *found) {
  *found = 0;
  if (js->type != json_object) { return 0; }
  for (int i = 0; i < js->u.object.length; i++) {
    json_object_entry *value = &js->u.object.values[i];
    if (strncmp(value->name, key, value->name_length) == 0) {
      if (value->value->type == json_integer) {
        *found = 1;
        return value->value->u.integer;
      }
      else {
        return 0;
      }
    }
  }
  return 0;
}

static int get_boolean_value(json_value *js, const char *key, FILE *log,
                             int *found) {
  *found = 0;
  if (js->type != json_object) { return 0; }
  for (int i = 0; i < js->u.object.length; i++) {
    json_object_entry *value = &js->u.object.values[i];
    if (strncmp(value->name, key, value->name_length) == 0) {
      if (value->value->type == json_boolean) {
        *found = 1;
        return value->value->u.boolean;
      }
      else {
        return 0;
      }
    }
  }
  return 0;
}

static json_value *get_object_value(json_value *js, const char *key,
                                    FILE *log) {
  if (js->type != json_object) { return NULL; }
  for (int i = 0; i < js->u.object.length; i++) {
    json_object_entry *value = &js->u.object.values[i];
    if (strncmp(value->name, key, value->name_length) == 0) {
      if (value->value->type == json_object) { return value->value; }
      else {
        return NULL;
      }
    }
  }
  return NULL;
}

typedef struct {
  const char *cmdname;
  int code;
} CommandMapping;

static CommandMapping commands[] = {
    {"initialize", VSCODE_INITIALIZE_REQUEST},
    {"configurationDone", VSCODE_CONFIGURATION_DONE_REQUEST},
    {"launch", VSCODE_LAUNCH_REQUEST},
    {"attach", VSCODE_ATTACH_REQUEST},
    {"disconnect", VSCODE_DISCONNECT_REQUEST},
    {"setBreakpoints", VSCODE_SET_BREAKPOINTS_REQUEST},
    {"setFunctionBreakpoints", VSCODE_SET_FUNCTION_BREAKPOINTS_REQUEST},
    {"setExceptionBreakpoints", VSCODE_SET_EXCEPTION_BREAKPOINTS_REQUEST},
    {"continue", VSCODE_CONTINUE_REQUEST},
    {"next", VSCODE_NEXT_REQUEST},
    {"stepIn", VSCODE_STEPIN_REQUEST},
    {"stepOut", VSCODE_STEPOUT_REQUEST},
    {"pause", VSCODE_PAUSE_REQUEST},
    {"stackTrace", VSCODE_STACK_TRACE_REQUEST},
    {"scopes", VSCODE_SCOPES_REQUEST},
    {"variables", VSCODE_VARIABLES_REQUEST},
    {"source", VSCODE_SOURCE_REQUEST},
    {"threads", VSCODE_THREAD_REQUEST},
    {"evaluate", VSCODE_EVALUATE_REQUEST},
    {NULL, VSCODE_UNKNOWN_REQUEST}};

static int get_cmdtype(const char *cmd) {
  for (int i = 0; commands[i].cmdname != NULL; i++) {
    if (strcmp(cmd, commands[i].cmdname) == 0)
      return commands[i].code;
  }
  return VSCODE_UNKNOWN_REQUEST;
}

static int vscode_parse_initialize_request(json_value *js, ProtocolMessage *msg,
  FILE *log) {
  // {"type":"request","seq":1,"command":"initialize","arguments":{"adapterID":"lua","pathFormat":"path","linesStartAt1":true,"columnsStartAt1":true}}
  json_value *args = get_object_value(js, "arguments", log);
  if (args == NULL) return VSCODE_UNKNOWN_REQUEST;
  const char *adapterID = get_string_value(args, "adapterID", log);
  if (adapterID == NULL || strcmp(adapterID, "lua") != 0) {
    fprintf(log, "Unknown adapterID '%s' in initialize command\n", adapterID != NULL ? adapterID : "null");
    return VSCODE_UNKNOWN_REQUEST;
  }
  strncpy(msg->u.Request.u.InitializeRequest.adapterID, adapterID, sizeof msg->u.Request.u.InitializeRequest.adapterID);
  const char *pathFormat = get_string_value(args, "pathFormat", log);
  if (pathFormat != NULL && strcmp(pathFormat, "path") != 0) {
    fprintf(log, "Unsupported pathFormat '%s' in initialize command\n", pathFormat != NULL ? pathFormat : "null");
    return VSCODE_UNKNOWN_REQUEST;
  }
  if (pathFormat)
    strncpy(msg->u.Request.u.InitializeRequest.pathFormat, pathFormat, sizeof msg->u.Request.u.InitializeRequest.pathFormat);
  int found = 0;
  msg->u.Request.u.InitializeRequest.columnsStartAt1 = get_boolean_value(args, "columnsStartAt1", log, &found);
  msg->u.Request.u.InitializeRequest.linesStartAt1 = get_boolean_value(args, "linesStartAt1", log, &found);
  msg->u.Request.request_type = VSCODE_INITIALIZE_REQUEST;
  return VSCODE_INITIALIZE_REQUEST;
}

static int vscode_parse_intarg_request(json_value *js, ProtocolMessage *msg, int msgtype, const char *key,
  FILE *log) {
  json_value *args = get_object_value(js, "arguments", log);
  if (args == NULL) return VSCODE_UNKNOWN_REQUEST;
  int found = 0;
  msg->u.Request.u.CommonIntRequest.integer = get_int_value(args, key, log, &found);
  msg->u.Request.request_type = msgtype;
  return msgtype;
}

static int vscode_parse_launch_request(json_value *js, ProtocolMessage *msg, int msgtype, FILE *log) {
  json_value *args = get_object_value(js, "arguments", log);
  if (args == NULL) return VSCODE_UNKNOWN_REQUEST;
  int found = 0;
  msg->u.Request.u.LaunchRequest.noDebug = get_boolean_value(args, "noDebug", log, &found);
  msg->u.Request.request_type = msgtype;
  return msgtype;
}

static int vscode_parse_request(json_value *js, ProtocolMessage *msg,
                                FILE *log) {
  const char *cmd = get_string_value(js, "command", log);
  int found = 0;
  if (cmd == NULL) return VSCODE_UNKNOWN_REQUEST;
  msg->type = VSCODE_REQUEST;
  msg->seq = get_int_value(js, "seq", log, &found);
  strncpy(msg->u.Request.command, cmd, sizeof msg->u.Request.command);
  int cmdtype = get_cmdtype(msg->u.Request.command);
  switch (cmdtype) {
  case VSCODE_INITIALIZE_REQUEST:
    return vscode_parse_initialize_request(js, msg, log);
  case VSCODE_DISCONNECT_REQUEST:
  case VSCODE_ATTACH_REQUEST:
  case VSCODE_CONFIGURATION_DONE_REQUEST:
    msg->u.Request.request_type = cmdtype;
    break;
  case VSCODE_CONTINUE_REQUEST:
  case VSCODE_NEXT_REQUEST:
  case VSCODE_STEPIN_REQUEST:
  case VSCODE_STEPOUT_REQUEST:
  case VSCODE_PAUSE_REQUEST:
    return vscode_parse_intarg_request(js, msg, cmdtype, "threadId", log);
  case VSCODE_SCOPES_REQUEST:
    return vscode_parse_intarg_request(js, msg, cmdtype, "frameId", log);
  case VSCODE_LAUNCH_REQUEST:
    return vscode_parse_launch_request(js, msg, cmdtype, log);
  case VSCODE_UNKNOWN_REQUEST:
    break;
  default:
    msg->u.Request.request_type = cmdtype;
  }
  return cmdtype;
}

int vscode_parse_message(char *buf, size_t len, ProtocolMessage *msg,
                         FILE *log) {
  memset(msg, 0, sizeof(ProtocolMessage));
  json_value *js = json_parse(buf, len);
  if (js == NULL) return VSCODE_UNKNOWN;

  int msgtype = vscode_message_type(js, log);
  if (msgtype == VSCODE_REQUEST) 
    msgtype = vscode_parse_request(js, msg, log);
  json_value_free(js);
  return msgtype;
}

static int seq = 1;

void vscode_make_error_response(ProtocolMessage *req, ProtocolMessage *res, int restype, const char *errormsg) {
  memset(res, 0, sizeof(ProtocolMessage));
  res->seq = seq++;
  res->type = VSCODE_RESPONSE;
  strncpy(res->u.Response.command, req->u.Request.command, sizeof res->u.Response.command);
  res->u.Response.request_seq = req->seq;
  res->u.Response.response_type = restype;
  strncpy(res->u.Response.message, errormsg, sizeof res->u.Response.message);
  res->u.Response.success = 0;
}

void vscode_make_success_response(ProtocolMessage *req, ProtocolMessage *res, int restype) {
  memset(res, 0, sizeof(ProtocolMessage));
  res->seq = seq++;
  res->type = VSCODE_RESPONSE;
  strncpy(res->u.Response.command, req->u.Request.command, sizeof res->u.Response.command);
  res->u.Response.request_seq = req->seq;
  res->u.Response.response_type = restype;
  res->u.Response.success = 1;
}

void vscode_serialize_response(char *buf, size_t buflen, ProtocolMessage *res) {
  char temp[1024] = { 0 };
  char *cp = temp;
  buf[0] = 0;
  if (res->type != VSCODE_RESPONSE)
    return;
  snprintf(cp, sizeof temp - strlen(temp), "{\"type\":\"response\",\"seq\":%d,\"command\":\"%s\",\"success\":%s",
    res->seq, res->u.Response.command, res->u.Response.success ? "true" : "false");
  cp = temp + strlen(temp);
  if (res->u.Response.message[0]) {
    snprintf(cp, sizeof temp - strlen(temp), ",\"error\":\"%s\"",
      res->u.Response.message);
    cp = temp + strlen(temp);
  }
  if (res->u.Response.response_type == VSCODE_INITIALIZE_RESPONSE && res->u.Response.success) {
    snprintf(cp, sizeof temp - strlen(temp), ",\"body\":{\"supportsConfigurationDoneRequest\":%s,\"supportsFunctionBreakpoints\":%s,\"supportsConditionalBreakpoints\":%s,\"supportsEvaluateForHovers\":%s}",
      res->u.Response.u.InitializeResponse.body.supportsConfigurationDoneRequest ? "true" : "false",
      res->u.Response.u.InitializeResponse.body.supportsFunctionBreakpoints ? "true" : "false",
      res->u.Response.u.InitializeResponse.body.supportsConditionalBreakpoints ? "true" : "false",
      res->u.Response.u.InitializeResponse.body.supportsEvaluateForHovers ? "true" : "false"
      );
    cp = temp + strlen(temp);
  }
  snprintf(cp, sizeof temp - strlen(temp), "}");
  cp = temp + strlen(temp);
  snprintf(buf, buflen, "Content-Length: %d\r\n\r\n%s", (int)strlen(temp), temp);
}