/*
** LuaJIT VM builder: PE object emitter.
** Copyright (C) 2005-2017 Mike Pall. See Copyright Notice in luajit.h
**
** Only used for building on Windows, since we cannot assume the presence
** of a suitable assembler. The host and target byte order must match.
*/

#include "buildvm.h"
#include "lopcodes.h"

#if RAVI_TARGET_X86ORX64 || RAVI_TARGET_PPC

/* Context for PE object emitter. */
static char *strtab;
static size_t strtabofs;

/* -- PE object definitions ----------------------------------------------- */

/* PE header. */
typedef struct PEheader {
  uint16_t Machine; /* The number that identifies the type of target machine */
  uint16_t NumberOfSections; /* The number of sections. This indicates the size of the section table, 
                                which immediately follows the headers. */
  uint32_t TimeDateStamp; /* The low 32 bits of the number of seconds since 00:00 January 1, 1970 
                             (a C run-time time_t value), that indicates when the file was created. */
  uint32_t PointerToSymbolTable; /* The file offset of the COFF symbol table, or zero if no COFF symbol 
                                    table is present. This value should be zero for an image because COFF 
                                    debugging information is deprecated. */
  uint32_t NumberOfSymbols; /* The number of entries in the symbol table. This data can be used to 
                               locate the string table, which immediately follows the symbol table. 
                               This value should be zero for an image because COFF debugging information is deprecated. */
  uint16_t SizeOfOptionalHeader; /* The size of the optional header, which is required for executable files 
                                    but not for object files. This value should be zero for an object file. */
  uint16_t Characteristics; /* The flags that indicate the attributes of the file */
} PEheader;

/* PE section. */
typedef struct PEsection {  
  char name[8]; /* An 8-byte, null-padded UTF-8 encoded string. If the string is exactly 8 characters long, there is no terminating null. For longer names, this field contains a slash (/) that is followed by an ASCII representation of a decimal number that is an offset into the string table. Executable images do not use a string table and do not support section names longer than 8 characters. Long names in object files are truncated if they are emitted to an executable file. */
  uint32_t VirtualSize; /* The total size of the section when loaded into memory. If this value is greater than SizeOfRawData, the section is zero-padded. This field is valid only for executable images and should be set to zero for object files. */
  uint32_t VirtualAddress; /* For executable images, the address of the first byte of the section relative to the image base when the section is loaded into memory. For object files, this field is the address of the first byte before relocation is applied; for simplicity, compilers should set this to zero. Otherwise, it is an arbitrary value that is subtracted from offsets during relocation. */
  uint32_t SizeOfRawData; /* The size of the section (for object files) or the size of the initialized data on disk (for image files). For executable images, this must be a multiple of FileAlignment from the optional header. If this is less than VirtualSize, the remainder of the section is zero-filled. Because the SizeOfRawData field is rounded but the VirtualSize field is not, it is possible for SizeOfRawData to be greater than VirtualSize as well. When a section contains only uninitialized data, this field should be zero. */
  uint32_t PointerToRawData; /* The file pointer to the first page of the section within the COFF file. For executable images, this must be a multiple of FileAlignment from the optional header. For object files, the value should be aligned on a 4-byte boundary for best performance. When a section contains only uninitialized data, this field should be zero. */
  uint32_t PointerToRelocations; /* The file pointer to the beginning of relocation entries for the section. This is set to zero for executable images or if there are no relocations. */
  uint32_t PointerToLinenumbers; /* The file pointer to the beginning of line-number entries for the section. This is set to zero if there are no COFF line numbers. This value should be zero for an image because COFF debugging information is deprecated. */
  uint16_t NumberOfRelocations;
  uint16_t nline;
  uint32_t flags;
} PEsection;

/* PE relocation. */
typedef struct PEreloc {
  uint32_t vaddr;
  uint32_t symidx;
  uint16_t type;
} PEreloc;

/* Cannot use sizeof, because it pads up to the max. alignment. */
#define PEOBJ_RELOC_SIZE	(4+4+2)

/* PE symbol table entry. */
typedef struct PEsym {
  union {
    char name[8];
    uint32_t nameref[2];
  } n;
  uint32_t value;
  int16_t sect;
  uint16_t type;
  uint8_t scl;
  uint8_t naux;
} PEsym;

/* PE symbol table auxiliary entry for a section. */
typedef struct PEsymaux {
  uint32_t size;
  uint16_t nreloc;
  uint16_t nline;
  uint32_t cksum;
  uint16_t assoc;
  uint8_t comdatsel;
  uint8_t unused[3];
} PEsymaux;

/* Cannot use sizeof, because it pads up to the max. alignment. */
#define PEOBJ_SYM_SIZE	(8+4+2+2+1+1)

/* PE object CPU specific defines. */
#if RAVI_TARGET_X86
#define PEOBJ_ARCH_TARGET	0x014c
#define PEOBJ_RELOC_REL32	0x14  /* MS: REL32, GNU: DISP32. */
#define PEOBJ_RELOC_DIR32	0x06
#define PEOBJ_RELOC_OFS		0
#define PEOBJ_TEXT_FLAGS	0x60500020  /* 60=r+x, 50=align16, 20=code. */
#elif RAVI_TARGET_X64
#define PEOBJ_ARCH_TARGET	0x8664  /* Image file AMD64 */
#define PEOBJ_RELOC_REL32	0x04  /* MS: REL32, GNU: DISP32. */
#define PEOBJ_RELOC_DIR32	0x02
#define PEOBJ_RELOC_ADDR32NB	0x03
#define PEOBJ_RELOC_OFS		0
#define PEOBJ_TEXT_FLAGS	0x60500020  /* 60=r+x, 50=align16, 20=code. */
#elif RAVI_TARGET_PPC
#define PEOBJ_ARCH_TARGET	0x01f2
#define PEOBJ_RELOC_REL32	0x06
#define PEOBJ_RELOC_DIR32	0x02
#define PEOBJ_RELOC_OFS		(-4)
#define PEOBJ_TEXT_FLAGS	0x60400020  /* 60=r+x, 40=align8, 20=code. */
#endif

/* Section numbers (0-based). */
enum {
  PEOBJ_SECT_ABS = -2,
  PEOBJ_SECT_UNDEF = -1,
  PEOBJ_SECT_TEXT,
#if RAVI_TARGET_X64
  PEOBJ_SECT_PDATA,
  PEOBJ_SECT_XDATA,
#elif RAVI_TARGET_X86
  PEOBJ_SECT_SXDATA,
#endif
  PEOBJ_SECT_RDATA_Z,
  PEOBJ_NSECTIONS
};

/* Symbol types. */
#define PEOBJ_TYPE_NULL		0
#define PEOBJ_TYPE_FUNC		0x20

/* Symbol storage class. */
#define PEOBJ_SCL_EXTERN	2
#define PEOBJ_SCL_STATIC	3

/* -- PE object emitter --------------------------------------------------- */

/* Emit PE object symbol. */
static void emit_peobj_sym(BuildCtx *ctx, const char *name, uint32_t value,
			   int sect, int type, int scl)
{
  PEsym sym;
  size_t len = strlen(name);
  if (!strtab) {  /* Pass 1: only calculate string table length. */
    if (len > 8) strtabofs += len+1;
    return;
  }
  if (len <= 8) {
    memcpy(sym.n.name, name, len);
    memset(sym.n.name+len, 0, 8-len);
  } else {
    sym.n.nameref[0] = 0;
    sym.n.nameref[1] = (uint32_t)strtabofs;
    memcpy(strtab + strtabofs, name, len);
    strtab[strtabofs+len] = 0;
    strtabofs += len+1;
  }
  sym.value = value;
  sym.sect = (int16_t)(sect+1);  /* 1-based section number. */
  sym.type = (uint16_t)type;
  sym.scl = (uint8_t)scl;
  sym.naux = 0;
  owrite(ctx, &sym, PEOBJ_SYM_SIZE);
}

/* Emit PE object section symbol. */
static void emit_peobj_sym_sect(BuildCtx *ctx, PEsection *pesect, int sect)
{
  PEsym sym;
  PEsymaux aux;
  if (!strtab) return;  /* Pass 1: no output. */
  memcpy(sym.n.name, pesect[sect].name, 8);
  sym.value = 0;
  sym.sect = (int16_t)(sect+1);  /* 1-based section number. */
  sym.type = PEOBJ_TYPE_NULL;
  sym.scl = PEOBJ_SCL_STATIC;
  sym.naux = 1;
  owrite(ctx, &sym, PEOBJ_SYM_SIZE);
  memset(&aux, 0, sizeof(PEsymaux));
  aux.size = pesect[sect].SizeOfRawData;
  aux.nreloc = pesect[sect].NumberOfRelocations;
  owrite(ctx, &aux, PEOBJ_SYM_SIZE);
}

/* Emit Windows PE object file. */
void emit_peobj(BuildCtx *ctx)
{
  PEheader pehdr;
  PEsection pesect[PEOBJ_NSECTIONS];
  uint32_t sofs;
  int i, nrsym;
  union { uint8_t b; uint32_t u; } host_endian;

  sofs = sizeof(PEheader) + PEOBJ_NSECTIONS*sizeof(PEsection);

  /* Fill in PE sections. */
  memset(&pesect, 0, PEOBJ_NSECTIONS*sizeof(PEsection));
  memcpy(pesect[PEOBJ_SECT_TEXT].name, ".text", sizeof(".text")-1);
  pesect[PEOBJ_SECT_TEXT].PointerToRawData = sofs;
  sofs += (pesect[PEOBJ_SECT_TEXT].SizeOfRawData = (uint32_t)ctx->codesz);
  pesect[PEOBJ_SECT_TEXT].PointerToRelocations = sofs;
  sofs += (pesect[PEOBJ_SECT_TEXT].NumberOfRelocations = (uint16_t)ctx->nreloc) * PEOBJ_RELOC_SIZE;
  /* Flags: 60 = read+execute, 50 = align16, 20 = code. */
  pesect[PEOBJ_SECT_TEXT].flags = PEOBJ_TEXT_FLAGS;

#if RAVI_TARGET_X64
  memcpy(pesect[PEOBJ_SECT_PDATA].name, ".pdata", sizeof(".pdata")-1);
  pesect[PEOBJ_SECT_PDATA].PointerToRawData = sofs;
  sofs += (pesect[PEOBJ_SECT_PDATA].SizeOfRawData = 6*4);
  pesect[PEOBJ_SECT_PDATA].PointerToRelocations = sofs;
  sofs += (pesect[PEOBJ_SECT_PDATA].NumberOfRelocations = 6) * PEOBJ_RELOC_SIZE;
  /* Flags: 40 = read, 30 = align4, 40 = initialized data. */
  pesect[PEOBJ_SECT_PDATA].flags = 0x40300040;

  memcpy(pesect[PEOBJ_SECT_XDATA].name, ".xdata", sizeof(".xdata")-1);
  pesect[PEOBJ_SECT_XDATA].PointerToRawData = sofs;
  sofs += (pesect[PEOBJ_SECT_XDATA].SizeOfRawData = 8*2+4+6*2);  /* See below. */
  pesect[PEOBJ_SECT_XDATA].PointerToRelocations = sofs;
  sofs += (pesect[PEOBJ_SECT_XDATA].NumberOfRelocations = 1) * PEOBJ_RELOC_SIZE;
  /* Flags: 40 = read, 30 = align4, 40 = initialized data. */
  pesect[PEOBJ_SECT_XDATA].flags = 0x40300040;
#elif RAVI_TARGET_X86
  memcpy(pesect[PEOBJ_SECT_SXDATA].name, ".sxdata", sizeof(".sxdata")-1);
  pesect[PEOBJ_SECT_SXDATA].PointerToRawData = sofs;
  sofs += (pesect[PEOBJ_SECT_SXDATA].SizeOfRawData = 4);
  pesect[PEOBJ_SECT_SXDATA].PointerToRelocations = sofs;
  /* Flags: 40 = read, 30 = align4, 02 = lnk_info, 40 = initialized data. */
  pesect[PEOBJ_SECT_SXDATA].flags = 0x40300240;
#endif

  memcpy(pesect[PEOBJ_SECT_RDATA_Z].name, ".rdata$Z", sizeof(".rdata$Z")-1);
  pesect[PEOBJ_SECT_RDATA_Z].PointerToRawData = sofs;
  sofs += (pesect[PEOBJ_SECT_RDATA_Z].SizeOfRawData = (uint32_t)strlen(ctx->dasm_ident)+1);
  /* Flags: 40 = read, 30 = align4, 40 = initialized data. */
  pesect[PEOBJ_SECT_RDATA_Z].flags = 0x40300040;

  /* Fill in PE header. */
  pehdr.Machine = PEOBJ_ARCH_TARGET;
  pehdr.NumberOfSections = PEOBJ_NSECTIONS;
  pehdr.TimeDateStamp = 0;  /* Timestamp is optional. */
  pehdr.PointerToSymbolTable = sofs;
  pehdr.SizeOfOptionalHeader = 0;
  pehdr.Characteristics = 0;

  /* Compute the size of the symbol table:
  ** @feat.00 + nsections*2
  ** + asm_start + nsym
  ** + nrsym
  */
  nrsym = ctx->nrelocsym;
  pehdr.NumberOfSymbols = 1+PEOBJ_NSECTIONS*2 + 1+ctx->nsym + nrsym;
#if RAVI_TARGET_X64
  pehdr.NumberOfSymbols += 1;  /* Symbol for lj_err_unwind_win. */
#endif

  /* Write PE object header and all sections. */
  owrite(ctx, &pehdr, sizeof(PEheader));
  owrite(ctx, &pesect, sizeof(PEsection)*PEOBJ_NSECTIONS);

  /* Write .text section. */
  host_endian.u = 1;
  if (host_endian.b != RAVI_ENDIAN_SELECT(1, 0)) {
#if RAVI_TARGET_PPC
    uint32_t *p = (uint32_t *)ctx->code;
    int n = (int)(ctx->codesz >> 2);
    for (i = 0; i < n; i++, p++)
      *p = lj_bswap(*p);  /* Byteswap .text section. */
#else
    fprintf(stderr, "Error: different byte order for host and target\n");
    exit(1);
#endif
  }
  owrite(ctx, ctx->code, ctx->codesz);
  for (i = 0; i < ctx->nreloc; i++) {
    PEreloc reloc;
    reloc.vaddr = (uint32_t)ctx->reloc[i].ofs + PEOBJ_RELOC_OFS;
    reloc.symidx = 1+2+ctx->reloc[i].sym;  /* Reloc syms are after .text sym. */
    reloc.type = ctx->reloc[i].type ? PEOBJ_RELOC_REL32 : PEOBJ_RELOC_DIR32;
    owrite(ctx, &reloc, PEOBJ_RELOC_SIZE);
  }

#if RAVI_TARGET_X64
  { /* Write .pdata section. */
    uint32_t fcofs = (uint32_t)ctx->sym[ctx->nsym-1].ofs;
    uint32_t pdata[3];  /* Start of .text, end of .text and .xdata. */
    PEreloc reloc;
    pdata[0] = 0; pdata[1] = fcofs; pdata[2] = 0;
    owrite(ctx, &pdata, sizeof(pdata));
    pdata[0] = fcofs; pdata[1] = (uint32_t)ctx->codesz; pdata[2] = 20;
    owrite(ctx, &pdata, sizeof(pdata));
    reloc.vaddr = 0; reloc.symidx = 1+2+nrsym+2+2+1;
    reloc.type = PEOBJ_RELOC_ADDR32NB;
    owrite(ctx, &reloc, PEOBJ_RELOC_SIZE);
    reloc.vaddr = 4; reloc.symidx = 1+2+nrsym+2+2+1;
    reloc.type = PEOBJ_RELOC_ADDR32NB;
    owrite(ctx, &reloc, PEOBJ_RELOC_SIZE);
    reloc.vaddr = 8; reloc.symidx = 1+2+nrsym+2;
    reloc.type = PEOBJ_RELOC_ADDR32NB;
    owrite(ctx, &reloc, PEOBJ_RELOC_SIZE);
    reloc.vaddr = 12; reloc.symidx = 1+2+nrsym+2+2+1;
    reloc.type = PEOBJ_RELOC_ADDR32NB;
    owrite(ctx, &reloc, PEOBJ_RELOC_SIZE);
    reloc.vaddr = 16; reloc.symidx = 1+2+nrsym+2+2+1;
    reloc.type = PEOBJ_RELOC_ADDR32NB;
    owrite(ctx, &reloc, PEOBJ_RELOC_SIZE);
    reloc.vaddr = 20; reloc.symidx = 1+2+nrsym+2;
    reloc.type = PEOBJ_RELOC_ADDR32NB;
    owrite(ctx, &reloc, PEOBJ_RELOC_SIZE);
  }
  { /* Write .xdata section. */
    uint16_t xdata[8+2+6];
    PEreloc reloc;
    xdata[0] = 0x01|0x08|0x10;  /* Ver. 1, uhandler/ehandler, prolog size 0. */
    xdata[1] = 0x0005;  /* Number of unwind codes, no frame pointer. */
    xdata[2] = 0x4200;  /* Stack offset 4*8+8 = aword*5. */
    xdata[3] = 0x3000;  /* Push rbx. */
    xdata[4] = 0x6000;  /* Push rsi. */
    xdata[5] = 0x7000;  /* Push rdi. */
    xdata[6] = 0x5000;  /* Push rbp. */
    xdata[7] = 0;  /* Alignment. */
    xdata[8] = xdata[9] = 0;  /* Relocated address of exception handler. */
    xdata[10] = 0x01;  /* Ver. 1, no handler, prolog size 0. */
    xdata[11] = 0x1504;  /* Number of unwind codes, fp = rbp, fpofs = 16. */
    xdata[12] = 0x0300;  /* set_fpreg. */
    xdata[13] = 0x0200;  /* stack offset 0*8+8 = aword*1. */
    xdata[14] = 0x3000;  /* Push rbx. */
    xdata[15] = 0x5000;  /* Push rbp. */
    owrite(ctx, &xdata, sizeof(xdata));
    reloc.vaddr = 2*8; reloc.symidx = 1+2+nrsym+2+2;
    reloc.type = PEOBJ_RELOC_ADDR32NB;
    owrite(ctx, &reloc, PEOBJ_RELOC_SIZE);
  }
#elif RAVI_TARGET_X86
  /* Write .sxdata section. */
  for (i = 0; i < nrsym; i++) {
    if (!strcmp(ctx->relocsym[i], "_lj_err_unwind_win")) {
      uint32_t symidx = 1+2+i;
      owrite(ctx, &symidx, 4);
      break;
    }
  }
  if (i == nrsym) {
    fprintf(stderr, "Error: extern lj_err_unwind_win not used\n");
    exit(1);
  }
#endif

  /* Write .rdata$Z section. */
  owrite(ctx, ctx->dasm_ident, strlen(ctx->dasm_ident)+1);

  /* Write symbol table. */
  strtab = NULL;  /* 1st pass: collect string sizes. */
  for (;;) {
    strtabofs = 4;
    /* Mark as SafeSEH compliant. */
    emit_peobj_sym(ctx, "@feat.00", 1,
		   PEOBJ_SECT_ABS, PEOBJ_TYPE_NULL, PEOBJ_SCL_STATIC);

    emit_peobj_sym_sect(ctx, pesect, PEOBJ_SECT_TEXT);
    for (i = 0; i < nrsym; i++)
      emit_peobj_sym(ctx, ctx->relocsym[i], 0,
		     PEOBJ_SECT_UNDEF, PEOBJ_TYPE_FUNC, PEOBJ_SCL_EXTERN);

#if RAVI_TARGET_X64
    emit_peobj_sym_sect(ctx, pesect, PEOBJ_SECT_PDATA);
    emit_peobj_sym_sect(ctx, pesect, PEOBJ_SECT_XDATA);
    emit_peobj_sym(ctx, "lj_err_unwind_win", 0,
		   PEOBJ_SECT_UNDEF, PEOBJ_TYPE_FUNC, PEOBJ_SCL_EXTERN);
#elif RAVI_TARGET_X86
    emit_peobj_sym_sect(ctx, pesect, PEOBJ_SECT_SXDATA);
#endif

    emit_peobj_sym(ctx, ctx->beginsym, 0,
		   PEOBJ_SECT_TEXT, PEOBJ_TYPE_NULL, PEOBJ_SCL_EXTERN);
    for (i = 0; i < ctx->nsym; i++)
      emit_peobj_sym(ctx, ctx->sym[i].name, (uint32_t)ctx->sym[i].ofs,
		     PEOBJ_SECT_TEXT, PEOBJ_TYPE_FUNC, PEOBJ_SCL_EXTERN);

    emit_peobj_sym_sect(ctx, pesect, PEOBJ_SECT_RDATA_Z);

    if (strtab)
      break;
    /* 2nd pass: alloc strtab, write syms and copy strings. */
    strtab = (char *)malloc(strtabofs);
    *(uint32_t *)strtab = (uint32_t)strtabofs;
  }

  /* Write string table. */
  owrite(ctx, strtab, strtabofs);
}

#else

void emit_peobj(BuildCtx *ctx)
{
  UNUSED(ctx);
  fprintf(stderr, "Error: no PE object support for this target\n");
  exit(1);
}

#endif
