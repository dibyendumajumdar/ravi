#include <signal.h>
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

static FILE *log = NULL;

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

/**
 * Called via Lua Hook or via main
 * If called from main then init is true and ar = NULL
 */
static void debugger(lua_State *L, int init, lua_Debug *ar, FILE *in,
                     FILE *out) {
  char buf[4096] = {0};
  ProtocolMessage req, res;
  while (fgets(buf, sizeof buf, in) != NULL) {
    buf[sizeof buf - 1] = 0;
    if (strncmp(buf, "Content-Length: ", 16) == 0) {
      int len = atoi(buf + 16);
      if (len >= sizeof buf) {
        fprintf(log, "%d: > bufsize\n", len);
        exit(1);
      }
      buf[0] = 0;
      /* skip blank line */
      if (fgets(buf, sizeof buf, stdin) == NULL) break;
      /* Now read exact number of characters */
      buf[0] = 0;
      if (fread(buf, len, 1, stdin) != 1) {
        fprintf(log, "Cannot read %d bytes\n", len);
        exit(1);
      }
      buf[len] = 0;
      fprintf(log, "Content-Length: %d\r\n\r\n%s", len, buf);
      int command = vscode_parse_message(buf, sizeof buf, &req, log);
      switch (command) {
        case VSCODE_INITIALIZE_REQUEST: {
          if (init) {
            vscode_make_success_response(&req, &res,
                                         VSCODE_INITIALIZE_RESPONSE);
            res.u.Response.u.InitializeResponse.body
                .supportsConfigurationDoneRequest = 1;
            vscode_serialize_response(buf, sizeof buf, &res);
            fprintf(log, "%s\n", buf);
            fprintf(out, buf);
            vscode_make_initialized_event(&res);
            vscode_serialize_event(buf, sizeof buf, &res);
            fprintf(log, "%s\n", buf);
            fprintf(out, buf);
            send_output_event(&res, "Debugger initialized", out);
          }
          break;
        }
        case VSCODE_LAUNCH_REQUEST: {
          if (init) {
            const char *progname = res.u.Request.u.LaunchRequest.program;
            int status = luaL_loadfile(L, progname);
            if (status != LUA_OK) {
              send_output_event(&res, "Launch failed", out);
              send_output_event(&res, lua_tostring(L, -1), out);
              send_error_response(&req, &res, VSCODE_LAUNCH_RESPONSE,
                                  "Launch failed", out);
              lua_pop(L, 1);
            }
            if (status == LUA_OK) {
              if (lua_pcall(L, 0, 0, 0)) {
                send_output_event(&res, "Program terminated with error", out);
                send_output_event(&res, lua_tostring(L, -1), out);
                lua_pop(L, 1);
              }
            }
          }
        }
        case VSCODE_DISCONNECT_REQUEST: {
          send_success_response(&req, &res, VSCODE_DISCONNECT_RESPONSE, out);
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

void ravi_debughook(lua_State *L, lua_Debug *ar) {}

int main(int argc, const char *argv[]) {
  log = fopen("\\temp\\out1.txt", "w");
  if (log == NULL) exit(1);
#ifdef _WIN32
  /* The VSCode debug protocol requires binary IO */
  _setmode(_fileno(stdout), _O_BINARY);
#endif
  setbuf(log, NULL);
  setbuf(stdout, NULL);
  lua_State *L = luaL_newstate(); /* create Lua state */
  if (L == NULL) { return EXIT_FAILURE; }
  luaL_checkversion(L); /* check that interpreter has correct version */
  luaL_openlibs(L);     /* open standard libraries */
  lua_sethook(L, ravi_debughook, LUA_MASKCALL | LUA_MASKLINE | LUA_MASKRET, 0);
  debugger(L, 1, NULL, stdin, stdout);
  lua_close(L);
  fclose(log);
  return 0;
}
