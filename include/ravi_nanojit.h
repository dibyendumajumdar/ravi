/******************************************************************************
* Copyright (C) 2015-2017 Dibyendu Majumdar
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

#ifndef RAVI_NANOJIT_H
#define RAVI_NANOJIT_H

#ifdef USE_NANOJIT
#include "dmr_c.h"

#ifdef __cplusplus
extern "C" {
#endif

// TODO we probably do not need all the headers
// below

#define LUA_CORE

#include "lprefix.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "lvm.h"

typedef enum {
  LUA__TNIL = LUA_TNIL,
  LUA__TBOOLEAN = LUA_TBOOLEAN,
  LUA__TLIGHTUSERDATA = LUA_TLIGHTUSERDATA,
  LUA__TNUMBER = LUA_TNUMBER,
  LUA__TSTRING = ctb(LUA_TSTRING),
  LUA__TTABLE = ctb(LUA_TTABLE),
  RAVI__TLTABLE = ctb(LUA_TTABLE),
  RAVI__TIARRAY = ctb(RAVI_TIARRAY),
  RAVI__TFARRAY = ctb(RAVI_TFARRAY),
  LUA__TFUNCTION = ctb(LUA_TFUNCTION),
  LUA__TUSERDATA = ctb(LUA_TUSERDATA),
  LUA__TTHREAD = ctb(LUA_TTHREAD),
  LUA__TLCL = ctb(LUA_TLCL),
  LUA__TLCF = LUA_TLCF,
  LUA__TCCL = ctb(LUA_TCCL),
  LUA__TSHRSTR = ctb(LUA_TSHRSTR),
  LUA__TLNGSTR = ctb(LUA_TLNGSTR),
  LUA__TNUMFLT = LUA_TNUMFLT,
  LUA__TNUMINT = LUA_TNUMINT
} lua_typecode_t;



#ifdef __cplusplus
};
#endif

#endif /* USE_GCCJIT */

#endif /* RAVI_RAVI_GCCJIT_H */
