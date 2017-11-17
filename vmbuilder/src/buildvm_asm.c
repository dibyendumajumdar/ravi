/*
** LuaJIT VM builder: Assembler source code emitter.
** Copyright (C) 2005-2017 Mike Pall. See Copyright Notice in luajit.h
*/

#include "buildvm.h"
#include "lopcodes.h"

/* ------------------------------------------------------------------------ */

#if RAVI_TARGET_X86ORX64
/* Emit bytes piecewise as assembler text. */
static void emit_asm_bytes(BuildCtx *ctx, uint8_t *p, int n)
{
  int i;
  for (i = 0; i < n; i++) {
    if ((i & 15) == 0)
      fprintf(ctx->fp, "\t.byte %d", p[i]);
    else
      fprintf(ctx->fp, ",%d", p[i]);
    if ((i & 15) == 15) putc('\n', ctx->fp);
  }
  if ((n & 15) != 0) putc('\n', ctx->fp);
}

/* Emit relocation */
static void emit_asm_reloc(BuildCtx *ctx, int type, const char *sym)
{
  switch (ctx->mode) {
  case BUILD_elfasm:
    if (type)
      fprintf(ctx->fp, "\t.long %s-.-4\n", sym);
    else
      fprintf(ctx->fp, "\t.long %s\n", sym);
    break;
  case BUILD_coffasm:
    fprintf(ctx->fp, "\t.def %s; .scl 3; .type 32; .endef\n", sym);
    if (type)
      fprintf(ctx->fp, "\t.long %s-.-4\n", sym);
    else
      fprintf(ctx->fp, "\t.long %s\n", sym);
    break;
  default:  /* BUILD_machasm for relative relocations handled below. */
    fprintf(ctx->fp, "\t.long %s\n", sym);
    break;
  }
}

static const char *const jccnames[] = {
  "jo", "jno", "jb", "jnb", "jz", "jnz", "jbe", "ja",
  "js", "jns", "jpe", "jpo", "jl", "jge", "jle", "jg"
};

/* Emit x86/x64 text relocations. */
static void emit_asm_reloc_text(BuildCtx *ctx, uint8_t *cp, int n,
				const char *sym)
{
  const char *opname = NULL;
  if (--n < 0) goto err;
  if (cp[n] == 0xe8) {
    opname = "call";
  } else if (cp[n] == 0xe9) {
    opname = "jmp";
  } else if (cp[n] >= 0x80 && cp[n] <= 0x8f && n > 0 && cp[n-1] == 0x0f) {
    opname = jccnames[cp[n]-0x80];
    n--;
  } else {
err:
    fprintf(stderr, "Error: unsupported opcode for %s symbol relocation.\n",
	    sym);
    exit(1);
  }
  emit_asm_bytes(ctx, cp, n);
  if (strncmp(sym+(*sym == '_'), LABEL_PREFIX, sizeof(LABEL_PREFIX)-1)) {
    /* Various fixups for external symbols outside of our binary. */
    if (ctx->mode == BUILD_elfasm) {
      if (RAVI_32)
	fprintf(ctx->fp, "#if __PIC__\n\t%s lj_wrap_%s\n#else\n", opname, sym);
      fprintf(ctx->fp, "\t%s %s@PLT\n", opname, sym);
      if (RAVI_32)
	fprintf(ctx->fp, "#endif\n");
      return;
    } else if (RAVI_32 && ctx->mode == BUILD_machasm) {
      fprintf(ctx->fp, "\t%s L%s$stub\n", opname, sym);
      return;
    }
  }
  fprintf(ctx->fp, "\t%s %s\n", opname, sym);
}
#else
#error "missing relocation support for this architecture"
#endif

#define ELFASM_PX	"@"

/* Emit an assembler label. */
static void emit_asm_label(BuildCtx *ctx, const char *name, int size, int isfunc)
{
  switch (ctx->mode) {
  case BUILD_elfasm:
    fprintf(ctx->fp,
      "\n\t.globl %s\n"
      "\t.hidden %s\n"
      "\t.type %s, " ELFASM_PX "%s\n"
      "\t.size %s, %d\n"
      "%s:\n",
      name, name, name, isfunc ? "function" : "object", name, size, name);
    break;
  case BUILD_coffasm:
    fprintf(ctx->fp, "\n\t.globl %s\n", name);
    if (isfunc)
      fprintf(ctx->fp, "\t.def %s; .scl 3; .type 32; .endef\n", name);
    fprintf(ctx->fp, "%s:\n", name);
    break;
  case BUILD_machasm:
    fprintf(ctx->fp,
      "\n\t.private_extern %s\n"
      "\t.no_dead_strip %s\n"
      "%s:\n", name, name, name);
    break;
  default:
    break;
  }
}

/* Emit alignment. */
static void emit_asm_align(BuildCtx *ctx, int bits)
{
  switch (ctx->mode) {
  case BUILD_elfasm:
  case BUILD_coffasm:
    fprintf(ctx->fp, "\t.p2align %d\n", bits);
    break;
  case BUILD_machasm:
    fprintf(ctx->fp, "\t.align %d\n", bits);
    break;
  default:
    break;
  }
}

/* ------------------------------------------------------------------------ */

/* Emit assembler source code. */
void emit_asm(BuildCtx *ctx)
{
  int i, rel;

  fprintf(ctx->fp, "\t.file \"buildvm_%s.dasc\"\n", ctx->dasm_arch);
  fprintf(ctx->fp, "\t.text\n");
  emit_asm_align(ctx, 4);

  emit_asm_label(ctx, ctx->StartSymbol, 0, 0);
  if (ctx->mode != BUILD_machasm)
    fprintf(ctx->fp, ".Lbegin:\n");

  for (i = rel = 0; i < ctx->NumberOfSymbols; i++) {
    int32_t ofs = ctx->AllSymbols[i].ofs;
    int32_t next = ctx->AllSymbols[i+1].ofs;
    emit_asm_label(ctx, ctx->AllSymbols[i].name, next - ofs, 1);
    while (rel < ctx->nreloc && ctx->reloc[rel].ofs <= next) {
      BuildReloc *r = &ctx->reloc[rel];
      int n = r->ofs - ofs;
      if (r->type != 0 &&
	  (ctx->mode == BUILD_elfasm || ctx->mode == BUILD_machasm)) {
	emit_asm_reloc_text(ctx, ctx->code+ofs, n, ctx->relocsym[r->sym]);
      } else {
	emit_asm_bytes(ctx, ctx->code+ofs, n);
	emit_asm_reloc(ctx, r->type, ctx->relocsym[r->sym]);
      }
      ofs += n+4;
      rel++;
    }
    emit_asm_bytes(ctx, ctx->code+ofs, next-ofs);
  }

  fprintf(ctx->fp, "\n");
  switch (ctx->mode) {
  case BUILD_elfasm:
    /* fallthrough */
  case BUILD_coffasm:
    fprintf(ctx->fp, "\t.ident \"%s\"\n", ctx->dasm_ident);
    break;
  case BUILD_machasm:
    fprintf(ctx->fp,
      "\t.cstring\n"
      "\t.ascii \"%s\\0\"\n", ctx->dasm_ident);
    break;
  default:
    break;
  }
  fprintf(ctx->fp, "\n");
}

