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
#include "lobject.h"

#ifndef RAVI_USE_ASMVM
#error This file can only be compiled with ASMVM
#endif

struct MyValue {
	int type;
	union {
		lua_Integer i;
		lua_Number n;
		const char *s;
	} u;
};

/**
 * Compiles the given code and invokes it, passing params to it.
 * Checks that the code returns given number of values that match expected
 * results. Returns 0 if no error.
 */
static int do_asmvm_test(
	const char *code, /* code to compiled and invoked */
	int nparams, struct MyValue *params, /* parameters */
	int nresults, struct MyValue *expected) /* expected return values */
{
	int rc = 0;
	lua_State *L;
	L = luaL_newstate();
	luaL_openlibs(L);  /* open standard libraries */
	if (luaL_loadbuffer(L, code, strlen(code), "chunk") != 0) {
		rc = 1;
		fprintf(stderr, "Failed to load chunk: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);  /* pop error message from the stack */
		goto Lerror;
	}
	if (lua_pcall(L, 0, 1, 0) != 0) {
		rc = 1;
		fprintf(stderr, "Failed to run chunk: %s\n", lua_tostring(L, -1));
		goto Lerror;
	}
	if (!lua_isfunction(L, -1)) {
		rc = 1;
		fprintf(stderr, "Script did not return a function\n");
		goto Lerror;
	}
	for (int i = 0; i < nparams; i++) {
		switch (params[i].type) {
		case RAVI_TNUMINT: 
			lua_pushinteger(L, params[i].u.i);
			break;
		case RAVI_TNUMFLT:
			lua_pushnumber(L, params[i].u.n);
			break;
		case RAVI_TSTRING:
			lua_pushstring(L, params[i].u.s);
			break;
		default:
			fprintf(stderr, "Unsupported argument type %d\n", params[i].type);
			rc = 1;
			goto Lerror;
		}
	}
	if (lua_pcall(L, nparams, nresults, 0) != 0) {
		rc = 1;
		fprintf(stderr, "Test function failed: %s\n", lua_tostring(L, -1));
		goto Lerror;
	}
	for (int i = nresults - 1, j = -1; i >= 0; i--, j--) {
		switch (expected[i].type) {
		case RAVI_TNUMINT: {
			if (!lua_isinteger(L, j)) {
				fprintf(stderr, "Result %d was expected to be integer\n", i + 1);
				rc = 1;
				goto Lerror;
			}
			lua_Integer num = lua_tointeger(L, j);
			if (num != expected[i].u.i) {
				fprintf(stderr, "Result %d was expected to be %d, but got %d\n", i + 1, (int)expected[i].u.i, (int)num);
				rc = 1;
				goto Lerror;
			}
			break;
		}
		case RAVI_TNUMFLT: {
			if (!lua_isnumber(L, j)) {
				fprintf(stderr, "Result %d was expected to be number\n", i + 1);
				rc = 1;
				goto Lerror;
			}
			lua_Number num = lua_tonumber(L, j);
			if (num != expected[i].u.n) {
				fprintf(stderr, "Result %d was expected to be %g, but got %g\n", i + 1, expected[i].u.n, num);
				rc = 1;
				goto Lerror;
			}
			break;
		}
		case RAVI_TSTRING: {
			if (!lua_isstring(L, j)) {
				fprintf(stderr, "Result %d was expected to be string\n", i + 1);
				rc = 1;
				goto Lerror;
			}
			const char *s = lua_tostring(L, j);
			if (strcmp(s, expected[i].u.s) != 0) {
				fprintf(stderr, "Result %d was expected to be %s, but got %s\n", i + 1, expected[i].u.s, s);
				rc = 1;
				goto Lerror;
			}
			break;
		}
		default: {
			fprintf(stderr, "Result %d has unexpected type\n", i + 1);
			rc = 1;
			goto Lerror;
		}
		}
	}
Lerror:
	lua_close(L);
	return rc;
}

static int test_vm()
{
	int failures = 0;
	struct MyValue args[3];
	struct MyValue results[3];

	args[0].type = RAVI_TNUMINT; args[0].u.i = 42;
	args[1].type = RAVI_TNUMFLT; args[1].u.n = -4.2;
	args[2].type = RAVI_TSTRING; args[2].u.s = "hello";

	results[0].type = RAVI_TNUMINT; results[0].u.i = 42;
	results[1].type = RAVI_TNUMFLT; results[1].u.n = -4.2;
	results[2].type = RAVI_TSTRING; results[2].u.s = "hello";

	failures = do_asmvm_test("return function() end", 0, NULL, 0, NULL); // OP_RETURN 
	failures += do_asmvm_test("return function() return 42 end", 0, NULL, 1, results); // OP_LOADK, OP_RETURN
	failures += do_asmvm_test("return function() return 42, -4.2 end", 0, NULL, 2, results); // OP_LOADK, OP_RETURN
	failures += do_asmvm_test("return function() return 42, -4.2, 'hello' end", 0, NULL, 3, results); // OP_LOADK, OP_RETURN
	failures += do_asmvm_test("return function(a) local b = a; return b end", 1, args, 1, results); // OP_MOVE, OP_RETURN
	failures += do_asmvm_test("return function(a,c) local b,d = a,c; return b,d end", 2, args, 2, results); // OP_MOVE, OP_RETURN

	results[0].u.i = 5;
	failures += do_asmvm_test("return function (a) for i=1,5 do a=i; end return a end", 0, NULL, 1, results); // OP_LOADK, OP_MOVE, OP_RETURN, OP_RAVI_FOPREP_I1, OP_RAVI_FORLOOP_I1
	results[0].u.i = 3;
	failures += do_asmvm_test("return function (a) for i=1,4,2 do a=i; end return a end", 0, NULL, 1, results); // OP_LOADK, OP_MOVE, OP_RETURN, OP_RAVI_FOPREP_IP, OP_RAVI_FORLOOP_IP
	return failures;
}

int main() 
{
	int failures = 0;
	failures += test_vm();
	if (failures)
		printf("FAILED\n");
	else
		printf("OK\n");
    return failures ? 1 : 0;
}
