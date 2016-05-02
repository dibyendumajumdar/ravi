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
#include <fcntl.h>
#include <io.h>
#endif

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#include "protocol.h"

enum {
  DEBUGGER_BIRTH = 1,
  DEBUGGER_INITIALIZED = 2,
  DEBUGGER_PROGRAM_LAUNCHED = 3,
  DEBUGGER_PROGRAM_STEPPING = 4,
  DEBUGGER_PROGRAM_RUNNING = 5,
  DEBUGGER_PROGRAM_STOPPED = 6,
  DEBUGGER_PROGRAM_TERMINATED = 7
};

enum {
  VAR_TYPE_LOCALS = 1,
  VAR_TYPE_UPVALUES = 2,
  VAR_TYPE_GLOBALS = 3
};

#define MASK0 0x3F
#define MASK  0xFF
#define MASK1 0x3

#define MAKENUM(t,a,b,c,d) \
(( ((t)&MASK1) | (((a)&MASK0) << 2) ) | \
 ( ((b)&MASK) << 8 ) | \
 ( ((c)&MASK) << 16 ) | \
 ( ((d)&MASK) << 24 ))

#define EXTRACT_T(x) ( (x)&MASK1 )
#define EXTRACT_A(x) ( ((x)>>2) & MASK0 )
#define EXTRACT_B(x) ( ((x)>>8) & MASK )
#define EXTRACT_C(x) ( ((x)>>16) & MASK )
#define EXTRACT_D(x) ( ((x)>>24) & MASK )

/*
 * These statics are temporary - eventually they will be moved to
 * the Lua global state; but right now while things are 
 * evolving this is easier to work with.
 */
static FILE *log = NULL;
static int thread_event_sent = 0;
static int debugger_state = DEBUGGER_BIRTH;
static Breakpoint breakpoints[20];
static ProtocolMessage output_response;

/*
* Generate response to InitializeRequest
* Send InitializedEvent
*/
static void handle_initialize_request(ProtocolMessage *req,
                                      ProtocolMessage *res, FILE *out) {
  if (debugger_state >= DEBUGGER_INITIALIZED) {
    vscode_send_error_response(req, res, VSCODE_INITIALIZE_RESPONSE,
                               "already initialized", out, log);
    return;
  }
  /* Send InitializedEvent */
  vscode_make_initialized_event(res);
  vscode_send(res, out, log);

  /* Send InitializeResponse */
  vscode_make_success_response(req, res, VSCODE_INITIALIZE_RESPONSE);
  res->u.Response.u.InitializeResponse.body.supportsConfigurationDoneRequest =
      1;
  vscode_send(res, out, log);

  /* Send notification */
  vscode_send_output_event(res, "console", "Debugger initialized\n", out, log);
  debugger_state = DEBUGGER_INITIALIZED;
}

/*
* Generate response to ThreadRequest
*/
static void handle_thread_request(ProtocolMessage *req, ProtocolMessage *res,
                                  FILE *out) {
  vscode_make_success_response(req, res, VSCODE_THREAD_RESPONSE);
  res->u.Response.u.ThreadResponse.threads[0].id = 1;
  strncpy(res->u.Response.u.ThreadResponse.threads[0].name, "RaviThread",
          sizeof res->u.Response.u.ThreadResponse.threads[0].name);
  vscode_send(res, out, log);
}

/*
* Handle StackTraceRequest
*/
static void handle_stack_trace_request(ProtocolMessage *req,
                                       ProtocolMessage *res, lua_State *L,
                                       FILE *out) {
  lua_Debug entry;
  int depth = 0;
  vscode_make_success_response(req, res, VSCODE_STACK_TRACE_RESPONSE);
  while (lua_getstack(L, depth, &entry) &&
         depth < req->u.Request.u.StackTraceRequest.levels &&
         depth < MAX_STACK_FRAMES) {
    int status = lua_getinfo(L, "Sln", &entry);
    assert(status);
    const char *src = entry.source;
    if (*src == '@') {
      /* Source is a file */
      src++;
      const char *last_path_delim = strrchr(src, '/');
      char path[1024];
      char name[256];
      if (last_path_delim) { strncpy(name, last_path_delim + 1, sizeof name); }
      else {
        strncpy(name, src, sizeof name);
      }
      strncpy(path, src, sizeof path);
      strncpy(res->u.Response.u.StackTraceResponse.stackFrames[depth].source.path,
        path, sizeof res->u.Response.u.StackTraceResponse.stackFrames[depth]
        .source.path);
      strncpy(res->u.Response.u.StackTraceResponse.stackFrames[depth].source.name,
        name, sizeof res->u.Response.u.StackTraceResponse.stackFrames[depth]
        .source.name);
    }
    else {
      /* Source is a string - send a reference to the stack frame */
      res->u.Response.u.StackTraceResponse.stackFrames[depth].source.sourceReference = depth + 1;
      strncpy(res->u.Response.u.StackTraceResponse.stackFrames[depth].source.name,
        "<dynamic function>", sizeof res->u.Response.u.StackTraceResponse.stackFrames[depth]
        .source.name);
    }
    res->u.Response.u.StackTraceResponse.stackFrames[depth].id = depth;
    res->u.Response.u.StackTraceResponse.stackFrames[depth].line =
        entry.currentline;
    const char *funcname = entry.name ? entry.name : "?";
    strncpy(
        res->u.Response.u.StackTraceResponse.stackFrames[depth].name, funcname,
        sizeof res->u.Response.u.StackTraceResponse.stackFrames[depth].name);
    depth++;
  }
  res->u.Response.u.StackTraceResponse.totalFrames = depth;
  vscode_send(res, out, log);
}

static void handle_source_request(ProtocolMessage *req,
  ProtocolMessage *res, lua_State *L,
  FILE *out) {
  lua_Debug entry;
  int depth = req->u.Request.u.SourceRequest.sourceReference - 1;
  vscode_make_success_response(req, res, VSCODE_SOURCE_RESPONSE);
  if (lua_getstack(L, depth, &entry)) {
    int status = lua_getinfo(L, "Sln", &entry);
    const char *src = entry.source;
    if (*src != '@') {
      /* Source is a string */
      strncpy(res->u.Response.u.SourceResponse.content, src, sizeof res->u.Response.u.SourceResponse.content);
      res->u.Response.u.SourceResponse.content[sizeof res->u.Response.u.SourceResponse.content - 1] = 0; // just in case
    }
    else {
      vscode_make_error_response(req, res, VSCODE_SOURCE_RESPONSE, "Source is in a file");
    }
  }
  else {
    vscode_make_error_response(req, res, VSCODE_SOURCE_RESPONSE, "Source is in a file");
  }
  vscode_send(res, out, log);
}


/*
* Handle StackTraceRequest
*/
static void handle_set_breakpoints_request(ProtocolMessage *req,
  ProtocolMessage *res, FILE *out, FILE *log) {
  vscode_make_success_response(req, res, VSCODE_SET_BREAKPOINTS_RESPONSE);
  int j = 0, k = 0;
  for (int i = 0; i < MAX_BREAKPOINTS; i++) {
    if (req->u.Request.u.SetBreakpointsRequest.breakpoints[i].line > 0) {
      while (j < 20) {
        int y = j++;
        if (breakpoints[y].source.path[0] == 0 || strcmp(breakpoints[y].source.path, req->u.Request.u.SetBreakpointsRequest.source.path) == 0) {
          strncpy(breakpoints[y].source.path, req->u.Request.u.SetBreakpointsRequest.source.path, sizeof breakpoints[0].source.path);
          breakpoints[y].line = req->u.Request.u.SetBreakpointsRequest.breakpoints[y].line;
          fprintf(log, "Saving breakpoint j=%d, k=%d, i=%d\n", y, k, i);
          if (k < MAX_BREAKPOINTS) {
            res->u.Response.u.SetBreakpointsResponse.breakpoints[k].line = req->u.Request.u.SetBreakpointsRequest.breakpoints[i].line;
            res->u.Response.u.SetBreakpointsResponse.breakpoints[k].verified = false;
            strncpy(res->u.Response.u.SetBreakpointsResponse.breakpoints[k].source.path, breakpoints[y].source.path,
              sizeof res->u.Response.u.SetBreakpointsResponse.breakpoints[k].source.path);
            k++;
          }
          break;
        }
      }
      if (j == 20 || k == MAX_BREAKPOINTS) break;
    }
  }
  for (j++; j < 20; j++) {
    if (strcmp(breakpoints[j].source.path, req->u.Request.u.SetBreakpointsRequest.source.path) == 0) {
      breakpoints[j].source.path[0] = 0;
      breakpoints[j].line = 0;
    }
  }
  vscode_send(res, out, log);
}


/*
* Handle ScopeRequest
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
    strncpy(res->u.Response.u.ScopesResponse.scopes[i].name, "Locals",
            sizeof res->u.Response.u.ScopesResponse.scopes[0].name);
    res->u.Response.u.ScopesResponse.scopes[i].variablesReference = MAKENUM(VAR_TYPE_LOCALS, depth, 0, 0, 0);
    res->u.Response.u.ScopesResponse.scopes[i++].expensive = 0;
    if (entry.nups > 0) {
      strncpy(res->u.Response.u.ScopesResponse.scopes[i].name, "Up Values",
              sizeof res->u.Response.u.ScopesResponse.scopes[0].name);
      res->u.Response.u.ScopesResponse.scopes[i].variablesReference = MAKENUM(VAR_TYPE_UPVALUES, depth, 0, 0, 0);
      res->u.Response.u.ScopesResponse.scopes[i++].expensive = 0;
    }
    strncpy(res->u.Response.u.ScopesResponse.scopes[i].name, "Globals",
            sizeof res->u.Response.u.ScopesResponse.scopes[0].name);
    res->u.Response.u.ScopesResponse.scopes[i].variablesReference = MAKENUM(VAR_TYPE_GLOBALS, depth, 0, 0, 0);
    res->u.Response.u.ScopesResponse.scopes[i].expensive = 1;
  }
  else {
    vscode_make_error_response(req, res, VSCODE_SCOPES_RESPONSE,
                               "Error retrieving stack frame");
  }
  vscode_send(res, out, log);
}

static void get_table_info(lua_State *L, int stack_idx, char *buf, size_t len) {
  int num = luaL_len(L, stack_idx);
  const void *ptr = lua_topointer(L, stack_idx);
  snprintf(buf, len, "%p (%d items)", ptr, num);
}

static void get_userdata(lua_State *L, int stack_idx, char *buf, size_t len) {
  void *udata = lua_touserdata(L, stack_idx);
  snprintf(buf, len, "%p", udata);
}

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
      snprintf(tbuf, sizeof tbuf, "%.*s", (int)(sizeof tbuf) - 1, lua_tostring(L, stack_idx));
      vscode_json_stringify(tbuf, buf, len);
      break;
    }
    case LUA_TTABLE: {
      get_table_info(L, stack_idx, buf, len);
      rc = 1;
      break;
    }
    case LUA_TFUNCTION: {
      snprintf(buf, len, "%p", lua_topointer(L, stack_idx));
      break;
    }
    case LUA_TUSERDATA: {
      get_userdata(L, stack_idx, buf, len);
      break;
    }
    case LUA_TTHREAD: {
      snprintf(buf, len, "%p", lua_topointer(L, stack_idx));
      break;
    }
  }

  return rc;
}

/*
* Handle ScopeRequest
*/
static void handle_variables_request(ProtocolMessage *req, ProtocolMessage *res,
                                     lua_State *L, FILE *out) {
  lua_Debug entry;
  vscode_make_success_response(req, res, VSCODE_VARIABLES_RESPONSE);
  int varRef = req->u.Request.u.VariablesRequest.variablesReference;
  /*
   * The variable reference is encoded such that it contains:
   * type - the scope type 
   * depth - the stack frame
   * var - the index of the variable as provided to lua_getlocal()
   */
  int type = EXTRACT_T(varRef);
  int depth = EXTRACT_A(varRef);
  int var = EXTRACT_B(varRef);
  if (lua_getstack(L, depth, &entry)) {
    if (type == VAR_TYPE_LOCALS) { // locals
      if (var == 0) {
        /*
         * A top level request - i.e. from the scope
         */
        for (int n = 1, v = 0; v < MAX_VARIABLES; n++) {
          const char *name = lua_getlocal(L, &entry, n);
          if (name) {
            /* Temporary variables have names that start with (. 
             * Skip such variables
             */
            if (*name != '(') {
              strncpy(
                  res->u.Response.u.VariablesResponse.variables[v].name, name,
                  sizeof res->u.Response.u.VariablesResponse.variables[0].name);
              int is_table = get_value(
                  L, lua_gettop(L),
                  res->u.Response.u.VariablesResponse.variables[v].value,
                  sizeof res->u.Response.u.VariablesResponse.variables[0].value);
              /* If the variable is a table then we pass pack a reference
                 that is used by the front end to drill down */
              res->u.Response.u.VariablesResponse.variables[v].variablesReference = is_table ?
                MAKENUM(type, depth, n, 0, 0) : 0;
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
        fprintf(log, "\n--> Request to extract local variable %d of type %d at depth %d\n", var, type, depth);
        const char *name = lua_getlocal(L, &entry, var);
        if (name) {
          int stack_idx = lua_gettop(L);
          int l_type = lua_type(L, stack_idx);
          if (l_type == LUA_TTABLE) {
            lua_pushnil(L);  // push first key
            int v = 0;
            while (lua_next(L, stack_idx) && v < MAX_VARIABLES) {
              // stack now contains: -1 => value; -2 => key
              // copy the key so that lua_tostring does not modify the original
              lua_pushvalue(L, -2);
              // stack now contains: -1 => key; -2 => value; -3 => key
              get_value(
                        L, -1,
                        res->u.Response.u.VariablesResponse.variables[v].name,
                        sizeof res->u.Response.u.VariablesResponse.variables[0].name);
              get_value(
                        L, -2,
                        res->u.Response.u.VariablesResponse.variables[v].value,
                        sizeof res->u.Response.u.VariablesResponse.variables[0].value);
              /*
               * Right now we do not support further drill down
               */
              res->u.Response.u.VariablesResponse.variables[v].variablesReference = 0;
              v++;
              // pop value + copy of key, leaving original key
              lua_pop(L, 2);
            }
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
  vscode_send(res, out, log);
}

static void handle_launch_request(ProtocolMessage *req, ProtocolMessage *res,
                                  lua_State *L, FILE *out) {
  if (debugger_state != DEBUGGER_INITIALIZED) {
    vscode_send_error_response(req, res, VSCODE_LAUNCH_RESPONSE,
                               "not initialized or unexpected state", out, log);
    return;
  }

  const char *progname = req->u.Request.u.LaunchRequest.program;
  fprintf(log, "\n--> Launching '%s'\n", progname);
  int status = luaL_loadfile(L, progname);
  if (status != LUA_OK) {
    char temp[1024];
    snprintf(temp, sizeof temp, "Failed to launch %s due to error: %s\n",
             progname, lua_tostring(L, -1));
    vscode_send_output_event(res, "console", temp, out, log);
    vscode_send_error_response(req, res, VSCODE_LAUNCH_RESPONSE,
                               "Launch failed", out, log);
    lua_pop(L, 1);
    return;
  }
  else {
    vscode_send_success_response(req, res, VSCODE_LAUNCH_RESPONSE, out, log);
  }
  if (req->u.Request.u.LaunchRequest.stopOnEntry)
    debugger_state = DEBUGGER_PROGRAM_STEPPING;
  else
    debugger_state = DEBUGGER_PROGRAM_RUNNING;
  if (lua_pcall(L, 0, 0, 0)) {
    char temp[1024];
    snprintf(temp, sizeof temp, "Program terminated with error: %s\n",
      lua_tostring(L, -1));
    vscode_send_output_event(res, "console", temp, out, log);
    lua_pop(L, 1);
  }
  vscode_send_terminated_event(res, out, log);
  debugger_state = DEBUGGER_PROGRAM_TERMINATED;
}

/**
 * Called via Lua Hook or via main
 * If called from main then init is true and ar == NULL
 */
static void debugger(lua_State *L, lua_Debug *ar, FILE *in,
                     FILE *out) {
  ProtocolMessage req, res;
  if (debugger_state == DEBUGGER_PROGRAM_TERMINATED) {
    return;
  }
  if (debugger_state == DEBUGGER_PROGRAM_RUNNING) {
    int status = lua_getinfo(L, "S", ar);
    if (status && ar->source[0] == '@') {
      for (int j = 0; j < 20; j++) {
        if (!breakpoints[j].source.path[0])
          break;
        fprintf(log, "Breakpoint[%d] check %s vs ar.source=%s, %d vs ar.line=%d\n", j, breakpoints[j].source.path, ar->source, breakpoints[j].line, ar->currentline);
        if (ar->currentline == breakpoints[j].line && strcmp(breakpoints[j].source.path, ar->source+1) == 0) {
          debugger_state = DEBUGGER_PROGRAM_STEPPING;
          break;
        }
      }
    }
    if (debugger_state == DEBUGGER_PROGRAM_RUNNING)
      return;
  }
  if (debugger_state == DEBUGGER_PROGRAM_STEPPING) {
    /* running within Lua at line change */
    if (!thread_event_sent) {
      /* thread started - only sent once in the debug session */
      thread_event_sent = 1;
      vscode_send_thread_event(&res, true, out, log);
      /* Inform VSCode we have stopped */
      vscode_send_stopped_event(&res, "entry", out, log);
    }
    else {
      /* Inform VSCode we have stopped */
      vscode_send_stopped_event(&res, "step", out, log);
    }
    debugger_state = DEBUGGER_PROGRAM_STOPPED;
  }
  bool get_command = true;
  int command = VSCODE_UNKNOWN_REQUEST;
  while (get_command &&
         (command = vscode_get_request(in, &req, log)) != VSCODE_EOF) {
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
        vscode_send_terminated_event(&res, out, log);
        debugger_state = DEBUGGER_PROGRAM_TERMINATED;
        vscode_send_success_response(&req, &res, VSCODE_DISCONNECT_RESPONSE,
                                     out, log);
        exit(0);
      }
      case VSCODE_SET_EXCEPTION_BREAKPOINTS_REQUEST: {
        vscode_send_success_response(
            &req, &res, VSCODE_SET_EXCEPTION_BREAKPOINTS_RESPONSE, out, log);
        break;
      }
      case VSCODE_SET_BREAKPOINTS_REQUEST: {
        handle_set_breakpoints_request(
          &req, &res, out, log);
        break;
      }
      case VSCODE_CONFIGURATION_DONE_REQUEST: {
        vscode_send_success_response(
            &req, &res, VSCODE_CONFIGURATION_DONE_RESPONSE, out, log);
        break;
      }
      case VSCODE_THREAD_REQUEST: {
        handle_thread_request(&req, &res, out);
        break;
      }
      case VSCODE_STEPIN_REQUEST: {
        vscode_send_success_response(&req, &res, VSCODE_STEPIN_RESPONSE, out,
                                     log);
        debugger_state = DEBUGGER_PROGRAM_STEPPING;
        get_command = false;
        break;
      }
      case VSCODE_STEPOUT_REQUEST: {
        vscode_send_success_response(&req, &res, VSCODE_STEPOUT_RESPONSE, out,
                                     log);
        debugger_state = DEBUGGER_PROGRAM_STEPPING;
        get_command = false;
        break;
      }
      case VSCODE_NEXT_REQUEST: {
        vscode_send_success_response(&req, &res, VSCODE_NEXT_RESPONSE, out,
                                     log);
        debugger_state = DEBUGGER_PROGRAM_STEPPING;
        get_command = false;
        break;
      }
      case VSCODE_CONTINUE_REQUEST: {
        debugger_state = DEBUGGER_PROGRAM_RUNNING;
        vscode_send_success_response(&req, &res, VSCODE_CONTINUE_RESPONSE, out,
                                     log);
        get_command = false;
        break;
      }
      default: {
        char msg[100];
        snprintf(msg, sizeof msg, "%s not yet implemented",
                 req.u.Request.command);
        fprintf(log, "%s\n", msg);
        vscode_send_error_response(&req, &res, command, msg, out, log);
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

/* TODO following should probably not live here*/
void ravi_debug_writestring(const char *s, size_t l) {
  char temp[256];
  if (l >= sizeof temp)
    l = sizeof temp-1;
  strncpy(temp, s, l+1);
  temp[l] = 0;
  vscode_send_output_event(&output_response, "stdout", temp, stdout, log);
}

void ravi_debug_writeline(void) {
  vscode_send_output_event(&output_response, "stdout", "\n", stdout, log);
}

void ravi_debug_writestringerror(const char *fmt, const char *p) {
  char temp[256];
  snprintf(temp, sizeof temp, fmt, p);
  temp[sizeof temp - 1] = 0;
  vscode_send_output_event(&output_response, "stderr", temp, stdout, log);
}

/*
* Entry point for the debugger
* The debugger will use stdin/stdout to interact with VSCode
* The protocol used is described in protocol.h.
*/
int main(void) {
#ifdef _WIN32
  log = fopen("/temp/out1.txt", "w");
#else
  log = fopen("/tmp/out1.txt", "w");
#endif
  if (log == NULL)
    log = stderr;
#ifdef _WIN32
  /* The VSCode debug protocol requires binary IO */
  _setmode(_fileno(stdout), _O_BINARY);
#endif
  /* switch off buffering */
  setbuf(log, NULL);
  setbuf(stdout, NULL);
  lua_State *L = luaL_newstate(); /* create Lua state */
  if (L == NULL) { return EXIT_FAILURE; }
  ravi_set_writefuncs(L, ravi_debug_writestring, ravi_debug_writeline, ravi_debug_writestringerror);
  luaL_checkversion(L); /* check that interpreter has correct version */
  /* TODO need to redirect the stdin/stdout used by Lua */
  luaL_openlibs(L); /* open standard libraries */
  lua_sethook(L, ravi_debughook, LUA_MASKCALL | LUA_MASKLINE | LUA_MASKRET, 0);
  debugger_state = DEBUGGER_BIRTH;
  debugger(L, NULL, stdin, stdout);
  lua_close(L);
  fclose(log);
  return 0;
}
