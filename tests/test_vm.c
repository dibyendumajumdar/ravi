#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

/*
** LUA_TFUNCTION variants:
** 0 - Lua function
** 1 - light C function
** 2 - regular C function (closure)
*/

/* Variant tags for functions */
#define LUA_TLCL	(LUA_TFUNCTION | (0 << 4))  /* Lua closure */
#define LUA_TLCF	(LUA_TFUNCTION | (1 << 4))  /* light C function */
#define LUA_TCCL	(LUA_TFUNCTION | (2 << 4))  /* C closure */


/* Variant tags for strings */
#define LUA_TSHRSTR	(LUA_TSTRING | (0 << 4))  /* short strings */
#define LUA_TLNGSTR	(LUA_TSTRING | (1 << 4))  /* long strings */


/* Variant tags for numbers */
#define LUA_TNUMFLT	(LUA_TNUMBER | (0 << 4))  /* float numbers */
#define LUA_TNUMINT	(LUA_TNUMBER | (1 << 4))  /* integer numbers */


/* Bit mark for collectable types */
#define BIT_ISCOLLECTABLE	(1 << 6)

/* mark a tag as collectable */
#define ctb(t)			((t) | BIT_ISCOLLECTABLE)

union ValueX {
  double n;
  long long i;
};

typedef struct {
  int lo;
  int hi;
} PartsX;

union TypeX {
  double d;
  PartsX tt;
  unsigned char bytes[8];
};

typedef struct {
  union ValueX value_;
  union TypeX tt_;
} MyValueT;

static void dumpll(uint64_t d)
{
  union {
    uint64_t d;
    unsigned char bytes[8];
  } tt;

  tt.d = d;
  // Litle endian assumed
  for (int i = 7; i >= 0; i--) {
    printf("%02x", tt.bytes[i]);
  }
  printf("\n");
}

static void dumpd(double d)
{
  union {
    double d;
    unsigned char bytes[8];
  } tt;

  tt.d = d;
  // Litle endian assumed
  for (int i = 7; i >=0 ; i--) {
    printf("%02x", tt.bytes[i]);
  }
  printf("\n");
}

static void dumpvalue(MyValueT* v)
{
  // Litle endian assumed
  for (int i = 7; i >= 0; i--) {
    printf("%02x", v->tt_.bytes[i]);
  }
  printf("\n");
}


#define QNAN 0x7ffc0000
#define QVAL 0x0000ffff

#define LUA__TNUMFLT   (QNAN | LUA_TNUMFLT)
#define LUA__TNUMINT   (QNAN | LUA_TNUMINT)
#define LUA__TNUMBER   (QNAN | LUA_TNUMINT)
#define LUA__TTABLE    (QNAN | ctb(LUA_TTABLE))

static void tryme() {
  MyValueT v = { 0 };

  //v.tt_.d = 1.0;
  //v.tt_.d /= 0;
  //dumpvalue(&v);

  v.tt_.tt.hi = LUA__TNUMINT;
  dumpvalue(&v);

  if (isnan(v.tt_.d))
    printf("Is NaN set as expected\n");

  if ((v.tt_.tt.hi & QNAN) == QNAN)
    printf("QNAN set as expected\n");
  if ((v.tt_.tt.hi & QVAL) == LUA_TNUMINT)
    printf("LUA_TNUMINT set as expected\n");


  v.tt_.d = 1.5;
  if ((v.tt_.tt.hi & QNAN) != QNAN)
    printf("QNAN unset as expected\n");

  v.tt_.tt.hi = LUA__TTABLE;
  dumpvalue(&v);

  if ((v.tt_.tt.hi & QNAN) == QNAN)
    printf("QNAN set as expected\n");
  if ((v.tt_.tt.hi & QVAL) == ctb(LUA_TTABLE))
    printf("LUA_TTABLE set as expected\n");

  v.tt_.d = 42;
  if ((v.tt_.tt.hi & QNAN) != QNAN)
    printf("QNAN unset as expected\n");
}


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

#if 0
static int test_luacompfile(const char *code)
{
  int rc = 0;
  lua_State *L;
  L = luaL_newstate();
  if (luaL_loadfile(L, code) != 0) {
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
static int test_luafileexec1(const char *code, int expected)
{
  int rc = 0;
  lua_State *L;
  L = luaL_newstate();
  luaL_openlibs(L);  /* open standard libraries */
  if (luaL_loadfile(L, code) != 0) {
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
#endif


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

int main() 
{
    int failures = 0;
    //  
#if 0
    tryme();
#else
    failures += test_luacompexec1("function test(); local x: integer = 1; return function (j) x = j; return x; end; end; fn = test(); return fn('55')", 55);
    failures += test_luacompexec1("ravi.auto(true); function arrayaccess (); local x: integer[] = {5}; return x[1]; end; assert(ravi.compile(arrayaccess)); return arrayaccess()", 5);
    failures += test_luacompexec1("ravi.auto(true); function cannotload (msg, a,b); assert(not a and string.find(b, msg)); end; ravi.compile(cannotload); return 1", 1);
    failures += test_luacompexec1("ravi.auto(true); function z(); local a = 5; a = a + 1; return a; end; ravi.compile(z); return z()", 6);
    failures += test_luacompexec1("ravi.auto(true); function z(x); x[1] = 5; return x[1]; end; ravi.compile(z); return z({})", 5);
    failures += test_luacompexec1("ravi.auto(true); function z(x,y) return x<y end; ravi.compile(z); return not z(2,1)", 1);
    failures += test_luacompexec1("ravi.auto(true); local function x(); local d:number = 5.0; return d+5 == 5+d and d-5 == 5-d and d*5 == 5*d; end; local y = x(); return y", 1);
    failures += test_luacompexec1("ravi.auto(true); function x(f); local i : integer, j : integer = f(); return i + j; end; return ravi.compile(x)", 1);
    failures += test_luacompexec1("ravi.auto(true); local function z(a); print(a); return a+1; end; local function x(yy); local j = 5; j = yy(j); return j; end; local y = x(z); return y", 6);
    failures += test_luacompexec1("ravi.auto(true); local function z(a,p); p(a); return 6; end; local function x(yy,p); local j = 5; j = yy(j,p); return j; end; local y = x(z,print); return y", 6);
    failures += test_luacompexec1("ravi.auto(true); local function x(yy); local j = 5; yy(j); return j; end; local y = x(print); return y", 5);
    failures += test_luacompexec1("ravi.auto(true); local function x(); local i, j:integer; j=0; for i=1,1000000000 do; j = j+1; end; return j; end; local y = x(); print(y); return y", 1000000000);
    failures += test_luacompexec1("ravi.auto(true); local function x(); local j:number; for i=1,1000000000 do; j = j+1; end; return j; end; local y = x(); print(y); return y", 1000000000);

    failures += test_luacompexec1("ravi.auto(true); local function x(); local j = 0; for i=2,6,3 do; j = i; end; return j; end; local y = x(); print(y); return y", 5);
    failures += test_luacompexec1("ravi.auto(true); local function x(); local j = 0; for i=2.0,6.0,3.0 do; j = i; end; return j; end; local y = x(); print(y); return y", 5);
    failures += test_luacompexec1("ravi.auto(true); local function x(); local a=5; return 1004,2; end; local y; y = x(); print(y); return y", 1004);
    failures += test_luacompexec1("ravi.auto(true); local function x(); if 1 == 2 then; return 5.0; end; return 1.0; end; local z = x(); print(z); return z", 1);
    failures += test_luacompexec1("ravi.auto(true); local function x(y); if y == 1 then; return 1.0; elseif y == 5 then; return 2.0; else; return 3.0; end; end; local z = x(5); print(z); return z", 2);
    failures += test_luacompexec1("ravi.auto(true); local function x(y); if y == 1 then; return 1.0; elseif y == 5 then; return 2.0; else; return 3.0; end; end; local z = x(4); print(z); return z", 3);
    failures += test_luacompexec1("ravi.auto(true); local function x(y,z); if y == 1 then; if z == 1 then; return 99.0; else; return z; end; elseif y == 5 then; return 2.0; else; return 3.0; end; end; local z = x(1,1); print(z); return z", 99);

    failures += test_luacompexec1("local x:integer[] = {1}; local i:integer = 1; local d:integer = x[i]; x[i] = 5; return d*x[i];", 5);
    failures += test_luacompexec1("ravi.auto(true); local function x(); local a:number = 1.0; return a+127 == 128.0; end; local y = x(); return y", 1);
    failures += test_luacompexec1("ravi.auto(true); local function x(); local a:number = 1.0; return a+128 == 129.0; end; local y = x(); return y", 1);
    failures += test_luacompexec1("ravi.auto(true); local function x(); local a:number = 1.0; return 127+a == 128.0; end; local y = x(); return y", 1);
    failures += test_luacompexec1("ravi.auto(true); local function x(); local a:number = 1.0; return 128+a == 129.0; end; local y = x(); return y", 1);
    failures += test_luacompexec1("ravi.auto(true); local function x(); local a:number = 1.0; return a+1.0 == 1.0+a; end; local y = x(); return y", 1);
    failures += test_luacompexec1("ravi.auto(true); local function x(); local a:integer = 1; return a+127 == 128; end; local y = x(); return y", 1);
    failures += test_luacompexec1("ravi.auto(true); local function x(); local a:integer = 1; return a+128 == 129; end; local y = x(); return y", 1);
    failures += test_luacompexec1("ravi.auto(true); local function x(); local a:integer = 1; return 127+a == 128; end; local y = x(); return y", 1);
    failures += test_luacompexec1("ravi.auto(true); local function x(); local a:integer = 1; return 128+a == 129; end; local y = x(); return y", 1);
    failures += test_luacompexec1("ravi.auto(true); local function x(); local a:integer = 1; return a+1 == 1+a; end; local y = x(); return y", 1);
    failures += test_luacompexec1("ravi.auto(true); local function tryme(x); print(#x); return x; end; local da: number[] = { 5, 6 }; da[1] = 42; da = tryme(da); return da[1];", 42);
    /* following should fail as x is a number[] */
    failures += test_luacompexec1("ravi.auto(true); local function tryme(x); print(#x); x[1] = 'junk'; return x; end; local da: number[] = {}; da[1] = 42; da = tryme(da); return da[1];", 42) == 1 ? 0 : 1;
    failures += test_luacomp1("local t = {}; local da : number[] = {}; da=t[1];") == 1 ? 0 : 1;
    failures += test_luacomp1("local a : integer[] = {}");
    failures += test_luacompexec1("for i=1,10 do; end; return 0", 0);
    failures += test_luacompexec1("local a : number[], j:number = {}; for i=1,10 do; a[i] = i; j = j + a[i]; end; return j", 55);
    failures += test_luacompexec1("local a:integer[] = {}; local i:integer; a[1] = i+5; i = a[1]; return i", 5);
    failures += test_luacompexec1("local function tryme(); local i,j = 5,6; return i,j; end; local i:integer, j:integer = tryme(); return i+j", 11);
    failures += test_luacompexec1("local i:integer,j:integer = 1; j = i*j+i; return j", 1);
    failures += test_luacompexec1("local i:integer; for i=1,10 do; print(i); end; print(i); return i", 0);
    failures += test_luacomp1("local i:integer, j:number; i,j = f(); j = i*j+i");
    failures += test_luacomp1("local d; d = f()");
    failures += test_luacomp1("local d, e; d, e = f(), g()");
    failures += test_luacomp1("local i:integer, d:number = f()");
    failures += test_luacomp1("local i:integer,j:number,k:integer = f(), g()");
    failures += test_luacomp1("local f = function(); return; end; local d:number, j:integer = f(); return d");
    failures += test_luacomp1("local d = f()");
    failures += test_luacomp1("return (-1.25 or -4)+0");
    failures += test_luacomp1("f = nil; local f; function f(a); end");
    failures += test_luacomp1("local max, min = 0x7fffffff, -0x80000000; assert(string.format(\"%d\", min) == \"-2147483648\"); max, min = 0x7fffffffffffffff, -0x8000000000000000; if max > 2.0 ^ 53 then; end;");
    failures += test_luacomp1("local function F (m); local function round(m); m = m + 0.04999; return format(\"%.1f\", m);end; end");
    failures += test_luacomp1("local b:integer = 6; local i:integer = 5+b; return i");
    failures += test_luacompexec1("local b:integer = 6; local i:integer = 5+b; return i", 11);
    failures += test_luacomp1("local f = function(); end");
    failures += test_luacomp1("local b:integer = 6; b = nil; return i") == 0; /* should fail */
    failures += test_luacomp1("local f = function(); local function y() ; end; end");
    failures += test_luacompexec1("return -(1 or 2)", -1);
    failures += test_luacompexec1("return (1 and 2)+(-1.25 or -4) == 0.75", 1);
    failures += test_luacomp1("local a=1; if a==0 then; a = 2; else a=3; end;");
    failures += test_luacomp1("local f = function(); return; end; local d:number = 5.0; d = f(); return d");
    failures += test_luacomp1("local f = function(); return; end; local d = 5.0; d = f(); return d");
#endif
    return failures ? 1 : 0;
}
