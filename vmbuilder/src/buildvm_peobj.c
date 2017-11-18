/*
** LuaJIT VM builder: PE object emitter.
** Copyright (C) 2005-2017 Mike Pall. See Copyright Notice in luajit.h
**
** Only used for building on Windows, since we cannot assume the presence
** of a suitable assembler. The host and target byte order must match.
*/

#include "buildvm.h"
#include "lopcodes.h"

#if RAVI_TARGET_X86ORX64

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
typedef struct PESectionHeader {  
  char name[8]; /* An 8-byte, null-padded UTF-8 encoded string. If the string is exactly 8 characters long, 
                   there is no terminating null. For longer names, this field contains a slash (/) that is 
                   followed by an ASCII representation of a decimal number that is an offset into the string table. 
                   Executable images do not use a string table and do not support section names longer than 
                   8 characters. Long names in object files are truncated if they are emitted to an executable file. */
  uint32_t VirtualSize; /* The total size of the section when loaded into memory. If this value is greater 
                           than SizeOfRawData, the section is zero-padded. This field is valid only for 
                           executable images and should be set to zero for object files. */
  uint32_t VirtualAddress; /* For executable images, the address of the first byte of the section relative 
                              to the image base when the section is loaded into memory. For object files, 
                              this field is the address of the first byte before relocation is applied; 
                              for simplicity, compilers should set this to zero. Otherwise, it is an arbitrary 
                              value that is subtracted from offsets during relocation. */
  uint32_t SizeOfRawData; /* The size of the section (for object files) or the size of the initialized 
                             data on disk (for image files). For executable images, this must be a multiple 
                             of FileAlignment from the optional header. If this is less than VirtualSize, 
                             the remainder of the section is zero-filled. Because the SizeOfRawData field 
                             is rounded but the VirtualSize field is not, it is possible for SizeOfRawData to be 
                             greater than VirtualSize as well. When a section contains only uninitialized data, 
                             this field should be zero. */
  uint32_t PointerToRawData; /* The file pointer to the first page of the section within the COFF file. For 
                                executable images, this must be a multiple of FileAlignment from the optional header. 
                                For object files, the value should be aligned on a 4-byte boundary for best performance. 
                                When a section contains only uninitialized data, this field should be zero. */
  uint32_t PointerToRelocations; /* The file pointer to the beginning of relocation entries for the section. 
                                    This is set to zero for executable images or if there are no relocations. */
  uint32_t PointerToLinenumbers; /* The file pointer to the beginning of line-number entries for the section. This is 
                                    set to zero if there are no COFF line numbers. This value should be zero for an 
                                    image because COFF debugging information is deprecated. */
  uint16_t NumberOfRelocations; /* The number of relocation entries for the section. This is set to zero for executable images. */
  uint16_t NumberOfLinenumbers; /* The number of line-number entries for the section. This value should be zero for an 
                                   image because COFF debugging information is deprecated. */
  uint32_t Characteristics;
} PESectionHeader;

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
    char ShortName[8]; /* An array of 8 bytes. This array is padded with nulls on the right if the name is less than 8 bytes long. */
    uint32_t nameref[2];
  } n;
  uint32_t Value; /* The value that is associated with the symbol. The interpretation of this field depends on SectionNumber and StorageClass. A typical meaning is the relocatable address. */
  int16_t SectionNumber; /* The signed integer that identifies the section, using a one-based index into the section table. Some values have special meaning */
  uint16_t Type; /* A number that represents type. Microsoft tools set this field to 0x20 (function) or 0x0 (not a function). For more information */
  uint8_t StorageClass; /* An enumerated value that represents storage class. */
  uint8_t NumberOfAuxSymbols; /* The number of auxiliary symbol table entries that follow this record. */
} PEsym;
/* sizeof(PEsym) must be 18 */
#define PEOBJ_SYM_SIZE  18  // must be 18 bytes
/*
Zero or more auxiliary symbol-table records immediately follow each standard symbol-table record. 
However, typically not more than one auxiliary symbol-table record follows a standard symbol-table record 
(except for .file records with long file names). Each auxiliary record is the same size as a standard symbol-table 
record (18 bytes), but rather than define a new symbol, the auxiliary record gives additional information on the
last symbol defined. The choice of which of several formats to use depends on the StorageClass field.
*/
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

/* PE object CPU specific defines. */
#define PEOBJ_ARCH_TARGET	0x8664  /* Image file AMD64 */
#define PEOBJ_RELOC_REL32	0x04  /* MS: REL32, GNU: DISP32. */
#define PEOBJ_RELOC_DIR32	0x02
#define PEOBJ_RELOC_ADDR32NB	0x03
#define PEOBJ_RELOC_OFS		0
#define PEOBJ_TEXT_FLAGS	0x60500020  /* 60=r+x, 50=align16, 20=code. */

/* Section numbers (0-based). */
enum {
  PEOBJ_SECT_ABS = -2,
  PEOBJ_SECT_UNDEF = -1,
  PEOBJ_SECT_TEXT,
  PEOBJ_SECT_PDATA,
  PEOBJ_SECT_XDATA,
  PEOBJ_SECT_RDATA_Z,
  PEOBJ_NSECTIONS
};

/* Symbol types. */
#define IMAGE_SYM_TYPE_NULL		0
#define IMAGE_SYM_TYPE_FUNC		0x20

/* Symbol storage class. */
#define IMAGE_SYM_CLASS_EXTERNAL	2
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
    memcpy(sym.n.ShortName, name, len);
    memset(sym.n.ShortName+len, 0, 8-len);
  } else {
    sym.n.nameref[0] = 0;
    sym.n.nameref[1] = (uint32_t)strtabofs;
    memcpy(strtab + strtabofs, name, len);
    strtab[strtabofs+len] = 0;
    strtabofs += len+1;
  }
  sym.Value = value;
  sym.SectionNumber = (int16_t)(sect+1);  /* 1-based section number. */
  sym.Type = (uint16_t)type;
  sym.StorageClass = (uint8_t)scl;
  sym.NumberOfAuxSymbols = 0;
  owrite(ctx, &sym, PEOBJ_SYM_SIZE);
}

/* Emit PE object section symbol. */
static void emit_peobj_sym_sect(BuildCtx *ctx, PESectionHeader *sectionHeader, int sect)
{
  PEsym sym;
  PEsymaux aux;
  if (!strtab) return;  /* Pass 1: no output. */
  memcpy(sym.n.ShortName, sectionHeader[sect].name, 8);
  sym.Value = 0;
  sym.SectionNumber = (int16_t)(sect+1);  /* 1-based section number. */
  sym.Type = IMAGE_SYM_TYPE_NULL;
  sym.StorageClass = PEOBJ_SCL_STATIC;
  sym.NumberOfAuxSymbols = 1;
  owrite(ctx, &sym, PEOBJ_SYM_SIZE);
  memset(&aux, 0, sizeof(PEsymaux));
  aux.size = sectionHeader[sect].SizeOfRawData;
  aux.nreloc = sectionHeader[sect].NumberOfRelocations;
  owrite(ctx, &aux, PEOBJ_SYM_SIZE);
}

typedef enum _UNWIND_OP_CODES {  
    UWOP_PUSH_NONVOL = 0, /* info == register number */  
    UWOP_ALLOC_LARGE,     /* no info, alloc size in next 2 slots */  
    UWOP_ALLOC_SMALL,     /* info == size of allocation / 8 - 1 */  
    UWOP_SET_FPREG,       /* no info, FP = RSP + UNWIND_INFO.FPRegOffset*16 */  
    UWOP_SAVE_NONVOL,     /* info == register number, offset in next slot */  
    UWOP_SAVE_NONVOL_FAR, /* info == register number, offset in next 2 slots */  
    UWOP_SAVE_XMM128,     /* info == XMM reg number, offset in next slot */  
    UWOP_SAVE_XMM128_FAR, /* info == XMM reg number, offset in next 2 slots */  
    UWOP_PUSH_MACHFRAME   /* info == 0: no error-code, 1: error-code */  
} UNWIND_CODE_OPS;  

typedef union _UNWIND_CODE {  
    struct {  
        uint8_t CodeOffset;  
        uint8_t UnwindOp : 4;  
        uint8_t OpInfo   : 4;  
    };  
    uint16_t FrameOffset;  
} UNWIND_CODE, *PUNWIND_CODE;  

#define UNW_FLAG_EHANDLER  0x01  
#define UNW_FLAG_UHANDLER  0x02  
#define UNW_FLAG_CHAININFO 0x04  

typedef struct _UNWIND_INFO {  
    uint8_t Version       : 3;  
    uint8_t Flags         : 5;  
    uint8_t SizeOfProlog;  
    uint8_t CountOfCodes;  
    uint8_t FrameRegister : 4;  
    uint8_t FrameOffset   : 4;  
    UNWIND_CODE UnwindCode[1];  
/*  UNWIND_CODE MoreUnwindCode[((CountOfCodes + 1) & ~1) - 1];  
*   union {  
*       OPTIONAL ULONG ExceptionHandler;  
*       OPTIONAL ULONG FunctionEntry;  
*   };  
*   OPTIONAL ULONG ExceptionData[]; */  
} UNWIND_INFO, *PUNWIND_INFO;



/* Emit Windows PE object file. */
void emit_peobj(BuildCtx *ctx)
{
  PEheader pehdr;
  PESectionHeader sectionHeaders[PEOBJ_NSECTIONS];
  uint32_t offset;
  int i, nrsym;
  union { uint8_t b; uint32_t u; } host_endian;

  enum {
    IMAGE_SCN_CNT_CODE = 0x00000020, /* The section contains executable code. */
    IMAGE_SCN_CNT_INITIALIZED_DATA = 0x00000040, /* The section contains initialized data. */
    IMAGE_SCN_LNK_INFO = 0x00000200, /* The section contains comments or other information. The .drectve section has this Type. This is valid for object files only. */
    IMAGE_SCN_ALIGN_4BYTES = 0x00300000, /* Align data on a 4-byte boundary. Valid only for object files. */
    IMAGE_SCN_ALIGN_16BYTES = 0x00500000, /* Align data on a 16-byte boundary. Valid only for object files. */
    IMAGE_SCN_MEM_READ = 0x40000000, /* The section can be read. */
    IMAGE_SCN_MEM_EXECUTE = 0x20000000, /* The section can be executed as code. */
  };

  memset(&sectionHeaders, 0, sizeof sectionHeaders);

  /* Set offset to end of the header and section headers */
  offset = sizeof(PEheader) + PEOBJ_NSECTIONS*sizeof(PESectionHeader);

  /* Fill in PE sections. */
  memcpy(sectionHeaders[PEOBJ_SECT_TEXT].name, ".text", sizeof(".text")-1);
  sectionHeaders[PEOBJ_SECT_TEXT].PointerToRawData = offset;
  offset += (sectionHeaders[PEOBJ_SECT_TEXT].SizeOfRawData = (uint32_t)ctx->CodeSize);
  sectionHeaders[PEOBJ_SECT_TEXT].PointerToRelocations = offset;
  offset += (sectionHeaders[PEOBJ_SECT_TEXT].NumberOfRelocations = (uint16_t)ctx->RelocSize) * PEOBJ_RELOC_SIZE;
  /* Flags: 60 = read+execute, 50 = align16, 20 = code. */
  sectionHeaders[PEOBJ_SECT_TEXT].Characteristics = IMAGE_SCN_CNT_CODE | IMAGE_SCN_ALIGN_16BYTES | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_EXECUTE; //   ;PEOBJ_TEXT_FLAGS;

  memcpy(sectionHeaders[PEOBJ_SECT_PDATA].name, ".pdata", sizeof(".pdata")-1);
  sectionHeaders[PEOBJ_SECT_PDATA].PointerToRawData = offset;
  offset += (sectionHeaders[PEOBJ_SECT_PDATA].SizeOfRawData = 6*4);	// FIXME what are these magic numbers
  sectionHeaders[PEOBJ_SECT_PDATA].PointerToRelocations = offset;
  offset += (sectionHeaders[PEOBJ_SECT_PDATA].NumberOfRelocations = 6) * PEOBJ_RELOC_SIZE; // FIXME why 6? 
  /* Flags: 40 = read, 30 = align4, 40 = initialized data. */
  sectionHeaders[PEOBJ_SECT_PDATA].Characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_ALIGN_4BYTES | IMAGE_SCN_MEM_READ; //                0x40300040;

  memcpy(sectionHeaders[PEOBJ_SECT_XDATA].name, ".xdata", sizeof(".xdata")-1);
  sectionHeaders[PEOBJ_SECT_XDATA].PointerToRawData = offset;
  offset += (sectionHeaders[PEOBJ_SECT_XDATA].SizeOfRawData = 8*2+4+6*2);  /* FIXME See below. */
  sectionHeaders[PEOBJ_SECT_XDATA].PointerToRelocations = offset;
  offset += (sectionHeaders[PEOBJ_SECT_XDATA].NumberOfRelocations = 1) * PEOBJ_RELOC_SIZE;
  /* Flags: 40 = read, 30 = align4, 40 = initialized data. */
  sectionHeaders[PEOBJ_SECT_XDATA].Characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_ALIGN_4BYTES | IMAGE_SCN_MEM_READ;  // 0x40300040;

  /* Don't this we really need this ... */
  memcpy(sectionHeaders[PEOBJ_SECT_RDATA_Z].name, ".rdata$Z", sizeof(".rdata$Z")-1);
  sectionHeaders[PEOBJ_SECT_RDATA_Z].PointerToRawData = offset;
  offset += (sectionHeaders[PEOBJ_SECT_RDATA_Z].SizeOfRawData = (uint32_t)strlen(ctx->dasm_ident)+1);
  /* Flags: 40 = read, 30 = align4, 40 = initialized data. */
  sectionHeaders[PEOBJ_SECT_RDATA_Z].Characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_ALIGN_4BYTES | IMAGE_SCN_MEM_READ;; // 0x40300040;

  /* Fill in PE header. */
  pehdr.Machine = PEOBJ_ARCH_TARGET;
  pehdr.NumberOfSections = PEOBJ_NSECTIONS;
  pehdr.TimeDateStamp = 0;  /* Timestamp is optional. */
  pehdr.PointerToSymbolTable = offset;
  pehdr.SizeOfOptionalHeader = 0;
  pehdr.Characteristics = 0;

  /* Compute the size of the symbol table:
  ** @feat.00 + nsections*2
  ** + asm_start + nsym
  ** + nrsym
  */
  nrsym = ctx->nrelocsym;
  pehdr.NumberOfSymbols = 1+PEOBJ_NSECTIONS*2 + 1 + ctx->NumberOfSymbols + nrsym;
  //pehdr.NumberOfSymbols += 1;  /* Symbol for lj_err_unwind_win. */

  /* Write PE object header and all sections. */
  owrite(ctx, &pehdr, sizeof(PEheader));
  owrite(ctx, &sectionHeaders, sizeof(PESectionHeader)*PEOBJ_NSECTIONS);

  /* Write .text section. */
  host_endian.u = 1;
  if (host_endian.b != RAVI_ENDIAN_SELECT(1, 0)) {
    fprintf(stderr, "Error: different byte order for host and target\n");
    exit(1);
  }
  owrite(ctx, ctx->code, ctx->CodeSize);
  for (i = 0; i < ctx->RelocSize; i++) {
    PEreloc reloc;
    reloc.vaddr = (uint32_t)ctx->Reloc[i].RelativeOffset + PEOBJ_RELOC_OFS;
    reloc.symidx = 1+2+ctx->Reloc[i].sym;  /* Reloc syms are after .text sym. */
    reloc.type = ctx->Reloc[i].type ? PEOBJ_RELOC_REL32 : PEOBJ_RELOC_DIR32;
    owrite(ctx, &reloc, PEOBJ_RELOC_SIZE);
  }

  { /* Write .pdata section. */
    /*
    The .pdata section contains an array of function table entries that are used for exception handling. 
    It is pointed to by the exception table entry in the image data directory. The entries must be sorted 
    according to the function addresses (the first field in each structure) before being emitted into the 
    final image. The target platform determines which of the three function table entry format variations 
    described below is used.
    For x64 and Itanium platforms, function table entries have the following format:
    Offset    Size     Field                      Description
    0         4        Begin Address              The RVA of the corresponding function
    4         4        End Address                The RVA of the end of the function.
    8         4        Unwind Information         The RVA of the unwind information.
    */
    uint32_t fcofs = (uint32_t)ctx->AllSymbols[ctx->NumberOfSymbols-1].ofs;
    uint32_t pdata[3];  /* Start of .text, end of .text and .xdata. */
    PEreloc reloc;
    pdata[0] = 0; pdata[1] = fcofs; pdata[2] = 0;
    owrite(ctx, &pdata, sizeof(pdata));
    pdata[0] = fcofs; pdata[1] = (uint32_t)ctx->CodeSize; pdata[2] = 20;
    owrite(ctx, &pdata, sizeof(pdata));

    /*
    Object files contain COFF relocations, which specify how the section data should be modified when placed in the image file and subsequently loaded into memory.
    */
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

  /* Write .rdata$Z section. */
  owrite(ctx, ctx->dasm_ident, strlen(ctx->dasm_ident)+1);

  /* Write symbol table. */
  strtab = NULL;  /* 1st pass: collect string sizes. */
  for (;;) {
    strtabofs = 4;
    /* Mark as SafeSEH compliant. */
    emit_peobj_sym(ctx, "@feat.00", 1,
		   PEOBJ_SECT_ABS, IMAGE_SYM_TYPE_NULL, PEOBJ_SCL_STATIC);

    emit_peobj_sym_sect(ctx, sectionHeaders, PEOBJ_SECT_TEXT);
    for (i = 0; i < nrsym; i++)
      emit_peobj_sym(ctx, ctx->RelocatableSymbolNames[i], 0,
		     PEOBJ_SECT_UNDEF, IMAGE_SYM_TYPE_FUNC, IMAGE_SYM_CLASS_EXTERNAL);

    emit_peobj_sym_sect(ctx, sectionHeaders, PEOBJ_SECT_PDATA);
    emit_peobj_sym_sect(ctx, sectionHeaders, PEOBJ_SECT_XDATA);
    emit_peobj_sym(ctx, "lj_err_unwind_win", 0,
		   PEOBJ_SECT_UNDEF, IMAGE_SYM_TYPE_FUNC, IMAGE_SYM_CLASS_EXTERNAL);

    emit_peobj_sym(ctx, ctx->StartSymbol, 0,
		   PEOBJ_SECT_TEXT, IMAGE_SYM_TYPE_NULL, IMAGE_SYM_CLASS_EXTERNAL);
    for (i = 0; i < ctx->NumberOfSymbols; i++)
      emit_peobj_sym(ctx, ctx->AllSymbols[i].name, (uint32_t)ctx->AllSymbols[i].ofs,
		     PEOBJ_SECT_TEXT, IMAGE_SYM_TYPE_FUNC, IMAGE_SYM_CLASS_EXTERNAL);

    emit_peobj_sym_sect(ctx, sectionHeaders, PEOBJ_SECT_RDATA_Z);

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
