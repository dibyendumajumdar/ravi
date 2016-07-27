/**
 * Standalone Lua/Ravi interpreter that is meant to be used as
 * a debugger in the VSCode IDE.
 */

#include <assert.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#include <fcntl.h>
#include <io.h>
#define chdir _chdir
#else
#include <unistd.h>
#endif

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#include "protocol.h"

/* debugger state */
enum {
  DEBUGGER_BIRTH = 1, /* Initial state */
  DEBUGGER_INITIALIZED = 2, /* Have processed VSCode initialize request */
  DEBUGGER_PROGRAM_LAUNCHED = 3, /* Have processed VSCode launch request - Lua program running */
  DEBUGGER_PROGRAM_STEPPING = 4, /* In stepping mode */
  DEBUGGER_PROGRAM_RUNNING = 5, /* In continue mode - will stop at breakpoint */
  DEBUGGER_PROGRAM_STOPPED = 6, /* Have hit a break point */
  DEBUGGER_PROGRAM_TERMINATED = 7 /* Program completed */
};

/* sub states of DEBUGGER_PROGRAM_STEPPING */
enum {
  DEBUGGER_STEPPING_IN = 1, /* Step into */
  DEBUGGER_STEPPING_OVER = 2, /* Step over */
  DEBUGGER_STEPPING_OUT = 3 /* Step out of the current function */
};

/* Following values must fit into 2 bits and 0 is not a valid value */
enum { VAR_TYPE_LOCALS = 1, VAR_TYPE_UPVALUES = 2, VAR_TYPE_GLOBALS = 3 };

/* Some utilities to allow packing of 5 components into a 32-bit integer value
 * This is used to encode the stack frame, variable reference
 */
#define MASK0 0x3F
#define MASK 0xFF
#define MASK1 0x3

#define MAKENUM(t, a, b, c, d)                                                 \
  \
((((t)&MASK1) | (((a)&MASK0) << 2)) | (((b)&MASK) << 8) | (((c)&MASK) << 16) | \
      (((d)&MASK) << 24))

#define EXTRACT_T(x) ((x)&MASK1)
#define EXTRACT_A(x) (((x) >> 2) & MASK0)
#define EXTRACT_B(x) (((x) >> 8) & MASK)
#define EXTRACT_C(x) (((x) >> 16) & MASK)
#define EXTRACT_D(x) (((x) >> 24) & MASK)

typedef struct {
  int depth;
  int64_t sourceReference;
} SourceOnStack;

/*
 * These statics are temporary - eventually they will be moved to
 * the Lua global state; but right now while things are
 * evolving this is easier to work with.
 */
static FILE *my_logger = NULL;
static int thread_event_sent = 0; /* Set to 1 once we have sent a thread event to VSCode */
static int debugger_state = DEBUGGER_BIRTH; /* Debugger's state is tracked in this variable */
static Breakpoint breakpoints[MAX_TOTAL_BREAKPOINTS];
static ProtocolMessage req, res;
static ProtocolMessage output_response;
static char workingdir[1024];
/* Following is for tracking the dynamically generated sources */
static SourceOnStack sourceOnStack[MAX_STACK_FRAMES];
static int sourceOnStackCount = 0;
/* Following three are for the three differet stepping modes */
static int stepping_mode = DEBUGGER_STEPPING_IN; /* default */
static int stepping_stacklevel = -1; /* This tracks the stack level from where a step over or step out was requested */
static lua_State *stepping_lua_State = NULL; /* Tracks the Lua State that requested a step over or step out */
static membuff_t readbuf;

/*
* Generate response to InitializeRequest
* Send InitializedEvent
*/
static void handle_initialize_request(ProtocolMessage *req,
                                      ProtocolMessage *res, FILE *out) {
  if (debugger_state >= DEBUGGER_INITIALIZED) {
    vscode_send_error_response(req, res, VSCODE_INITIALIZE_RESPONSE,
                               "already initialized", out, my_logger);
    return;
  }
  /* Send InitializeResponse */
  vscode_make_success_response(req, res, VSCODE_INITIALIZE_RESPONSE);
  res->u.Response.u.InitializeResponse.body.supportsConfigurationDoneRequest =
      1;
  vscode_send(res, out, my_logger);

  /* Send InitializedEvent */
  /* From VSCode Adapter comment: InitializedEvent must not be sent before
   * InitializeRequest has returned its result */
  vscode_make_initialized_event(res);
  vscode_send(res, out, my_logger);

  /* Send notification */
  /* This is just for display by the front end */
  vscode_send_output_event(res, "console", "Debugger initialized\n", out,
                           my_logger);
  debugger_state = DEBUGGER_INITIALIZED;
}

/*
* Generate response to ThreadRequest
* We treat all Lua Threads as part of the same OS thread
* so we always returned a fixed thread identifier
*/
static void handle_thread_request(ProtocolMessage *req, ProtocolMessage *res,
                                  FILE *out) {
  vscode_make_success_response(req, res, VSCODE_THREAD_RESPONSE);
  res->u.Response.u.ThreadResponse.threads[0].id = 1;
  strncpy(res->u.Response.u.ThreadResponse.threads[0].name, "RaviThread",
          sizeof res->u.Response.u.ThreadResponse.threads[0].name);
  vscode_send(res, out, my_logger);
}

/* FIXME handle Big Endian */
typedef union {
  double d;
  intptr_t i;
  struct {
    /* Following is Little endian */
    unsigned long long mant : 52;
    unsigned int expo : 11;
    unsigned int sign : 1;
  };
} doublebits;

/* Takes an intptr_t value and makes sure that it will
 * fit into the mantisaa (52 bits) of a double.
 * This is so that we can assign and send this value as a JSON
 * number which I understand is represented as a double.
 * Any excess bits will be discarded but from what I can see on
 * a 64-bit intel chip the pointer values are anyway less than
 * 52 bits in size.
 */
static intptr_t ensure_value_fits_in_mantissa(intptr_t sourceReference) {
  /* FIXME - big endian support */
  int64_t orig = sourceReference;
  doublebits bits;
  bits.mant = sourceReference;
  bits.expo = 0;            /* cleanup */
  bits.sign = 0;            /* cleanup */
  sourceReference = bits.i; /* extract mantissa */
  fprintf(my_logger, "Source Reference WAS %lld NOW %lld\n", (long long)orig,
          (long long)sourceReference);
  return sourceReference;
}

/*
* Handle StackTraceRequest
* FIXME - does not handle the paging concept
* truncates the stack trace to MAX_STACK_FRAMES
*/
static void handle_stack_trace_request(ProtocolMessage *req,
                                       ProtocolMessage *res, lua_State *L,
                                       FILE *out) {
  lua_Debug entry;
  int depth = 0; /* TODO we ignore startFrame in the request */
  vscode_make_success_response(req, res, VSCODE_STACK_TRACE_RESPONSE);
  sourceOnStackCount =
      0; /* for tracking functions that have a dynamic source */
  while (lua_getstack(L, depth, &entry) &&
         depth < req->u.Request.u.StackTraceRequest.levels &&
         depth < MAX_STACK_FRAMES) {
    int status = lua_getinfo(L, "Sln", &entry);
    assert(status);
    const char *src = entry.source;
    if (*src == '@') {
      /* Source is a file */
      src++;
      /* If the source does not include a path then
         we prefix the source with the path to the
         working directrory so that it can be reliably
         found, else we assume that either the path to the
         source is absolue or it is relative to the working
         directory */
      const char *last_path_delim = strrchr(src, '/');
      char path[1024];
      char name[256];
      if (last_path_delim) {
        /* source includes a path */
        vscode_string_copy(name, last_path_delim + 1, sizeof name);
        vscode_string_copy(path, src, sizeof path);
      }
      else {
        /* prepend the working directory to the name */
        vscode_string_copy(name, src, sizeof name);
        if (workingdir[0]) {
          size_t n = strlen(workingdir);
          if (workingdir[n - 1] == '/')
            snprintf(path, sizeof path, "%s%s", workingdir, src);
          else
            snprintf(path, sizeof path, "%s/%s", workingdir, src);
        }
        else {
          vscode_string_copy(path, src, sizeof path);
        }
      }
      vscode_string_copy(
          res->u.Response.u.StackTraceResponse.stackFrames[depth].source.path,
          path, sizeof res->u.Response.u.StackTraceResponse.stackFrames[depth]
                    .source.path);
      vscode_string_copy(
          res->u.Response.u.StackTraceResponse.stackFrames[depth].source.name,
          name, sizeof res->u.Response.u.StackTraceResponse.stackFrames[depth]
                    .source.name);
    }
    else if (memcmp(src, "=[C]", 4) == 0) {
      /* C Function so source is not available */
      res->u.Response.u.StackTraceResponse.stackFrames[depth]
          .source.sourceReference = -1;
      vscode_string_copy(
          res->u.Response.u.StackTraceResponse.stackFrames[depth].source.name,
          "<C function>",
          sizeof res->u.Response.u.StackTraceResponse.stackFrames[depth]
              .source.name);
    }
    else {
      /* Source is a string - send a reference to the stack frame */
      /* Currently (as ov VSCode 1.1 the sourceReference must be unique within
       * a debug session. A cheap way of making this unique is to use the
       * pointer to the source itself. However as we are passing this to
       * a JavaScript number (double) - we have to be careful how we encode
       * the reference. Following ensures that we encode the pointer value in
       * the mantissa bits.
       */
      int64_t sourceReference = (intptr_t)src;
      sourceReference = ensure_value_fits_in_mantissa(sourceReference);
      /* Record the depth -> source mapping so that we can later on
         determine the correct depth from the source reference */
      /* following not range checked as already checked */
      sourceOnStack[sourceOnStackCount].depth = depth;
      sourceOnStack[sourceOnStackCount++].sourceReference = sourceReference;
      res->u.Response.u.StackTraceResponse.stackFrames[depth]
          .source.sourceReference = sourceReference;
      /* name must be uniquely associated with the source else
       * VSCode gets confused (see
       * https://github.com/Microsoft/vscode/issues/6360) */
      snprintf(
          res->u.Response.u.StackTraceResponse.stackFrames[depth].source.name,
          sizeof res->u.Response.u.StackTraceResponse.stackFrames[depth]
              .source.name,
          "<dynamic function %p>", src);
    }
    res->u.Response.u.StackTraceResponse.stackFrames[depth].id = depth;
    res->u.Response.u.StackTraceResponse.stackFrames[depth].line =
        entry.currentline;
    const char *funcname = entry.name ? entry.name : "?";
    vscode_string_copy(
        res->u.Response.u.StackTraceResponse.stackFrames[depth].name, funcname,
        sizeof res->u.Response.u.StackTraceResponse.stackFrames[depth].name);
    depth++;
  }
  res->u.Response.u.StackTraceResponse.totalFrames = depth;
  vscode_send(res, out, my_logger);
}

/*
 * Handle the request to provide the source given a source reference
 * Previously in the stack request response we have encoded a mapping between
 * each source reference to the stack depth so we can now lookup in the
 * mapping and obtain the appropriate source from the Lua VM
 */
static void handle_source_request(ProtocolMessage *req, ProtocolMessage *res,
                                  lua_State *L, FILE *out) {
  lua_Debug entry;
  int64_t sourceReference = req->u.Request.u.SourceRequest.sourceReference;
  int depth = -1;
  /* search for the source reference */
  for (int i = 0; i < sourceOnStackCount; i++) {
    if (sourceOnStack[i].sourceReference == sourceReference) {
      depth = sourceOnStack[i].depth;
      break;
    }
  }
  fprintf(my_logger,
          "SEARCHED FOR sourceReference=%lld, found at stack depth = %d\n",
          (long long)sourceReference, depth);
  vscode_make_success_response(req, res, VSCODE_SOURCE_RESPONSE);
  if (lua_getstack(L, depth, &entry)) {
    int status = lua_getinfo(L, "Sln", &entry);
    if (status) {
      const char *src = entry.source;
      if (*src != '@' && *src != '=') {
        /* Source is a string */
        vscode_string_copy(res->u.Response.u.SourceResponse.content, src,
                         sizeof res->u.Response.u.SourceResponse.content);
      }
      else
        goto l_nosource;
    }
    else
      goto l_nosource;
  }
  else {
  l_nosource:
    vscode_string_copy(res->u.Response.u.SourceResponse.content,
                     "Source not available",
                     sizeof res->u.Response.u.SourceResponse.content);
  }
  vscode_send(res, out, my_logger);
}

/*
* Handle SetBreakpointsRequest
* A list of all breakpoints is maintained globally - this
* is simply a linear array of breakpoints. We search and overwrite
* all breakpoints associated with the given source.
* Note that breakpoints against dynamic source is not handled
*/
static void handle_set_breakpoints_request(ProtocolMessage *req,
                                           ProtocolMessage *res, FILE *out,
                                           FILE *my_logger) {
  vscode_make_success_response(req, res, VSCODE_SET_BREAKPOINTS_RESPONSE);
  int j = 0, k = 0; /* j tracks the position in our global breakpoints,
                    k tracks the breakpoint position in the response,
                    i tracks the breakpoint position in the request */
  for (int i = 0; i < MAX_BREAKPOINTS; i++) {
    /* Make sure that the breakpoint is in a source file - disallow breakpoints
     * in dynamic code */
    if (req->u.Request.u.SetBreakpointsRequest.breakpoints[i].line > 0 &&
        req->u.Request.u.SetBreakpointsRequest.source.sourceReference == 0) {
      while (j < MAX_TOTAL_BREAKPOINTS) {
        int y = j++;
        /* as we overwrite any previously set breakpoints we do not need
           to match the line number */
        if (breakpoints[y].source.path[0] == 0 ||
            strcmp(breakpoints[y].source.path,
                   req->u.Request.u.SetBreakpointsRequest.source.path) == 0) {
          vscode_string_copy(breakpoints[y].source.path,
                           req->u.Request.u.SetBreakpointsRequest.source.path,
                           sizeof breakpoints[0].source.path);
          breakpoints[y].line =
              req->u.Request.u.SetBreakpointsRequest.breakpoints[i].line;
          fprintf(my_logger, "Saving breakpoint j=%d, k=%d, i=%d\n", y, k, i);
          if (k < MAX_BREAKPOINTS) {
            res->u.Response.u.SetBreakpointsResponse.breakpoints[k].line =
                req->u.Request.u.SetBreakpointsRequest.breakpoints[i].line;
            res->u.Response.u.SetBreakpointsResponse.breakpoints[k].verified =
                false;
            vscode_string_copy(
                res->u.Response.u.SetBreakpointsResponse.breakpoints[k]
                    .source.path,
                breakpoints[y].source.path,
                sizeof res->u.Response.u.SetBreakpointsResponse.breakpoints[k]
                    .source.path);
            k++;
          }
          break;
        }
      }
      if (j == MAX_TOTAL_BREAKPOINTS || k == MAX_BREAKPOINTS) break;
    }
  }
  /* Clear any other breakpoints within the same source file */
  for (; j < MAX_TOTAL_BREAKPOINTS; j++) {
    if (strcmp(breakpoints[j].source.path,
               req->u.Request.u.SetBreakpointsRequest.source.path) == 0) {
      breakpoints[j].source.path[0] = 0;
      breakpoints[j].line = 0;
    }
  }
  vscode_send(res, out, my_logger);
}

/*
* Handle ScopesRequest
* Since VSCode requires us to send a numeric references we need
* to encode the stack depth and the scope within a numeric value.
* We do this by packing the values into an int32_t value.
*/
static void handle_scopes_request(ProtocolMessage *req, ProtocolMessage *res,
                                  lua_State *L, FILE *out) {
  lua_Debug entry;
  int depth = 0;
  vscode_make_success_response(req, res, VSCODE_SCOPES_RESPONSE);
  depth = req->u.Request.u.ScopesRequest.frameId;
  if (lua_getstack(L, depth, &entry)) {
    int status = lua_getinfo(L, "u", &entry);
    assert(status);
    int i = 0;
    vscode_string_copy(res->u.Response.u.ScopesResponse.scopes[i].name, "Locals",
                     sizeof res->u.Response.u.ScopesResponse.scopes[0].name);
    res->u.Response.u.ScopesResponse.scopes[i].variablesReference =
        MAKENUM(VAR_TYPE_LOCALS, depth, 0, 0, 0);
    res->u.Response.u.ScopesResponse.scopes[i++].expensive = 0;
    if (entry.isvararg) {
      vscode_string_copy(res->u.Response.u.ScopesResponse.scopes[i].name,
                       "Var Args",
                       sizeof res->u.Response.u.ScopesResponse.scopes[0].name);
      res->u.Response.u.ScopesResponse.scopes[i].variablesReference =
          MAKENUM(VAR_TYPE_LOCALS, depth, 0, 0, 1);
      res->u.Response.u.ScopesResponse.scopes[i++].expensive = 0;
    }
    vscode_string_copy(res->u.Response.u.ScopesResponse.scopes[i].name, "Globals",
                     sizeof res->u.Response.u.ScopesResponse.scopes[0].name);
    res->u.Response.u.ScopesResponse.scopes[i].variablesReference =
        MAKENUM(VAR_TYPE_GLOBALS, depth, 0, 0, 0);
    res->u.Response.u.ScopesResponse.scopes[i].expensive = 1;
  }
  else {
    vscode_make_error_response(req, res, VSCODE_SCOPES_RESPONSE,
                               "Error retrieving stack frame");
  }
  vscode_send(res, out, my_logger);
}

// Count number of entries in a Lua table
// The standard luaL_len() only works for sequences
static int count_table_entries(lua_State *L, int stack_index) {
  int count = 0;
  int oldt = lua_gettop(L);
  // Push another reference to the table on top of the stack (so we know
  // where it is, and this function can work for negative, positive and
  // pseudo indices
  lua_pushvalue(L, stack_index);
  // stack now contains: -1 => table
  lua_pushnil(L);  // push first key
  // stack now contains: -1 => nil; -2 => table
  while (lua_next(L, -2)) {
    // stack now contains: -1 => value; -2 => key; -3 => table
    count++;
    lua_pop(L, 1);  // pop value, but keep key
  }
  // pop the table
  lua_pop(L, 1);
  assert(lua_gettop(L) == oldt);
  return count;
}

// Get information regarding a Lua table
static void get_table_info(lua_State *L, int stack_idx, char *buf, size_t len) {
  int num = count_table_entries(L, stack_idx);
  const void *ptr = lua_topointer(L, stack_idx);
  snprintf(buf, len, "%p (%d items)", ptr, num);
}

// Get information regarding a Lua userdata value
static void get_userdata(lua_State *L, int stack_idx, char *buf, size_t len) {
  void *udata = lua_touserdata(L, stack_idx);
  snprintf(buf, len, "%p", udata);
}

// Get information regarding a Lua value
static int get_value(lua_State *L, int stack_idx, char *buf, size_t len) {
  int rc = 0;
  int l_type = lua_type(L, stack_idx);
  *buf = 0;
  switch (l_type) {
    case LUA_TNIL: {
      snprintf(buf, len, "nil");
      break;
    }
    case LUA_TBOOLEAN: {
      snprintf(buf, len, "%s",
               (lua_toboolean(L, stack_idx) != 0) ? "true" : "false");
      break;
    }
    case LUA_TLIGHTUSERDATA: {
      get_userdata(L, stack_idx, buf, len);
      break;
    }
    case LUA_TNUMBER: {
      double num = lua_tonumber(L, stack_idx);
      if ((long)num == num)
        snprintf(buf, len, "%ld (0x%lx)", (long)num, (unsigned long)num);
      else
        snprintf(buf, len, "%g", num);
      break;
    }
    case LUA_TSTRING: {
      char tbuf[1024];
      // We assume here that the value at stack index is safe to convert 
      // to a string
      snprintf(tbuf, sizeof tbuf, "%s", lua_tostring(L, stack_idx));
      vscode_json_stringify(tbuf, buf, len);
      break;
    }
    case LUA_TTABLE: {
      get_table_info(L, stack_idx, buf, len);
      rc = 1;
      break;
    }
    case LUA_TFUNCTION: {
      snprintf(buf, len, "%s", ravi_typename(L, stack_idx));
      break;
    }
    case LUA_TUSERDATA: {
      get_userdata(L, stack_idx, buf, len);
      break;
    }
    case LUA_TTHREAD: {
      snprintf(buf, len, "Thread %p", lua_topointer(L, stack_idx));
      break;
    }
  }
  return rc;
}

/*
 * Get a table's values into the response
 */
static void get_table_values(ProtocolMessage *res, lua_State *L, int stack_idx,
                             int varType, int depth, int var) {
  // Push another reference to the table on top of the stack (so we know
  // where it is, and this function can work for negative, positive and
  // pseudo indices  
  lua_pushvalue(L, stack_idx);
  // stack now contains: -1 => table
  lua_pushnil(L);  // push first key
  // stack now contains: -1 => nil (key); -2 => table
  int v = 0;
  while (lua_next(L, -2) && v < MAX_VARIABLES) {
    // stack now contains: -1 => value; -2 => key; -3 => table
    if (v+1 == MAX_VARIABLES) {
      vscode_string_copy(
          res->u.Response.u.VariablesResponse.variables[v].name, "...",
          sizeof res->u.Response.u.VariablesResponse.variables[0].name);
      vscode_string_copy(
          res->u.Response.u.VariablesResponse.variables[v].value, "truncated",
          sizeof res->u.Response.u.VariablesResponse.variables[0].value);
      lua_pop(L, 1); /* pop value */
    }
    else {
      // stack now contains: -1 => value; -2 => key; -3 => table
      // copy the key so that lua_tostring does not modify the
      // original
      lua_pushvalue(L, -2);
      // stack now contains: -1 => key; -2 => value; -3 => key
      get_value(L, -1, res->u.Response.u.VariablesResponse.variables[v].name,
                sizeof res->u.Response.u.VariablesResponse.variables[0].name);
      int is_table = get_value(
          L, -2, res->u.Response.u.VariablesResponse.variables[v].value,
          sizeof res->u.Response.u.VariablesResponse.variables[0].value);
      /*
      * Right now we do not support further drill down
      */
      res->u.Response.u.VariablesResponse.variables[v].variablesReference =
          (is_table && var == 0) ? MAKENUM(varType, depth, v, 0, 0) : 0;
      // pop value + copy of key, leaving original key
      lua_pop(L, 2);
    }
    v++;
  }
  lua_pop(L, 1); /* pop the table */
}
/*
 * The VSCode front-end sends a variables request when it wants to
 * display variables. Unfortunately a limitation is that only a numberic
 * reference field is available named 'variableReference' to identify the
 * variable.
 * We need to know various bits about the variable - such as its stack frame
 * location, the variable's location. Therefore we have to encode various pieces
 * of information in the numeric value.
 */
static void handle_variables_request(ProtocolMessage *req, ProtocolMessage *res,
                                     lua_State *L, FILE *out) {
  lua_Debug entry;
  vscode_make_success_response(req, res, VSCODE_VARIABLES_RESPONSE);
  int varRef = req->u.Request.u.VariablesRequest.variablesReference;
  /*
   * The variable reference is encoded such that it contains:
   * type (2 bits) - the scope type
   * depth (6 bits) - the stack frame
   * var (8 bits) - the index of the variable as provided to lua_getlocal()
   *              - These are negative for varargs values
   * unused (8 bits)
   * isvararg (8 bits) - this is a flag to indicate request for varargs
   */
  int type = EXTRACT_T(varRef);
  int depth = EXTRACT_A(varRef);
  int var = (char)(EXTRACT_B(varRef)); /* the cast is to get the signed value */
  int isvararg = EXTRACT_D(varRef);
  fprintf(my_logger, "Var Request --> %d isvararg=%d\n", type, isvararg);
  if (lua_getstack(L, depth, &entry)) {
    if (type == VAR_TYPE_GLOBALS) {
      if (var == 0) {
        lua_pushglobaltable(L);
        int stack_idx = lua_gettop(L);
        get_table_values(res, L, stack_idx, VAR_TYPE_GLOBALS, depth, 0);
        lua_pop(L, 1);
      }
      else {
        /* TODO */
      }
    }
    else if (type == VAR_TYPE_LOCALS) {  // locals
      if (var == 0) {
        /*
         * A top level request - i.e. from the scope
         */
        for (int n = 1, v = 0; v < MAX_VARIABLES; n++) {
          const char *name = lua_getlocal(L, &entry, isvararg ? -n : n);
          if (name) {
            /* Temporary variables have names that start with (*temporary).
             * Skip such variables
             */
            if (*name != '(' || strcmp(name, "(*vararg)") == 0) {
              if (*name == '(') {
                char temp[80];
                snprintf(temp, sizeof temp, "vararg[%d]", n);
                vscode_string_copy(
                    res->u.Response.u.VariablesResponse.variables[v].name, temp,
                    sizeof res->u.Response.u.VariablesResponse.variables[0]
                        .name);
              }
              else {
                vscode_string_copy(
                    res->u.Response.u.VariablesResponse.variables[v].name, name,
                    sizeof res->u.Response.u.VariablesResponse.variables[0]
                        .name);
              }
              int is_table = get_value(
                  L, lua_gettop(L),
                  res->u.Response.u.VariablesResponse.variables[v].value,
                  sizeof res->u.Response.u.VariablesResponse.variables[0]
                      .value);
              /* If the variable is a table then we pass pack a reference
                 that is used by the front end to drill down */
              res->u.Response.u.VariablesResponse.variables[v]
                  .variablesReference =
                  is_table ? MAKENUM(type, depth, isvararg ? ((char)-n) : n, 0,
                                     isvararg)
                           : 0; /* cast is to ensure that we
                                   save a signed char value */
              v++;
            }
            lua_pop(L, 1); /* pop the value */
          }
          else {
            break;
          }
        }
      }
      else {
        /*
         * We support one level of drill down
         * FIXME - the number of items is limited to MAX_VARIABLES
         * but user is not shown any warning if values are truncated
         */
        fprintf(my_logger,
                "\n--> Request to extract local variable %d of type %d at "
                "depth %d\n",
                var, type, depth);
        const char *name = lua_getlocal(L, &entry, var);
        if (name) {
          int stack_idx = lua_gettop(L);
          int l_type = lua_type(L, stack_idx);
          if (l_type == LUA_TTABLE) {
            get_table_values(res, L, stack_idx, VAR_TYPE_LOCALS, depth, var);
          }
          lua_pop(L, 1);
        }
      }
    }
  }
  else {
    vscode_make_error_response(req, res, VSCODE_VARIABLES_RESPONSE,
                               "Error retrieving variables");
  }
  vscode_send(res, out, my_logger);
}

/* Sets values in the Lua global package table such as 'path' and 'cpath' */
static void set_package_var(lua_State *L, const char *key, const char *value) {
  lua_getglobal(L, "package");
  lua_pushstring(L, value);
  lua_setfield(L, -2, key);
  lua_pop(L, 1);
}

/**
 * The VSCode front-end sends a Launch request when the user
 * starts a debug session. This is where the actual Lua code
 * execution begins
 */
static void handle_launch_request(ProtocolMessage *req, ProtocolMessage *res,
                                  lua_State *L, FILE *out) {
  if (debugger_state != DEBUGGER_INITIALIZED) {
    vscode_send_error_response(req, res, VSCODE_LAUNCH_RESPONSE,
                               "not initialized or unexpected state", out,
                               my_logger);
    return;
  }
  /* We allow the user to set LUA_PATH and LUA_CPATH */
  if (req->u.Request.u.LaunchRequest.lua_path[0]) {
    set_package_var(L, "path", req->u.Request.u.LaunchRequest.lua_path);
  }
  if (req->u.Request.u.LaunchRequest.lua_cpath[0]) {
    set_package_var(L, "cpath", req->u.Request.u.LaunchRequest.lua_cpath);
  }
  /* We allow the user to change current directory */
  if (req->u.Request.u.LaunchRequest.cwd[0]) {
    if (chdir(req->u.Request.u.LaunchRequest.cwd) != 0) {
      char temp[1024];
      snprintf(temp, sizeof temp, "Unable to change directory to %s\n",
               req->u.Request.u.LaunchRequest.cwd);
      vscode_send_output_event(res, "console", temp, out, my_logger);
      vscode_send_error_response(req, res, VSCODE_LAUNCH_RESPONSE,
                                 "Launch failed", out, my_logger);
      return;
    }
    else {
      /* Make a note of the working directory so that we can work out the
        path name of any source files */
      vscode_string_copy(workingdir, req->u.Request.u.LaunchRequest.cwd,
                       sizeof workingdir);
    }
  }
  const char *progname = req->u.Request.u.LaunchRequest.program;
  fprintf(my_logger, "\n--> Launching '%s'\n", progname);
  int status = luaL_loadfile(L, progname);
  if (status != LUA_OK) {
    char temp[1024];
    snprintf(temp, sizeof temp, "Failed to launch %s due to error: %s\n",
             progname, lua_tostring(L, -1));
    vscode_send_output_event(res, "console", temp, out, my_logger);
    vscode_send_error_response(req, res, VSCODE_LAUNCH_RESPONSE,
                               "Launch failed", out, my_logger);
    lua_pop(L, 1);
    return;
  }
  else {
    /* Lua program successfully compiled. Although we have not yet
      launched, we tell VSCode that we have at this stage */
    vscode_send_success_response(req, res, VSCODE_LAUNCH_RESPONSE, out,
                                 my_logger);
  }
  if (req->u.Request.u.LaunchRequest.stopOnEntry)
    debugger_state = DEBUGGER_PROGRAM_STEPPING;
  else
    debugger_state = DEBUGGER_PROGRAM_RUNNING;
  /* Start the Lua code! */
  /* From here on the debugger will get control inside the debugger() function
     below which is setup as a Lua hook whenever Lua steps across a new line of
     code. When control gets back here it means the program finished executing */
  if (lua_pcall(L, 0, 0, 0)) {
    char temp[1024];
    snprintf(temp, sizeof temp, "Program terminated with error: %s\n",
             lua_tostring(L, -1));
    vscode_send_output_event(res, "console", temp, out, my_logger);
    lua_pop(L, 1);
  }
  vscode_send_terminated_event(res, out, my_logger);
  debugger_state = DEBUGGER_PROGRAM_TERMINATED;
}

/**
 * Called via Lua Hook or from main()
 * If called from main then debugger_state == DEBUGGER_BIRTH and ar == NULL
 */
static void debugger(lua_State *L, lua_Debug *ar, FILE *in, FILE *out) {
  if (debugger_state == DEBUGGER_PROGRAM_TERMINATED) { return; }
  
  /* If the program is running or stepping over/out 
     then check if we hit a breakpoint. */
  if (debugger_state == DEBUGGER_PROGRAM_RUNNING ||
      (debugger_state == DEBUGGER_PROGRAM_STEPPING &&
       (stepping_mode == DEBUGGER_STEPPING_OUT ||
        stepping_mode == DEBUGGER_STEPPING_OVER))) {
    int initialized = 0;
    for (int j = 0; j < MAX_BREAKPOINTS; j++) {
      /* fast check - are we on a breakpoint line number */
      if (!breakpoints[j].source.path[0] ||
          ar->currentline != breakpoints[j].line)
        continue;
      /* potential match of breakpoint line - we need to check the source name */
      if (!initialized) initialized = lua_getinfo(L, "S", ar);
      if (!initialized) break;
      if (ar->source[0] == '@') {
        /* Only support breakpoints on source files */
        if (strcmp(breakpoints[j].source.path, ar->source + 1) == 0) {
          /* hit breakpoint */
          debugger_state = DEBUGGER_PROGRAM_STEPPING;
          stepping_mode = DEBUGGER_STEPPING_IN;
          stepping_stacklevel = -1;
          stepping_lua_State = NULL;
          break;
        }
      }
    }
    if (debugger_state == DEBUGGER_PROGRAM_RUNNING) return;
  }
  if (debugger_state == DEBUGGER_PROGRAM_STEPPING) {
    /* First check stepping over/out conditions and continue
       execution if not satisfied */
    if (stepping_mode == DEBUGGER_STEPPING_OVER && L == stepping_lua_State &&
        ar->stacklevel > stepping_stacklevel)
      return; /* we are deeper into the stack, so continue execution */
    else if (stepping_mode == DEBUGGER_STEPPING_OUT &&
             L == stepping_lua_State && ar->stacklevel >= stepping_stacklevel)
      return; /* we are still in current function or deeper so continue execution */
    /* OK so we are going to be stopping */
    /* running within Lua at line change */
    if (!thread_event_sent) {
      /* thread started - only sent once in the debug session */
      thread_event_sent = 1;
      vscode_send_thread_event(&res, true, out, my_logger);
      /* Inform VSCode we have stopped */
      vscode_send_stopped_event(&res, "entry", out, my_logger);
    }
    else {
      /* Inform VSCode we have stopped */
      vscode_send_stopped_event(&res, "step", out, my_logger);
    }
    debugger_state = DEBUGGER_PROGRAM_STOPPED;
  }
  /* Reset stepping mode */
  stepping_mode = DEBUGGER_STEPPING_IN;
  stepping_stacklevel = -1;

  /* Wait for debugger command */
  bool get_command = true;
  int command = VSCODE_UNKNOWN_REQUEST;
  while (get_command &&
         (command = vscode_get_request(in, &readbuf, &req, my_logger)) != VSCODE_EOF) {
    switch (command) {
      case VSCODE_INITIALIZE_REQUEST: {
        handle_initialize_request(&req, &res, out);
        break;
      }
      case VSCODE_LAUNCH_REQUEST: {
        handle_launch_request(&req, &res, L, out);
        break;
      }
      case VSCODE_STACK_TRACE_REQUEST: {
        handle_stack_trace_request(&req, &res, L, out);
        break;
      }
      case VSCODE_SCOPES_REQUEST: {
        handle_scopes_request(&req, &res, L, out);
        break;
      }
      case VSCODE_VARIABLES_REQUEST: {
        handle_variables_request(&req, &res, L, out);
        break;
      }
      case VSCODE_SOURCE_REQUEST: {
        handle_source_request(&req, &res, L, out);
        break;
      }
      case VSCODE_DISCONNECT_REQUEST: {
        vscode_send_terminated_event(&res, out, my_logger);
        debugger_state = DEBUGGER_PROGRAM_TERMINATED;
        vscode_send_success_response(&req, &res, VSCODE_DISCONNECT_RESPONSE,
                                     out, my_logger);
        exit(0);
      }
      case VSCODE_SET_EXCEPTION_BREAKPOINTS_REQUEST: {
        vscode_send_success_response(&req, &res,
                                     VSCODE_SET_EXCEPTION_BREAKPOINTS_RESPONSE,
                                     out, my_logger);
        break;
      }
      case VSCODE_SET_BREAKPOINTS_REQUEST: {
        handle_set_breakpoints_request(&req, &res, out, my_logger);
        break;
      }
      case VSCODE_CONFIGURATION_DONE_REQUEST: {
        vscode_send_success_response(
            &req, &res, VSCODE_CONFIGURATION_DONE_RESPONSE, out, my_logger);
        break;
      }
      case VSCODE_THREAD_REQUEST: {
        handle_thread_request(&req, &res, out);
        break;
      }
      case VSCODE_STEPIN_REQUEST: {
        vscode_send_success_response(&req, &res, VSCODE_STEPIN_RESPONSE, out,
                                     my_logger);
        debugger_state = DEBUGGER_PROGRAM_STEPPING;
        get_command = false;
        break;
      }
      case VSCODE_STEPOUT_REQUEST: {
        vscode_send_success_response(&req, &res, VSCODE_STEPOUT_RESPONSE, out,
                                     my_logger);
        debugger_state = DEBUGGER_PROGRAM_STEPPING;
        stepping_stacklevel = ar->stacklevel;
        stepping_mode = DEBUGGER_STEPPING_OUT;
        stepping_lua_State = L;
        get_command = false;
        break;
      }
      case VSCODE_NEXT_REQUEST: {
        /* Step Over */
        vscode_send_success_response(&req, &res, VSCODE_NEXT_RESPONSE, out,
                                     my_logger);
        debugger_state = DEBUGGER_PROGRAM_STEPPING;
        stepping_stacklevel = ar->stacklevel;
        stepping_mode = DEBUGGER_STEPPING_OVER;
        stepping_lua_State = L;
        get_command = false;
        break;
      }
      case VSCODE_CONTINUE_REQUEST: {
        debugger_state = DEBUGGER_PROGRAM_RUNNING;
        vscode_send_success_response(&req, &res, VSCODE_CONTINUE_RESPONSE, out,
                                     my_logger);
        get_command = false;
        break;
      }
      default: {
        char msg[100];
        snprintf(msg, sizeof msg, "%s not yet implemented",
                 req.u.Request.command);
        fprintf(my_logger, "%s\n", msg);
        vscode_send_error_response(&req, &res, command, msg, out, my_logger);
        break;
      }
    }
  }
}

/*
* Lua Hook used by the debugger
* Setup to intercept at every line change
*/
void ravi_debughook(lua_State *L, lua_Debug *ar) {
  int event = ar->event;
  if (event == LUA_HOOKLINE) { debugger(L, ar, stdin, stdout); }
}

void ravi_debug_writestring(const char *s, size_t l) {
  char temp[256];
  if (l >= sizeof temp) l = sizeof temp - 1;
  vscode_string_copy(temp, s, l + 1);
  vscode_send_output_event(&output_response, "stdout", temp, stdout, my_logger);
}

void ravi_debug_writeline(void) {
  vscode_send_output_event(&output_response, "stdout", "\n", stdout, my_logger);
}

void ravi_debug_writestringerror(const char *fmt, const char *p) {
  char temp[256];
  snprintf(temp, sizeof temp, fmt, p);
  vscode_send_output_event(&output_response, "stderr", temp, stdout, my_logger);
}

/*
** Create the 'arg' table, which stores all arguments from the
** command line ('argv'). It should be aligned so that, at index 0,
** it has 'argv[script]', which is the script name. The arguments
** to the script (everything after 'script') go to positive indices;
** other arguments (before the script name) go to negative indices.
** If there is no script name, assume interpreter's name as base.
*/
static void createargtable(lua_State *L, char **argv, int argc, int script) {
  int i, narg;
  if (script == argc) script = 0; /* no script name? */
  narg = argc - (script + 1);     /* number of positive indices */
  lua_createtable(L, narg, script + 1);
  for (i = 0; i < argc; i++) {
    lua_pushstring(L, argv[i]);
    lua_rawseti(L, -2, i - script);
  }
  lua_setglobal(L, "arg");
}

static inline bool is_bigendian() {
  static const int i = 1;
  return (*((char *)&i)) == 0;
}

/*
* Entry point for the debugger
* The debugger will use stdin/stdout to interact with VSCode
* The protocol used is described in protocol.h.
*/
int main(int argc, char **argv) {
  if (is_bigendian()) {
    fprintf(stderr, "Big endian architecture not supported\n");
    exit(1);
  }
  membuff_init(&readbuf, 0);
/* For debugging purposes we log the interaction */
#ifdef _WIN32
  my_logger = fopen("/temp/out1.txt", "w");
#else
  my_logger = fopen("/tmp/out1.txt", "w");
#endif
  if (my_logger == NULL) my_logger = stderr;
#ifdef _WIN32
  /* The VSCode debug protocol requires binary IO */
  _setmode(_fileno(stdout), _O_BINARY);
#endif
  /* switch off buffering */
  setbuf(my_logger, NULL);
  setbuf(stdout, NULL);
  lua_State *L = luaL_newstate(); /* create Lua state */
  if (L == NULL) { return EXIT_FAILURE; }
  /* redirect Lua's stdout and stderr */
  ravi_set_writefuncs(L, ravi_debug_writestring, ravi_debug_writeline,
                      ravi_debug_writestringerror);
  luaL_checkversion(L); /* check that interpreter has correct version */
  luaL_openlibs(L);     /* open standard libraries */
  createargtable(L, argv, argc, 0); /* Create a the args global in Lua */
  lua_sethook(L, ravi_debughook, LUA_MASKCALL | LUA_MASKLINE | LUA_MASKRET, 0);
  debugger_state = DEBUGGER_BIRTH;
  ravi_set_debugger_data(
      L, &debugger_state); /* This is useless data right now but it ensures that
                              the hook cannot be removed */
  debugger(L, NULL, stdin, stdout);
  lua_close(L);
  fclose(my_logger);
  membuff_free(&readbuf);
  return 0;
}
