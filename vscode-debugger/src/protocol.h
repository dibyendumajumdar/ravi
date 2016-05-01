#ifndef RAVI_VSCODE_PROTOCOL_H
#define RAVI_VSCODE_PROTOCOL_H

#include "json.h"

#include <stdbool.h>
#include <stdio.h>

enum {
  VSCODE_REQUEST = 1,
  VSCODE_EVENT = 2,
  VSCODE_RESPONSE = 3,
  VSCODE_UNKNOWN = 4
};

enum {
  VSCODE_INITIALIZED_EVENT = 1,
  VSCODE_STOPPED_EVENT = 2,
  VSCODE_EXITED_EVENT = 3,
  VSCODE_TERMINATED_EVENT = 4,
  VSCODE_THREAD_EVENT = 5,
  VSCODE_OUTPUT_EVENT = 6,
  VSCODE_BREAKPOINT_EVENT = 7,
};

enum {
  VSCODE_ERROR_RESPONSE = 1,
};

enum {
  VSCODE_EOF = -2,
  VSCODE_UNKNOWN_REQUEST = -1,
  VSCODE_INITIALIZE_REQUEST = 1,
  VSCODE_CONFIGURATION_DONE_REQUEST = 2,
  VSCODE_LAUNCH_REQUEST = 3,
  VSCODE_ATTACH_REQUEST = 4,
  VSCODE_DISCONNECT_REQUEST = 5,
  VSCODE_SET_BREAKPOINTS_REQUEST = 6,
  VSCODE_SET_FUNCTION_BREAKPOINTS_REQUEST = 7,
  VSCODE_SET_EXCEPTION_BREAKPOINTS_REQUEST = 8,
  VSCODE_CONTINUE_REQUEST = 9,
  VSCODE_NEXT_REQUEST = 10,
  VSCODE_STEPIN_REQUEST = 11,
  VSCODE_STEPOUT_REQUEST = 12,
  VSCODE_PAUSE_REQUEST = 13,
  VSCODE_STACK_TRACE_REQUEST = 14,
  VSCODE_SCOPES_REQUEST = 15,
  VSCODE_VARIABLES_REQUEST = 16,
  VSCODE_SOURCE_REQUEST = 17,
  VSCODE_THREAD_REQUEST = 18,
  VSCODE_EVALUATE_REQUEST = 19,
};

enum {
  VSCODE_INITIALIZE_RESPONSE = 1,
  VSCODE_CONFIGURATION_DONE_RESPONSE = 2,
  VSCODE_LAUNCH_RESPONSE = 3,
  VSCODE_ATTACH_RESPONSE = 4,
  VSCODE_DISCONNECT_RESPONSE = 5,
  VSCODE_SET_BREAKPOINTS_RESPONSE = 6,
  VSCODE_SET_FUNCTION_BREAKPOINTS_RESPONSE = 7,
  VSCODE_SET_EXCEPTION_BREAKPOINTS_RESPONSE = 8,
  VSCODE_CONTINUE_RESPONSE = 9,
  VSCODE_NEXT_RESPONSE = 10,
  VSCODE_STEPIN_RESPONSE = 11,
  VSCODE_STEPOUT_RESPONSE = 12,
  VSCODE_PAUSE_RESPONSE = 13,
  VSCODE_STACK_TRACE_RESPONSE = 14,
  VSCODE_SCOPES_RESPONSE = 15,
  VSCODE_VARIABLES_RESPONSE = 16,
  VSCODE_SOURCE_RESPONSE = 17,
  VSCODE_THREAD_RESPONSE = 18,
  VSCODE_EVALUATE_RESPONSE = 19,
};

enum {
  NAME_LEN = 50,
  PATH_LEN = 256,
  VALUE_LEN = 80,
  COMMAND_LEN = 30,
  EVENT_LEN = 30,
  REASON_LEN = 100,
  CATEGORY_LEN = 30,
  TEXT_LEN = 256,
  MAX_BREAKPOINTS = 5,
  MAX_STACK_FRAMES = 30,
  MAX_SCOPES = 3,
  MAX_VARIABLES = 120,
  MAX_THREADS = 10,
};

typedef struct {
  int id;
  char format[TEXT_LEN];
} Message;

typedef struct {
  int id;
  char name[NAME_LEN];
} Thread;

typedef struct {
  char name[NAME_LEN];
  char path[PATH_LEN];
  int sourceReference;
} Source;

typedef struct {
  int id;
  char name[NAME_LEN];
  Source source;
  int line;
  int column;
} StackFrame;

typedef struct {
  char name[NAME_LEN];
  int variablesReference;
  int expensive;
} Scope;

typedef struct {
  char name[NAME_LEN];
  char value[VALUE_LEN];
  int variablesReference;
} Variable;

typedef struct {
  int line;
  int column;
} SourceBreakpoint;

typedef struct { char name[NAME_LEN]; } FunctionBreakpoint;

typedef struct {
  int id;
  int verified;
  char message[TEXT_LEN];
  Source source;
  int line;
  int column;
} Breakpoint;

typedef struct {
  int supportsConfigurationDoneRequest;
  int supportsFunctionBreakpoints;
  int supportsConditionalBreakpoints;
  int supportsEvaluateForHovers;
} Capabilities;

/* See
 * https://github.com/Microsoft/vscode-debugadapter-node/blob/master/protocol/src/debugProtocol.ts
 */
typedef struct {
  int type;
  int seq;

  union {
    struct {
      int event_type;
      char event[EVENT_LEN];

      union {
        struct {
          char reason[REASON_LEN];
          int threadId;
          char text[TEXT_LEN];
          int allThreadsStopped;
        } StoppedEvent;

        struct {
          int exitCode;
        } ExitedEvent;

        struct {
          int restart;
        } TerminatedEvent;

        struct {
          char reason[REASON_LEN];
          int threadId;
        } ThreadEvent;

        struct {
          char category[CATEGORY_LEN];
          char output[TEXT_LEN];
        } OutputEvent;

        struct {
          char reason[TEXT_LEN];
          Breakpoint breakpoint;
        } BreakpointEvent;

      } u;
    } Event;

    struct {
      int request_type;
      char command[COMMAND_LEN];

      union {
        struct {
          char adapterID[NAME_LEN];
          int linesStartAt1;
          int columnsStartAt1;
          char pathFormat[NAME_LEN];
        } InitializeRequest;

        struct {
          int noDebug;
          char program[TEXT_LEN];
          int stopOnEntry;
        } LaunchRequest;

        struct {
          Source source;
          Breakpoint breakpoints[MAX_BREAKPOINTS];
        } SetBreakpointsRequest;

        struct {
          FunctionBreakpoint breakpoints[MAX_BREAKPOINTS];
        } SetFunctionBreakpointsRequest;

        struct {
          char filters[1][TEXT_LEN];
        } SetExceptionBreakpointsRequest;

        struct {
          int integer;
        } CommonIntRequest;

        struct {
          int threadId;
        } ContinueRequest;

        struct {
          int threadId;
        } NextRequest;

        struct {
          int threadId;
        } StepInRequest;

        struct {
          int threadId;
        } StepOutRequest;

        struct {
          int threadId;
        } PauseRequest;

        struct {
          int threadId;
          int startFrame;
          int levels;
        } StackTraceRequest;

        struct {
          int frameId;
        } ScopesRequest;

        struct {
          int variablesReference;
        } VariablesRequest;

        struct {
          int sourceReference;
        } SourceRequest;

        struct {
          char expression[TEXT_LEN];
          int frameId;
          char context[TEXT_LEN];
        } EvaluateRequest;

      } u;
    } Request;

    struct {
      int response_type;
      int request_seq;
      int success;
      char command[COMMAND_LEN];
      char message[TEXT_LEN];

      union {
        struct {
          char error[TEXT_LEN];
        } ErrorResponse;

        struct {
          Capabilities body;
        } InitializeResponse;

        struct {
          Breakpoint breakpoints[MAX_BREAKPOINTS];
        } SetBreakpointsResponse;

        struct {
          Breakpoint breakpoints[MAX_BREAKPOINTS];
        } SetFunctionBreakpointsResponse;

        struct {
          StackFrame stackFrames[MAX_STACK_FRAMES];
          int totalFrames;
        } StackTraceResponse;

        struct {
          Scope scopes[MAX_SCOPES];
        } ScopesResponse;

        struct {
          Variable variables[MAX_VARIABLES];
        } VariablesResponse;

        struct {
          char content[TEXT_LEN];
        } SourceResponse;

        struct {
          Thread threads[MAX_THREADS];
        } ThreadResponse;

        struct {
          char result[TEXT_LEN];
          int variablesReference;
        } EvaluateResponse;

      } u;

    } Response;

  } u;

} ProtocolMessage;

extern int vscode_parse_message(char *buf, size_t len, ProtocolMessage *msg,
                                FILE *log);
extern void vscode_make_error_response(ProtocolMessage *req,
                                       ProtocolMessage *res, int restype,
                                       const char *errormsg);
extern void vscode_make_success_response(ProtocolMessage *req,
                                         ProtocolMessage *res, int restype);
extern void vscode_serialize_response(char *buf, size_t buflen,
                                      ProtocolMessage *res);
extern void vscode_serialize_event(char *buf, size_t buflen,
                                   ProtocolMessage *res);
extern void vscode_make_initialized_event(ProtocolMessage *res);
extern void vscode_make_output_event(ProtocolMessage *res, const char *msg);
extern void vscode_make_stopped_event(ProtocolMessage *res, const char *reason);
extern void vscode_make_thread_event(ProtocolMessage *res, bool started);
extern void vscode_make_terminated_event(ProtocolMessage *res);
extern void vscode_send(ProtocolMessage *msg, FILE *out, FILE *log);
extern void vscode_send_stopped_event(ProtocolMessage *res, const char *msg,
                                      FILE *out, FILE *log);
extern void vscode_send_thread_event(ProtocolMessage *res, bool started,
                                     FILE *out, FILE *log);
extern void vscode_send_terminated_event(ProtocolMessage *res, FILE *out,
                                         FILE *log);
extern void vscode_send_output_event(ProtocolMessage *res, const char *msg,
                                     FILE *out, FILE *log);
extern void vscode_send_error_response(ProtocolMessage *req,
                                       ProtocolMessage *res, int responseType,
                                       const char *msg, FILE *out, FILE *log);
extern void vscode_send_success_response(ProtocolMessage *req,
                                         ProtocolMessage *res, int responseType,
                                         FILE *out, FILE *log);
extern int vscode_get_request(FILE *in, ProtocolMessage *req, FILE *log);
extern void vscode_json_stringify(const char *src, char *dest, size_t len);

#endif