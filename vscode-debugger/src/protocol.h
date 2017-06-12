/*
 ** See LICENSE Notice in lua.h
 ** Copyright (C) 2015-2016 Dibyendu Majumdar
 */
#ifndef RAVI_VSCODE_PROTOCOL_H
#define RAVI_VSCODE_PROTOCOL_H

#include "json.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

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
  MAX_STACK_FRAMES = 50,
  MAX_SCOPES = 4,
  MAX_VARIABLES = 251,
  MAX_THREADS = 1,
  SOURCE_LEN = 8196,
  MAX_TOTAL_BREAKPOINTS = 20
};

/*
The max integer that can be transferred to a Javascript number (and hence JSON
number) is 2^53-1 which is equal to 9007199254740991. This takes up 53 bits.
The integer 4503599627370495 takes up 52 bits - so that means 6 8-bit values and 1
4-bit value can be accomodated.
*/
typedef struct {
  unsigned int x8[5];
  unsigned int depth;
  unsigned int vartype;
} PackedInteger;

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
  int64_t sourceReference;
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
  int64_t variablesReference;
  int expensive;
} Scope;

typedef struct {
  char name[NAME_LEN];
  char type[NAME_LEN];
  char value[VALUE_LEN];
  int64_t variablesReference;
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
  int64_t seq;

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
          char lua_path[PATH_LEN];
          char lua_cpath[PATH_LEN];
          char cwd[PATH_LEN];
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
          int64_t integer64;
        } CommonInt64Request;
        
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
          int64_t variablesReference;
        } VariablesRequest;

        struct {
          int64_t sourceReference;
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
      int64_t request_seq;
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
          Variable variables[MAX_VARIABLES+1];
        } VariablesResponse;

        struct {
          char content[SOURCE_LEN];
        } SourceResponse;

        struct {
          Thread threads[MAX_THREADS];
        } ThreadResponse;

        struct {
          char result[TEXT_LEN];
          int64_t variablesReference;
        } EvaluateResponse;

      } u;

    } Response;

  } u;

} ProtocolMessage;


typedef struct {
  char *buf;
  size_t allocated_size;
  size_t pos;
} membuff_t;

extern void membuff_init(membuff_t *mb, size_t initial_size);
extern void membuff_rewindpos(membuff_t *mb);
extern void membuff_resize(membuff_t *mb, size_t new_size);
extern void membuff_free(membuff_t *mb);
extern void membuff_add_string(membuff_t *mb, const char *str);
extern void membuff_add_bool(membuff_t *mb, bool value);
extern void membuff_add_int(membuff_t *mb, int value);
extern void membuff_add_longlong(membuff_t *mb, int64_t value);

extern int vscode_parse_message(char *buf, size_t len, ProtocolMessage *msg,
                                FILE *log);
extern void vscode_make_error_response(ProtocolMessage *req,
                                       ProtocolMessage *res, int restype,
                                       const char *errormsg);
extern void vscode_make_success_response(ProtocolMessage *req,
                                         ProtocolMessage *res, int restype);
extern void vscode_serialize_response_new(membuff_t *mb, ProtocolMessage *res);
extern void vscode_serialize_event_new(membuff_t *mb, ProtocolMessage *res);
extern void vscode_make_initialized_event(ProtocolMessage *res);
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
extern void vscode_send_output_event(ProtocolMessage *res, const char *category, const char *msg,
                                     FILE *out, FILE *log);
extern void vscode_send_error_response(ProtocolMessage *req,
                                       ProtocolMessage *res, int responseType,
                                       const char *msg, FILE *out, FILE *log);
extern void vscode_send_success_response(ProtocolMessage *req,
                                         ProtocolMessage *res, int responseType,
                                         FILE *out, FILE *log);
extern int vscode_get_request(FILE *in, membuff_t *mb, ProtocolMessage *req, FILE *log);
extern void vscode_json_stringify(const char *src, char *dest, size_t len);

/* guaranteed null termination */
extern void vscode_string_copy(char *buf, const char *src, size_t buflen);
extern int64_t vscode_pack(PackedInteger *pi);
extern void vscode_unpack(int64_t i, PackedInteger *pi);

#endif
