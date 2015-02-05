#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"


/* test supplied lua code compiles */
static int test_luacomp1(const char *code)
{
    int rc = 0;
    lua_State *L;
    L = luaL_newstate();
    if (luaL_loadbuffer(L, code, strlen(code), "testfunc") != 0) {
      rc = 1;
      fprintf(stderr, "%s\n", lua_tostring(L, -1));
      lua_pop(L, 1);  /* pop error message from the stack */
    }
    else
      ravi_dump_function(L);
    lua_close(L);
    return rc;
}

/* test supplied lua code compiles */
static int test_luacompexec1(const char *code, int expected)
{
  int rc = 0;
  lua_State *L;
  L = luaL_newstate();
  luaL_openlibs(L);  /* open standard libraries */
  if (luaL_loadbuffer(L, code, strlen(code), "testfunc") != 0) {
    rc = 1;
    fprintf(stderr, "%s\n", lua_tostring(L, -1));
    lua_pop(L, 1);  /* pop error message from the stack */
  }
  else {
    ravi_dump_function(L);
    time_t start = time(NULL);
    if (lua_pcall(L, 0, 1, 0) != 0) {
      rc = 1;
      fprintf(stderr, "%s\n", lua_tostring(L, -1));
    }
    else {
      time_t end = time(NULL);
      ravi_dump_stack(L, "after executing function");
      printf("time taken = %f\n", difftime(end, start));
      lua_Integer got = 0;
      if (lua_isboolean(L, -1))
        got = lua_toboolean(L, -1) ? 1 : 0;
      else
        got = lua_tointeger(L, -1);
      if (got != expected) {
        rc = 1;
      }
    }
  }
  lua_close(L);
  return rc;
}

int main(int argc, const char *argv[]) 
{
    int failures = 0;
    failures += test_luacompexec1("local x:int[] = {1}; local i:int = 1; local d:int = x[i]; x[i] = 5; return d*x[i];", 5);
    failures += test_luacompexec1("local d:double = 5.0; return d+5 == 5+d and d-5 == 5-d and d*5 == 5*d", 1);
    failures += test_luacompexec1("local a:double = 1.0; return a+127 == 128.0;", 1);
    failures += test_luacompexec1("local a:double = 1.0; return a+128 == 129.0;", 1);
    failures += test_luacompexec1("local a:double = 1.0; return 127+a == 128.0;", 1);
    failures += test_luacompexec1("local a:double = 1.0; return 128+a == 129.0;", 1);
    failures += test_luacompexec1("local a:double = 1.0; return a+1.0 == 1.0+a;", 1);
    failures += test_luacompexec1("local a:int = 1; return a+127 == 128;", 1);
    failures += test_luacompexec1("local a:int = 1; return a+128 == 129;", 1);
    failures += test_luacompexec1("local a:int = 1; return 127+a == 128;", 1);
    failures += test_luacompexec1("local a:int = 1; return 128+a == 129;", 1);
    failures += test_luacompexec1("local a:int = 1; return a+1 == 1+a;", 1);
    failures += test_luacomp1("local t = {}; local da : double[] = {}; da=t[1];") == 1 ? 0 : 1;
    failures += test_luacompexec1("local function tryme(x); print(#x); return x; end; local da: double[] = { 5, 6 }; da[1] = 42; da = tryme(da); return da[1];", 42);
    /* following should fail as x is a double[] */
    failures += test_luacompexec1("local function tryme(x); print(#x); x[1] = 'junk'; return x; end; local da: double[] = {}; da[1] = 42; da = tryme(da); return da[1];", 42) == 1 ? 0 : 1;
    failures += test_luacompexec1("for i=1,10 do; end; return 0", 0);
    failures += test_luacomp1("local a : int[] = {}");
    failures += test_luacompexec1("local a : double[], j:double = {}; for i=1,10 do; a[i] = i; j = j + a[i]; end; return j", 55);
    failures += test_luacompexec1("local a:int[] = {}; local i:int; a[1] = i+5; i = a[1]; return i", 5);
    failures += test_luacompexec1("local function tryme(); local i,j = 5,6; return i,j; end; local i:int, j:int = tryme(); return i+j", 11);
    failures += test_luacompexec1("local i:int,j:int = 1; j = i*j+i; return j", 1);
    failures += test_luacompexec1("local i:int; for i=1,10 do; print(i); end; print(i); return i", 0);
    failures += test_luacomp1("local i:int, j:double; i,j = f(); j = i*j+i");
    failures += test_luacomp1("local d; d = f()");
    failures += test_luacomp1("local d, e; d, e = f(), g()");
    failures += test_luacomp1("local i:int, d:double = f()");
    failures += test_luacomp1("local i:int,j:double,k:int = f(), g()");
    failures += test_luacomp1("local f = function(); return; end; local d:double, j:int = f(); return d");
    failures += test_luacomp1("local d = f()");
    failures += test_luacomp1("return (-1.25 or -4)+0");
    failures += test_luacomp1("f = nil; local f; function f(a); end");
    failures += test_luacomp1("local max, min = 0x7fffffff, -0x80000000; assert(string.format(\"%d\", min) == \"-2147483648\"); max, min = 0x7fffffffffffffff, -0x8000000000000000; if max > 2.0 ^ 53 then; end;");
    failures += test_luacomp1("local function F (m); local function round(m); m = m + 0.04999; return format(\"%.1f\", m);end; end");
    failures += test_luacomp1("local b:int = 6; local i:int = 5+b; return i");
    failures += test_luacompexec1("local b:int = 6; local i:int = 5+b; return i", 11);
    failures += test_luacomp1("local f = function(); end");
    failures += test_luacomp1("local b:int = 6; b = nil; return i") == 0; /* should fail */
    failures += test_luacomp1("local f = function(); local function y() ; end; end");
    failures += test_luacompexec1("return -(1 or 2)", -1);
    failures += test_luacompexec1("return (1 and 2)+(-1.25 or -4) == 0.75", 1);
    failures += test_luacomp1("local a=1; if a==0 then; a = 2; else a=3; end;");
    failures += test_luacomp1("local f = function(); return; end; local d:double = 5.0; d = f(); return d");
    failures += test_luacompexec1("local j:double; for i=1,1000000000 do; j = j+1; end; return j", 1000000000);
    failures += test_luacompexec1("local i, j:int; j=0; for i=1,1000000000 do; j = j+1; end; return j", 1000000000);
    failures += test_luacomp1("local f = function(); return; end; local d = 5.0; d = f(); return d");
    return failures ? 1 : 0;
}
