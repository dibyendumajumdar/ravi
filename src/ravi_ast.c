#define LUA_CORE

#include "lprefix.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "lua.h"

#include "lcode.h"
#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "llex.h"
#include "lmem.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lparser.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ravi_ast.h"

// README
// THIS IS NOT WORKING - IT IS WORK IN PROGRESS
// Set test_ast.c for a way to exercise this.
// Also we enable an API raviload() to get to the entry point here

// We need a simple bump allocator
// Following is based on code from Linus Torvalds sparse project

/*
* Our "blob" allocator works on chunks that are multiples
* of this size (the underlying allocator may be a mmap that
* cannot handle smaller chunks, for example, so trying to
* allocate blobs that aren't aligned is not going to work).
*/
#define CHUNK 32768

static size_t alignment = sizeof(double);

struct allocation_blob {
  struct allocation_blob *next;
  size_t left, offset;
  unsigned char data[];
};

struct allocator_struct {
  struct allocation_blob *blobs;
  /* statistics */
  size_t allocations, total_bytes, useful_bytes;
};

void *blob_alloc(size_t size) {
  void *ptr;
  ptr = malloc(size);
  if (ptr != NULL) memset(ptr, 0, size);
  return ptr;
}

void blob_free(void *addr, size_t size) {
  (void)size;
  free(addr);
}

void init_allocator(struct allocator_struct *desc) {
  desc->blobs = NULL;
  desc->allocations = desc->total_bytes = desc->useful_bytes = 0;
}

void drop_all_allocations(struct allocator_struct *desc) {
  struct allocation_blob *blob = desc->blobs;

  desc->blobs = NULL;
  desc->allocations = 0;
  desc->total_bytes = 0;
  desc->useful_bytes = 0;
  while (blob) {
    struct allocation_blob *next = blob->next;
    blob_free(blob, CHUNK);
    blob = next;
  }
}

void *allocate(struct allocator_struct *desc, size_t size) {
  struct allocation_blob *blob = desc->blobs;
  void *retval;
  desc->allocations++;
  desc->useful_bytes += size;
  size = (size + alignment - 1) & ~(alignment - 1);
  if (!blob || blob->left < size) {
    size_t offset, chunking = CHUNK;
    struct allocation_blob *newblob = blob_alloc(chunking);
    if (!newblob) { abort(); }
    desc->total_bytes += chunking;
    newblob->next = blob;
    blob = newblob;
    desc->blobs = newblob;
    offset = offsetof(struct allocation_blob, data);
    offset = (offset + alignment - 1) & ~(alignment - 1);
    blob->left = chunking - offset;
    blob->offset = offset - offsetof(struct allocation_blob, data);
  }
  retval = blob->data + blob->offset;
  blob->offset += size;
  blob->left -= size;
  return retval;
}

void show_allocations(struct allocator_struct *x) {
  fprintf(stderr,
          "allocator: %d allocations, %d bytes (%d total bytes, "
          "%6.2f%% usage, %6.2f average size)\n",
          (int)x->allocations, (int)x->useful_bytes, (int)x->total_bytes,
          100 * (double)x->useful_bytes / x->total_bytes,
          (double)x->useful_bytes / x->allocations);
}

LClosure *raviY_parser(lua_State *L, ZIO *z, Mbuffer *buff, Dyndata *dyd,
                       const char *name, int firstchar) {
  LexState lexstate;
  // Mbuffer mbuff;
  // FuncState funcstate;
  // LClosure *cl = luaF_newLclosure(L, 1);  /* create main closure */
  // setclLvalue(L, L->top, cl);  /* anchor it (to avoid being collected) */
  // luaD_inctop(L);
  lexstate.h = luaH_new(L);         /* create table for scanner */
  sethvalue(L, L->top, lexstate.h); /* anchor it */
  luaD_inctop(L);
  TString *src = luaS_new(L, name); /* create and anchor TString */
  setsvalue(L, L->top, src);
  luaD_inctop(L);
  // funcstate.f = cl->p = luaF_newproto(L);
  // funcstate.f->source = luaS_new(L, name);  /* create and anchor TString */
  // lua_assert(iswhite(funcstate.f));  /* do not need barrier here */
  lexstate.buff = buff;
  lexstate.dyd = dyd;
  dyd->actvar.n = dyd->gt.n = dyd->label.n = 0;
  luaX_setinput(L, &lexstate, z, src, firstchar);
  // mainfunc(&lexstate, &funcstate);
  // lua_assert(!funcstate.prev && funcstate.nups == 1 && !lexstate.fs);
  /* all scopes should be correctly finished */
  lua_assert(dyd->actvar.n == 0 && dyd->gt.n == 0 && dyd->label.n == 0);
  L->top--; /* remove source name */
  L->top--; /* remove scanner's table */
  // return cl;  /* closure is on the stack, too */
  return luaY_parser(L, z, buff, dyd, name, firstchar);
}
