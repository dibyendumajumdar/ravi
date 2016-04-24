#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#include "protocol.h"

static FILE *log = NULL;

/*
* Send VSCode a StoppedEvent
* The event indicates that the execution of the debuggee has stopped due to some condition.
* This can be caused by a break point previously set, a stepping action has completed, by executing a debugger statement etc.
*/
static void send_stopped_event(ProtocolMessage *res, const char *msg,
  FILE *out) {
  char buf[1024];
  vscode_make_stopped_event(res, msg);
  vscode_serialize_event(buf, sizeof buf, res);
  fprintf(log, "%s\n", buf);
  fprintf(out, buf);
}

/*
* Send VSCode a ThreadEvent
*/
static void send_thread_event(ProtocolMessage *res, bool started,
  FILE *out) {
  char buf[1024];
  vscode_make_thread_event(res, started);
  vscode_serialize_event(buf, sizeof buf, res);
  fprintf(log, "%s\n", buf);
  fprintf(out, buf);
}

static void send_output_event(ProtocolMessage *res, const char *msg,
                              FILE *out) {
  char buf[1024];
  vscode_make_output_event(res, msg);
  vscode_serialize_event(buf, sizeof buf, res);
  fprintf(log, "%s\n", buf);
  fprintf(out, buf);
}

static void send_error_response(ProtocolMessage *req, ProtocolMessage *res,
                                int responseType, const char *msg, FILE *out) {
  char buf[1024];
  vscode_make_error_response(req, res, responseType, msg);
  vscode_serialize_response(buf, sizeof buf, res);
  fprintf(log, "%s\n", buf);
  fprintf(out, buf);
}

static void send_success_response(ProtocolMessage *req, ProtocolMessage *res,
                                  int responseType, FILE *out) {
  char buf[1024];
  vscode_make_success_response(req, res, responseType);
  vscode_serialize_response(buf, sizeof buf, res);
  fprintf(log, "%s\n", buf);
  fprintf(out, buf);
}

/*
* Generate response to InitializeRequest
* Send InitializedEvent
*/
static void handle_initialize_request(ProtocolMessage *req,
                                      ProtocolMessage *res, FILE *out) {
  char buf[1024];
  vscode_make_success_response(req, res, VSCODE_INITIALIZE_RESPONSE);
  res->u.Response.u.InitializeResponse.body.supportsConfigurationDoneRequest =
      1;
  vscode_serialize_response(buf, sizeof buf, res);
  fprintf(log, "%s\n", buf);
  fprintf(out, buf);
  vscode_make_initialized_event(res);
  vscode_serialize_event(buf, sizeof buf, res);
  fprintf(log, "%s\n", buf);
  fprintf(out, buf);
}

/*
* Generate response to ThreadRequest
*/
static void handle_thread_request(ProtocolMessage *req,
  ProtocolMessage *res, FILE *out) {
  char buf[1024];
  vscode_make_success_response(req, res, VSCODE_THREAD_RESPONSE);
  res->u.Response.u.ThreadResponse.threads[0].id = 1;
  strncpy(res->u.Response.u.ThreadResponse.threads[0].name, "Lua Thread", sizeof res->u.Response.u.ThreadResponse.threads[0].name);
  vscode_serialize_response(buf, sizeof buf, res);
  fprintf(log, "%s\n", buf);
  fprintf(out, buf);
}

/*
* Handle StackTraceRequest
*/
static void handle_stack_trace_request(ProtocolMessage *req,
  ProtocolMessage *res, lua_State *L, FILE *out) {
  char buf[1024];
  lua_Debug entry;
  int depth = 0;
  vscode_make_success_response(req, res, VSCODE_STACK_TRACE_RESPONSE);
  while (lua_getstack(L, depth, &entry) && depth < req->u.Request.u.StackTraceRequest.levels && depth < MAX_STACK_FRAMES)
  {
    int status = lua_getinfo(L, "Sln", &entry);
    assert(status);
    const char *src = entry.source;
    if (*src == '@') src++;
    const char *last_path_delim1 = strrchr(src, '/');
    const char *last_path_delim2 = strrchr(src, '\\');
    const char *last_path_delim = last_path_delim1;
    if (last_path_delim2 && last_path_delim2 > last_path_delim1)
      last_path_delim = last_path_delim2;
    char path[1024];
    char name[256];
    if (last_path_delim) {
//      int pathlen = last_path_delim - src;
//      memcpy(path, src, pathlen);
//      path[pathlen] = 0;
      strncpy(path, src, sizeof path);
      strncpy(name, last_path_delim + 1, sizeof name);
    }
    else {
      strncpy(name, src, sizeof name);
      path[0] = 0;
    }
    for (char *p = path; *p != 0; p++)
      if (*p == '\\') *p = '/';
    res->u.Response.u.StackTraceResponse.stackFrames[depth].id = depth;
    strncpy(res->u.Response.u.StackTraceResponse.stackFrames[depth].source.path, path, sizeof res->u.Response.u.StackTraceResponse.stackFrames[depth].source.path);
    strncpy(res->u.Response.u.StackTraceResponse.stackFrames[depth].source.name, name, sizeof res->u.Response.u.StackTraceResponse.stackFrames[depth].source.name);
    res->u.Response.u.StackTraceResponse.stackFrames[depth].line = entry.currentline;
    const char *funcname = entry.name ? entry.name : "?";
    strncpy(res->u.Response.u.StackTraceResponse.stackFrames[depth].name, funcname, sizeof res->u.Response.u.StackTraceResponse.stackFrames[depth].name);
    depth++;
  }
  res->u.Response.u.StackTraceResponse.totalFrames = depth;
  vscode_serialize_response(buf, sizeof buf, res);
  fprintf(log, "%s\n", buf);
  fprintf(out, buf);
}


static int thread_event_sent = 0;
static int stopping = 1;

/**
 * Called via Lua Hook or via main
 * If called from main then init is true and ar == NULL
 */
static void debugger(lua_State *L, bool init, lua_Debug *ar, FILE *in,
                     FILE *out) {
  char buf[4096] = {0};
  ProtocolMessage req, res;
  if (!init) {
    /* running within Lua at line change */
    if (!thread_event_sent) {
      /* thread started - only sent once in the debug session */
      thread_event_sent = 1;
      send_thread_event(&res, true, out);
      /* Inform VSCode we have stopped */
      send_stopped_event(&res, "entry", out);
    }
    else {
      /* Inform VSCode we have stopped */
      send_stopped_event(&res, "step", out);
    }
  }
  /* Get command from VSCode 
   * VSCode commands begin with the sequence:
   * 'Content-Length: nnn\r\n\r\n'
   * This is followed by nnn bytes which has a JSON
   * format request message
   * We currently don't bother checking the
   * \r\n\r\n sequence for incoming messages 
   */
  while (fgets(buf, sizeof buf, in) != NULL) {
    buf[sizeof buf - 1] = 0; /* NULL terminate - just in case */
    const char *bufp = strstr(buf, "Content-Length: ");
    if (bufp != NULL) {
      bufp += 16;
      /* get the message length */
      int len = atoi(bufp);
      if (len >= sizeof buf) {
        /* FIXME */
        fprintf(log, "FATAL ERROR - Content-Length = %d is greater than bufsize\n", len);
        exit(1);
      }
      buf[0] = 0;
      /* skip blank line - actually \r\n */
      if (fgets(buf, sizeof buf, stdin) == NULL) break;
      /* Now read exact number of characters */
      buf[0] = 0;
      if (fread(buf, len, 1, stdin) != 1) {
        fprintf(log, "FATAL ERROR - cannot read %d bytes\n", len);
        exit(1);
      }
      buf[len] = 0;
      fprintf(log, "Content-Length: %d\r\n\r\n%s", len, buf);

      /* Parse the VSCode request */
      int command = vscode_parse_message(buf, sizeof buf, &req, log);
      switch (command) {
        case VSCODE_INITIALIZE_REQUEST: {
          if (init) {
            handle_initialize_request(&req, &res, out);
            send_output_event(&res, "Debugger initialized\n", out);
          }
          break;
        }
        case VSCODE_LAUNCH_REQUEST: {
          if (init) {
            const char *progname = req.u.Request.u.LaunchRequest.program;
            fprintf(log, "Launching '%s'\n", progname);
            int status = luaL_loadfile(L, progname);
            if (status != LUA_OK) {
              char temp[1024];
              snprintf(temp, sizeof temp, "Failed to launch %s due to error: %s\n", progname, lua_tostring(L, -1));
              send_output_event(&res, temp, out);
              send_error_response(&req, &res, VSCODE_LAUNCH_RESPONSE,
                                  "Launch failed", out);
              lua_pop(L, 1);
            }
            if (status == LUA_OK) {
              if (lua_pcall(L, 0, 0, 0)) {
                send_output_event(&res, "Program terminated with error\n", out);
                send_output_event(&res, lua_tostring(L, -1), out);
                lua_pop(L, 1);
              }
            }
          }
        }
        case VSCODE_STACK_TRACE_REQUEST: {
          handle_stack_trace_request(&req, &res, L, out);
          break;
        }
        case VSCODE_DISCONNECT_REQUEST: {
          send_success_response(&req, &res, VSCODE_DISCONNECT_RESPONSE, out);
          exit(0);
        }
        case VSCODE_THREAD_REQUEST: {
          handle_thread_request(&req, &res, out);
          break;
        }
        default:
          send_error_response(&req, &res, command, "not yet implemented", out);
          break;
      }
    }
    else {
      fprintf(log, "Unexpected: %s\n", buf);
    }
  }
}

/* 
* Lua Hook used by the debugger 
* Setup to intercept at every line change
*/
void ravi_debughook(lua_State *L, lua_Debug *ar) {
  int event = ar->event;
  if (event == LUA_HOOKLINE) {
    debugger(L, false, ar, stdin, stdout);
  }
}

/*
* Entry point for the debugger
* The debugger will use stdin/stdout to interact with VSCode
* The protocol used is described in protocol.h.
*/
int main(int argc, const char *argv[]) {
  log = fopen("\\temp\\out1.txt", "w");
  if (log == NULL) exit(1);
#ifdef _WIN32
  /* The VSCode debug protocol requires binary IO */
  _setmode(_fileno(stdout), _O_BINARY);
#endif
  /* switch off buffering */
  setbuf(log, NULL);
  setbuf(stdout, NULL);
  lua_State *L = luaL_newstate(); /* create Lua state */
  if (L == NULL) { return EXIT_FAILURE; }
  luaL_checkversion(L); /* check that interpreter has correct version */
  /* TODO need to redirect the stdin/stdout used by Lua */
  luaL_openlibs(L);     /* open standard libraries */
  lua_sethook(L, ravi_debughook, LUA_MASKCALL | LUA_MASKLINE | LUA_MASKRET, 0);
  debugger(L, true, NULL, stdin, stdout);
  lua_close(L);
  fclose(log);
  return 0;
}
