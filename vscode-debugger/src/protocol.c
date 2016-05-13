#include "protocol.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Parse a VSCode JSON message type
 */
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
  (void)log;
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
  (void)log;
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
  (void)log;
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
  (void)log;
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

/*
 * Escape certain control characters to match JSON string
 * requirements. 'src' should be 0 terminated, 'len' is the
 * size of 'dest' buffer. If the 'dest' buffer is smaller than
 * required then the string will be silently truncated
 */
void vscode_json_stringify(const char *src, char *dest, size_t len) {
  char *p = dest;
  char *end = dest + len - 1;
  int n = (int) strlen(src);
  for (int i = 0; i < n && p < end; i++) {
    char ch = 0;
    switch (src[i]) {
      case '"': ch = '"'; goto l_escape;
      case '\\': ch = '\\'; goto l_escape;
      case '\b': ch = 'b'; goto l_escape;
      case '\f': ch = 'f'; goto l_escape;
      case '\n': ch = 'n'; goto l_escape;
      case '\r': ch = 'r'; goto l_escape;
      case '\t': {
        ch = 't';
      l_escape:
        if (p + 2 > end) goto l_done;
        *p++ = '\\';
        *p++ = ch;
      } break;
      default: {
        if (isprint(src[i]))
          *p++ = src[i];
        else
          goto l_done;
      }
    }
  }
l_done:
  *p = 0;
}

static json_value *get_array_value(json_value *js, const char *key, FILE *log) {
  (void)log;
  if (js->type != json_object) { return NULL; }
  for (int i = 0; i < js->u.object.length; i++) {
    json_object_entry *value = &js->u.object.values[i];
    if (strncmp(value->name, key, value->name_length) == 0) {
      if (value->value->type == json_array) { return value->value; }
      else {
        return NULL;
      }
    }
  }
  return NULL;
}

#if 0
static void dump_keys(json_value *js, FILE *log) {
  (void)log;
  if (js->type != json_object) return;
  for (int i = 0; i < js->u.object.length; i++) {
    json_object_entry *value = &js->u.object.values[i];
    fprintf(log, "key '%s'\n", value->name);
  }
}
#endif

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
    if (strcmp(cmd, commands[i].cmdname) == 0) return commands[i].code;
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
    fprintf(log, "Unknown adapterID '%s' in initialize command\n",
            adapterID != NULL ? adapterID : "null");
    return VSCODE_UNKNOWN_REQUEST;
  }
  ravi_string_copy(msg->u.Request.u.InitializeRequest.adapterID, adapterID,
                   sizeof msg->u.Request.u.InitializeRequest.adapterID);
  const char *pathFormat = get_string_value(args, "pathFormat", log);
  if (pathFormat != NULL && strcmp(pathFormat, "path") != 0) {
    fprintf(log, "Unsupported pathFormat '%s' in initialize command\n",
            pathFormat != NULL ? pathFormat : "null");
    return VSCODE_UNKNOWN_REQUEST;
  }
  if (pathFormat)
    ravi_string_copy(msg->u.Request.u.InitializeRequest.pathFormat, pathFormat,
                     sizeof msg->u.Request.u.InitializeRequest.pathFormat);
  int found = 0;
  msg->u.Request.u.InitializeRequest.columnsStartAt1 =
      get_boolean_value(args, "columnsStartAt1", log, &found);
  msg->u.Request.u.InitializeRequest.linesStartAt1 =
      get_boolean_value(args, "linesStartAt1", log, &found);
  msg->u.Request.request_type = VSCODE_INITIALIZE_REQUEST;
  return VSCODE_INITIALIZE_REQUEST;
}

/* Parse VSCode request message which has an integer parameter
 */
static int vscode_parse_intarg_request(json_value *js, ProtocolMessage *msg,
                                       int msgtype, const char *key,
                                       FILE *log) {
  json_value *args = get_object_value(js, "arguments", log);
  if (args == NULL) return VSCODE_UNKNOWN_REQUEST;
  int found = 0;
  msg->u.Request.u.CommonIntRequest.integer =
      get_int_value(args, key, log, &found);
  msg->u.Request.request_type = msgtype;
  return msgtype;
}

/* Parse launch request
 */
static int vscode_parse_launch_request(json_value *js, ProtocolMessage *msg,
                                       int msgtype, FILE *log) {
  json_value *args = get_object_value(js, "arguments", log);
  if (args == NULL) return VSCODE_UNKNOWN_REQUEST;
  int found = 0;
  msg->u.Request.u.LaunchRequest.noDebug =
      get_boolean_value(args, "noDebug", log, &found);
  msg->u.Request.u.LaunchRequest.stopOnEntry =
      get_boolean_value(args, "stopOnEntry", log, &found);
  const char *prog = get_string_value(args, "program", log);
  if (prog == NULL) return VSCODE_UNKNOWN_REQUEST;
  ravi_string_copy(msg->u.Request.u.LaunchRequest.program, prog,
                   sizeof msg->u.Request.u.LaunchRequest.program);
  for (char *cp = msg->u.Request.u.LaunchRequest.program; *cp; cp++) {
    /* replace back slashes with front slash as the VSCode front end doesn't
     * like
     * back slashes even though it sends them!
     */
    if (*cp == '\\') *cp = '/';
  }
  fprintf(log, "LAUNCH %s\n", prog);
  const char *lua_path = get_string_value(args, "LUA_PATH", log);
  if (lua_path != NULL) ravi_string_copy(msg->u.Request.u.LaunchRequest.lua_path, lua_path,
    sizeof msg->u.Request.u.LaunchRequest.lua_path);
  const char *lua_cpath = get_string_value(args, "LUA_CPATH", log);
  if (lua_cpath != NULL) ravi_string_copy(msg->u.Request.u.LaunchRequest.lua_cpath, lua_cpath,
    sizeof msg->u.Request.u.LaunchRequest.lua_cpath);
  const char *cwd = get_string_value(args, "cwd", log);
  if (cwd != NULL) ravi_string_copy(msg->u.Request.u.LaunchRequest.cwd, cwd,
    sizeof msg->u.Request.u.LaunchRequest.cwd);
  for (char *cp = msg->u.Request.u.LaunchRequest.cwd; *cp; cp++) {
    /* replace back slashes with front slash as the VSCode front end doesn't
    * like
    * back slashes even though it sends them!
    */
    if (*cp == '\\') *cp = '/';
  }
  msg->u.Request.request_type = msgtype;
  return msgtype;
}

static int vscode_parse_set_breakpoints_request(json_value *js,
                                                ProtocolMessage *msg,
                                                int msgtype, FILE *log) {
  int found = 0;
  json_value *args = get_object_value(js, "arguments", log);
  if (args == NULL) return VSCODE_UNKNOWN_REQUEST;
  json_value *source = get_object_value(args, "source", log);
  if (source == NULL) return VSCODE_UNKNOWN_REQUEST;
  const char *prog = get_string_value(source, "path", log);
  if (prog == NULL) return VSCODE_UNKNOWN_REQUEST;
  ravi_string_copy(msg->u.Request.u.SetBreakpointsRequest.source.path, prog,
                   sizeof msg->u.Request.u.SetBreakpointsRequest.source.path);
  for (char *cp = msg->u.Request.u.SetBreakpointsRequest.source.path; *cp;
       cp++) {
    if (*cp == '\\') *cp = '/';
  }
  json_value *breakpoints = get_array_value(args, "breakpoints", log);
  if (breakpoints == NULL || breakpoints->type != json_array)
    return VSCODE_UNKNOWN_REQUEST;
  for (int i = 0; i < breakpoints->u.array.length && i < MAX_BREAKPOINTS; i++) {
    json_value *element = breakpoints->u.array.values[i];
    if (element->type != json_object) return VSCODE_UNKNOWN_REQUEST;
    int line = get_int_value(element, "line", log, &found);
    fprintf(log, "Set breakpoint line = %d\n", line);
    msg->u.Request.u.SetBreakpointsRequest.breakpoints[i].line =
        found ? line : -1;
  }
  msg->u.Request.request_type = msgtype;
  return msgtype;
}

static int vscode_parse_stack_trace_request(json_value *js,
                                            ProtocolMessage *msg, int msgtype,
                                            FILE *log) {
  json_value *args = get_object_value(js, "arguments", log);
  if (args == NULL) return VSCODE_UNKNOWN_REQUEST;
  int found = 0;
  msg->u.Request.u.StackTraceRequest.threadId =
      get_int_value(args, "threadId", log, &found);
  msg->u.Request.u.StackTraceRequest.levels =
      get_int_value(args, "levels", log, &found);
  msg->u.Request.u.StackTraceRequest.startFrame =
      get_int_value(args, "startFrame", log, &found);
  msg->u.Request.request_type = msgtype;
  return msgtype;
}

/* Parse a VSCode request */
static int vscode_parse_request(json_value *js, ProtocolMessage *msg,
                                FILE *log) {
  const char *cmd = get_string_value(js, "command", log);
  int found = 0;
  if (cmd == NULL) return VSCODE_UNKNOWN_REQUEST;
  msg->type = VSCODE_REQUEST;
  msg->seq = get_int_value(js, "seq", log, &found);
  ravi_string_copy(msg->u.Request.command, cmd, sizeof msg->u.Request.command);
  fprintf(log, "\nRequest --> '%s'\n", cmd);
  int cmdtype = get_cmdtype(msg->u.Request.command);
  switch (cmdtype) {
    case VSCODE_INITIALIZE_REQUEST:
      return vscode_parse_initialize_request(js, msg, log);
    case VSCODE_DISCONNECT_REQUEST:
    case VSCODE_ATTACH_REQUEST:
    case VSCODE_CONFIGURATION_DONE_REQUEST:
    case VSCODE_THREAD_REQUEST: msg->u.Request.request_type = cmdtype; break;
    case VSCODE_CONTINUE_REQUEST:
    case VSCODE_NEXT_REQUEST:
    case VSCODE_STEPIN_REQUEST:
    case VSCODE_STEPOUT_REQUEST:
    case VSCODE_PAUSE_REQUEST:
      return vscode_parse_intarg_request(js, msg, cmdtype, "threadId", log);
    case VSCODE_SCOPES_REQUEST:
      return vscode_parse_intarg_request(js, msg, cmdtype, "frameId", log);
    case VSCODE_VARIABLES_REQUEST:
      return vscode_parse_intarg_request(js, msg, cmdtype, "variablesReference",
                                         log);
    case VSCODE_SOURCE_REQUEST:
      return vscode_parse_intarg_request(js, msg, cmdtype, "sourceReference",
                                         log);
    case VSCODE_LAUNCH_REQUEST:
      return vscode_parse_launch_request(js, msg, cmdtype, log);
    case VSCODE_STACK_TRACE_REQUEST:
      return vscode_parse_stack_trace_request(js, msg, cmdtype, log);
    case VSCODE_SET_EXCEPTION_BREAKPOINTS_REQUEST:
      msg->u.Request.request_type = cmdtype;
      break;
    case VSCODE_SET_BREAKPOINTS_REQUEST:
      return vscode_parse_set_breakpoints_request(js, msg, cmdtype, log);
    case VSCODE_UNKNOWN_REQUEST: break;
    default: msg->u.Request.request_type = cmdtype;
  }
  return cmdtype;
}

int vscode_parse_message(char *buf, size_t len, ProtocolMessage *msg,
                         FILE *log) {
  memset(msg, 0, sizeof(ProtocolMessage));
  json_value *js = json_parse(buf, len);
  if (js == NULL) return VSCODE_UNKNOWN;

  int msgtype = vscode_message_type(js, log);
  if (msgtype == VSCODE_REQUEST) msgtype = vscode_parse_request(js, msg, log);
  json_value_free(js);
  return msgtype;
}

static int seq = 1;

void vscode_make_error_response(ProtocolMessage *req, ProtocolMessage *res,
                                int restype, const char *errormsg) {
  memset(res, 0, sizeof(ProtocolMessage));
  res->seq = seq++;
  res->type = VSCODE_RESPONSE;
  ravi_string_copy(res->u.Response.command, req->u.Request.command,
                   sizeof res->u.Response.command);
  res->u.Response.request_seq = req->seq;
  res->u.Response.response_type = restype;
  ravi_string_copy(res->u.Response.message, errormsg,
                   sizeof res->u.Response.message);
  res->u.Response.success = 0;
}

void vscode_make_success_response(ProtocolMessage *req, ProtocolMessage *res,
                                  int restype) {
  memset(res, 0, sizeof(ProtocolMessage));
  res->seq = seq++;
  res->type = VSCODE_RESPONSE;
  ravi_string_copy(res->u.Response.command, req->u.Request.command,
                   sizeof res->u.Response.command);
  res->u.Response.request_seq = req->seq;
  res->u.Response.response_type = restype;
  res->u.Response.success = 1;
}

void vscode_make_initialized_event(ProtocolMessage *res) {
  memset(res, 0, sizeof(ProtocolMessage));
  res->seq = seq++;
  res->type = VSCODE_EVENT;
  ravi_string_copy(res->u.Event.event, "initialized",
                   sizeof res->u.Event.event);
  res->u.Event.event_type = VSCODE_INITIALIZED_EVENT;
}

static void vscode_make_output_event(ProtocolMessage *res, const char *category,
                                     const char *msg) {
  memset(res, 0, sizeof(ProtocolMessage));
  res->seq = seq++;
  res->type = VSCODE_EVENT;
  ravi_string_copy(res->u.Event.event, "output", sizeof res->u.Event.event);
  ravi_string_copy(res->u.Event.u.OutputEvent.category, category,
                   sizeof res->u.Event.u.OutputEvent.category);
  vscode_json_stringify(msg, res->u.Event.u.OutputEvent.output,
                        sizeof res->u.Event.u.OutputEvent.output);
  res->u.Event.event_type = VSCODE_OUTPUT_EVENT;
}

/*
* Build a StoppedEvent {event, reason, threadId}
*/
void vscode_make_stopped_event(ProtocolMessage *res, const char *reason) {
  memset(res, 0, sizeof(ProtocolMessage));
  res->seq = seq++;
  res->type = VSCODE_EVENT;
  res->u.Event.event_type = VSCODE_STOPPED_EVENT;
  ravi_string_copy(res->u.Event.event, "stopped", sizeof res->u.Event.event);
  res->u.Event.u.StoppedEvent.threadId = 1; /* dummy thread id - always 1 */
  ravi_string_copy(res->u.Event.u.StoppedEvent.reason, reason,
                   sizeof res->u.Event.u.StoppedEvent.reason);
}

/*
* Build a TerminatedEvent
*/
void vscode_make_terminated_event(ProtocolMessage *res) {
  memset(res, 0, sizeof(ProtocolMessage));
  res->seq = seq++;
  res->type = VSCODE_EVENT;
  res->u.Event.event_type = VSCODE_TERMINATED_EVENT;
  ravi_string_copy(res->u.Event.event, "terminated", sizeof res->u.Event.event);
  res->u.Event.u.TerminatedEvent.restart = 0;
}

/*
* Build a ThreadEvent  {event, reason, threadid}
*/
void vscode_make_thread_event(ProtocolMessage *res, bool started) {
  memset(res, 0, sizeof(ProtocolMessage));
  res->seq = seq++;
  res->type = VSCODE_EVENT;
  res->u.Event.event_type = VSCODE_THREAD_EVENT;
  ravi_string_copy(res->u.Event.event, "thread", sizeof res->u.Event.event);
  res->u.Event.u.ThreadEvent.threadId = 1; /* dummy thread id - always 1 */
  ravi_string_copy(res->u.Event.u.ThreadEvent.reason,
                   started ? "started" : "exited",
                   sizeof res->u.Event.u.ThreadEvent.reason);
}

void vscode_serialize_response(char *buf, size_t buflen, ProtocolMessage *res) {
  char temp[1024 * 8] = {0};
  char *cp = temp;
  buf[0] = 0;
  if (res->type != VSCODE_RESPONSE) return;
  snprintf(cp, sizeof temp - strlen(temp),
           "{\"type\":\"response\",\"seq\":%d,\"command\":\"%s\",\"request_"
           "seq\":%d,\"success\":%s",
           res->seq, res->u.Response.command, res->u.Response.request_seq,
           res->u.Response.success ? "true" : "false");
  cp = temp + strlen(temp);
  if (res->u.Response.message[0]) {
    snprintf(cp, sizeof temp - strlen(temp), ",\"message\":\"%s\"",
             res->u.Response.message);
    cp = temp + strlen(temp);
  }
  if (res->u.Response.response_type == VSCODE_INITIALIZE_RESPONSE &&
      res->u.Response.success) {
    snprintf(
        cp, sizeof temp - strlen(temp),
        ",\"body\":{\"supportsConfigurationDoneRequest\":%s,"
        "\"supportsFunctionBreakpoints\":%s,\"supportsConditionalBreakpoints\":"
        "%s,\"supportsEvaluateForHovers\":%s}",
        res->u.Response.u.InitializeResponse.body
                .supportsConfigurationDoneRequest
            ? "true"
            : "false",
        res->u.Response.u.InitializeResponse.body.supportsFunctionBreakpoints
            ? "true"
            : "false",
        res->u.Response.u.InitializeResponse.body.supportsConditionalBreakpoints
            ? "true"
            : "false",
        res->u.Response.u.InitializeResponse.body.supportsEvaluateForHovers
            ? "true"
            : "false");
    cp = temp + strlen(temp);
  }
  else if (res->u.Response.response_type == VSCODE_THREAD_RESPONSE &&
           res->u.Response.success) {
    snprintf(cp, sizeof temp - strlen(temp),
             ",\"body\":{\"threads\":[{\"name\":\"%s\",\"id\":%d}]}",
             res->u.Response.u.ThreadResponse.threads[0].name,
             res->u.Response.u.ThreadResponse.threads[0].id);
    cp = temp + strlen(temp);
  }
  else if (res->u.Response.response_type == VSCODE_STACK_TRACE_RESPONSE &&
           res->u.Response.success) {
    snprintf(cp, sizeof temp - strlen(temp),
             ",\"body\":{\"totalFrames\":%d,\"stackFrames\":[",
             res->u.Response.u.StackTraceResponse.totalFrames);
    cp = temp + strlen(temp);
    int d = 0;
    for (; d < res->u.Response.u.StackTraceResponse.totalFrames; d++) {
      if (d) {
        snprintf(cp, sizeof temp - strlen(temp), ",");
        cp = temp + strlen(temp);
      }
      if (res->u.Response.u.StackTraceResponse.stackFrames[d]
              .source.sourceReference == 0) {
        snprintf(
            cp, sizeof temp - strlen(temp),
            "{\"id\":%d,\"name\":\"%s\",\"column\":1,\"line\":%d,\"source\":"
            "{\"name\":\"%s\",\"path\":\"%s\",\"sourceReference\":0}}",
            d, res->u.Response.u.StackTraceResponse.stackFrames[d].name,
            res->u.Response.u.StackTraceResponse.stackFrames[d].line,
            res->u.Response.u.StackTraceResponse.stackFrames[d].source.name,
            res->u.Response.u.StackTraceResponse.stackFrames[d].source.path);
        cp = temp + strlen(temp);
      }
      else {
        snprintf(
            cp, sizeof temp - strlen(temp),
            "{\"id\":%d,\"name\":\"%s\",\"column\":1,\"line\":%d,\"source\":"
            "{\"name\":\"%s\",\"sourceReference\":%d}}",
            d, res->u.Response.u.StackTraceResponse.stackFrames[d].name,
            res->u.Response.u.StackTraceResponse.stackFrames[d].line,
            res->u.Response.u.StackTraceResponse.stackFrames[d].source.name,
            res->u.Response.u.StackTraceResponse.stackFrames[d]
                .source.sourceReference);
        cp = temp + strlen(temp);
      }
    }
    snprintf(cp, sizeof temp - strlen(temp), "]}");
    cp = temp + strlen(temp);
  }
  else if (res->u.Response.response_type == VSCODE_SCOPES_RESPONSE &&
           res->u.Response.success) {
    snprintf(cp, sizeof temp - strlen(temp), ",\"body\":{\"scopes\":[");
    cp = temp + strlen(temp);
    for (int d = 0; d < MAX_SCOPES; d++) {
      if (!res->u.Response.u.ScopesResponse.scopes[d].name[0]) break;
      if (d) {
        snprintf(cp, sizeof temp - strlen(temp), ",");
        cp = temp + strlen(temp);
      }
      snprintf(cp, sizeof temp - strlen(temp),
               "{\"name\":\"%s\",\"variablesReference\":%d,\"expensive\":%s}",
               res->u.Response.u.ScopesResponse.scopes[d].name,
               res->u.Response.u.ScopesResponse.scopes[d].variablesReference,
               res->u.Response.u.ScopesResponse.scopes[d].expensive ? "true"
                                                                    : "false");
      cp = temp + strlen(temp);
    }
    snprintf(cp, sizeof temp - strlen(temp), "]}");
    cp = temp + strlen(temp);
  }
  else if (res->u.Response.response_type == VSCODE_VARIABLES_RESPONSE &&
           res->u.Response.success) {
    snprintf(cp, sizeof temp - strlen(temp), ",\"body\":{\"variables\":[");
    cp = temp + strlen(temp);
    for (int d = 0; d < MAX_VARIABLES; d++) {
      if (!res->u.Response.u.VariablesResponse.variables[d].name[0]) break;
      if (d) {
        snprintf(cp, sizeof temp - strlen(temp), ",");
        cp = temp + strlen(temp);
      }
      snprintf(
          cp, sizeof temp - strlen(temp),
          "{\"name\":\"%s\",\"variablesReference\":%d,\"value\":\"%s\"}",
          res->u.Response.u.VariablesResponse.variables[d].name,
          res->u.Response.u.VariablesResponse.variables[d].variablesReference,
          res->u.Response.u.VariablesResponse.variables[d].value);
      cp = temp + strlen(temp);
    }
    snprintf(cp, sizeof temp - strlen(temp), "]}");
    cp = temp + strlen(temp);
  }
  else if (res->u.Response.response_type == VSCODE_SET_BREAKPOINTS_RESPONSE &&
           res->u.Response.success) {
    snprintf(cp, sizeof temp - strlen(temp), ",\"body\":{\"breakpoints\":[");
    cp = temp + strlen(temp);
    for (int d = 0; d < MAX_BREAKPOINTS; d++) {
      if (!res->u.Response.u.SetBreakpointsResponse.breakpoints[d]
               .source.path[0])
        break;
      if (d) {
        snprintf(cp, sizeof temp - strlen(temp), ",");
        cp = temp + strlen(temp);
      }
      snprintf(
          cp, sizeof temp - strlen(temp),
          "{\"verified\":%s,\"source\":{\"path\":\"%s\"},\"line\":%d}",
          res->u.Response.u.SetBreakpointsResponse.breakpoints[d].verified
              ? "true"
              : "false",
          res->u.Response.u.SetBreakpointsResponse.breakpoints[d].source.path,
          res->u.Response.u.SetBreakpointsResponse.breakpoints[d].line);
      cp = temp + strlen(temp);
    }
    snprintf(cp, sizeof temp - strlen(temp), "]}");
    cp = temp + strlen(temp);
  }
  else if (res->u.Response.response_type == VSCODE_SOURCE_RESPONSE &&
           res->u.Response.success) {
    char buf[SOURCE_LEN * 2];
    vscode_json_stringify(res->u.Response.u.SourceResponse.content, buf,
                          sizeof buf);
    snprintf(cp, sizeof temp - strlen(temp), ",\"body\":{\"content\":\"%s\"}",
             buf);
    cp = temp + strlen(temp);
  }
  snprintf(cp, sizeof temp - strlen(temp), "}");
  snprintf(buf, buflen, temp);
}

/*
* Create a serialized form of the event in VSCode
* wire protocol format (see protocol.h)
*/
void vscode_serialize_event(char *buf, size_t buflen, ProtocolMessage *res) {
  char temp[1024] = {0};
  char *cp = temp;
  buf[0] = 0;
  if (res->type != VSCODE_EVENT) return;
  snprintf(cp, sizeof temp - strlen(temp),
           "{\"type\":\"event\",\"seq\":%d,\"event\":\"%s\"", res->seq,
           res->u.Event.event);
  cp = temp + strlen(temp);
  if (res->u.Event.event_type == VSCODE_OUTPUT_EVENT) {
    snprintf(cp, sizeof temp - strlen(temp),
             ",\"body\":{\"category\":\"%s\",\"output\":\"%s\"}",
             res->u.Event.u.OutputEvent.category,
             res->u.Event.u.OutputEvent.output);
    cp = temp + strlen(temp);
  }
  else if (res->u.Event.event_type == VSCODE_THREAD_EVENT) {
    snprintf(cp, sizeof temp - strlen(temp),
             ",\"body\":{\"reason\":\"%s\",\"threadId\":%d}",
             res->u.Event.u.ThreadEvent.reason,
             res->u.Event.u.ThreadEvent.threadId);
    cp = temp + strlen(temp);
  }
  else if (res->u.Event.event_type == VSCODE_STOPPED_EVENT) {
    snprintf(cp, sizeof temp - strlen(temp),
             ",\"body\":{\"reason\":\"%s\",\"threadId\":%d}",
             res->u.Event.u.StoppedEvent.reason,
             res->u.Event.u.StoppedEvent.threadId);
    cp = temp + strlen(temp);
  }
  else if (res->u.Event.event_type == VSCODE_TERMINATED_EVENT) {
    snprintf(cp, sizeof temp - strlen(temp), ",\"body\":{\"restart\":%s}",
             res->u.Event.u.TerminatedEvent.restart ? "true" : "false");
    cp = temp + strlen(temp);
  }
  snprintf(cp, sizeof temp - strlen(temp), "}");
  snprintf(buf, buflen, temp);
}

void vscode_send(ProtocolMessage *msg, FILE *out, FILE *log) {
  char buf[4096];
  char json_buf[sizeof buf + 30];
  if (msg->type == VSCODE_EVENT)
    vscode_serialize_event(buf, sizeof buf, msg);
  else if (msg->type == VSCODE_RESPONSE)
    vscode_serialize_response(buf, sizeof buf, msg);
  else
    return;
  snprintf(json_buf, sizeof json_buf, "Content-Length: %d\r\n\r\n%s",
           (int)strlen(buf), buf);
  fprintf(log, "%s\n", json_buf);
  fprintf(out, "%s", json_buf);
}

/*
* Send VSCode a StoppedEvent
* The event indicates that the execution of the debuggee has stopped due to some
* condition.
* This can be caused by a break point previously set, a stepping action has
* completed, by executing a debugger statement etc.
*/
void vscode_send_stopped_event(ProtocolMessage *res, const char *msg, FILE *out,
                               FILE *log) {
  vscode_make_stopped_event(res, msg);
  vscode_send(res, out, log);
}

/*
* Send VSCode a ThreadEvent
*/
void vscode_send_thread_event(ProtocolMessage *res, bool started, FILE *out,
                              FILE *log) {
  vscode_make_thread_event(res, started);
  vscode_send(res, out, log);
}

/*
* Send VSCode a TerminatedEvent
*/
void vscode_send_terminated_event(ProtocolMessage *res, FILE *out, FILE *log) {
  vscode_make_terminated_event(res);
  vscode_send(res, out, log);
}

void vscode_send_output_event(ProtocolMessage *res, const char *category,
                              const char *msg, FILE *out, FILE *log) {
  vscode_make_output_event(res, category, msg);
  vscode_send(res, out, log);
}

void vscode_send_error_response(ProtocolMessage *req, ProtocolMessage *res,
                                int responseType, const char *msg, FILE *out,
                                FILE *log) {
  vscode_make_error_response(req, res, responseType, msg);
  vscode_send(res, out, log);
}

void vscode_send_success_response(ProtocolMessage *req, ProtocolMessage *res,
                                  int responseType, FILE *out, FILE *log) {
  vscode_make_success_response(req, res, responseType);
  vscode_send(res, out, log);
}

/*
* Get command from VSCode
* VSCode commands begin with the sequence:
* 'Content-Length: nnn\r\n\r\n'
* This is followed by nnn bytes which has a JSON
* format request message
* We currently don't bother checking the
* \r\n\r\n sequence for incoming messages
*/
int vscode_get_request(FILE *in, ProtocolMessage *req, FILE *log) {
  char buf[4096] = {0};
  if (fgets(buf, sizeof buf, in) != NULL) {
    buf[sizeof buf - 1] = 0; /* NULL terminate - just in case */
    const char *bufp = strstr(buf, "Content-Length: ");
    if (bufp != NULL) {
      bufp += 16;
      /* get the message length */
      int len = atoi(bufp);
      if (len >= (int)(sizeof buf)) {
        /* FIXME */
        fprintf(log,
                "FATAL ERROR - Content-Length = %d is greater than bufsize\n",
                len);
        exit(1);
      }
      buf[0] = 0;
      /* skip blank line - actually \r\n */
      if (fgets(buf, sizeof buf, stdin) == NULL) {
        fprintf(log, "FATAL ERROR - cannot read %d bytes\n", len);
        exit(1);
      }
      /* Now read exact number of characters */
      buf[0] = 0;
      if (fread(buf, len, 1, stdin) != 1) {
        fprintf(log, "FATAL ERROR - cannot read %d bytes\n", len);
        exit(1);
      }
      buf[len] = 0;
      fprintf(log, "Content-Length: %d\r\n\r\n%s", len, buf);
      fflush(log);
      return vscode_parse_message(buf, sizeof buf, req, log);
    }
    else {
      return VSCODE_EOF;
    }
  }
  else {
    return VSCODE_EOF;
  }
}

void ravi_string_copy(char *buf, const char *src, size_t buflen) {
  strncpy(buf, src, buflen);
  buf[buflen - 1] = 0;
}