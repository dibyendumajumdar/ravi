/*
** LuaJIT VM builder: PE object emitter.
** Modified for Ravi - no longer compatible with LuaJIT.
** Copyright (C) 2005-2017 Mike Pall. See Copyright Notice in luajit.h
**
** Only used for building on Windows, since we cannot assume the presence
** of a suitable assembler. The host and target byte order must match.
*/

#include "buildvm.h"
#include "lopcodes.h"

#if RAVI_TARGET_X86ORX64

/* IMPORTANT: Note that the code here depends upon Little Endian layout - i.e. 
   it assumes the compiler is generating Little Endian code. */

/* Context for PE object emitter. */
static char *strtab;
static size_t strtabofs;

/* -- PE object definitions ----------------------------------------------- */

/* PE header. */
typedef struct PEheader {
  uint16_t Machine; /* The number that identifies the type of target machine */
  uint16_t NumberOfSections; /* The number of sections. This indicates the size
                                of the section table, which immediately follows
                                the headers. */
  uint32_t TimeDateStamp;    /* The low 32 bits of the number of seconds since
                                00:00 January 1, 1970    (a C run-time time_t value),
                                that indicates when the file was created. */
  uint32_t PointerToSymbolTable; /* The file offset of the COFF symbol table, or
                                    zero if no COFF symbol table is present.
                                    This value should be zero for an image
                                    because COFF debugging information is
                                    deprecated. */
  uint32_t NumberOfSymbols; /* The number of entries in the symbol table. This
                               data can be used to locate the string table,
                               which immediately follows the symbol table. This
                               value should be zero for an image because COFF
                               debugging information is deprecated. */
  uint16_t
      SizeOfOptionalHeader; /* The size of the optional header, which is
                               required for executable files but not for object
                               files. This value should be zero for an object
                               file. */
  uint16_t
      Characteristics; /* The flags that indicate the attributes of the file */
} PEheader;

/* PE section. */
typedef struct PESectionHeader {
  char name[8]; /* An 8-byte, null-padded UTF-8 encoded string. If the string is
                   exactly 8 characters long, there is no terminating null. For
                   longer names, this field contains a slash (/) that is
                   followed by an ASCII representation of a decimal number that
                   is an offset into the string table. Executable images do not
                   use a string table and do not support section names longer
                   than 8 characters. Long names in object files are truncated
                   if they are emitted to an executable file. */
  uint32_t VirtualSize;    /* The total size of the section when loaded into
                              memory. If this value is greater    than SizeOfRawData,
                              the section is zero-padded. This field is valid only
                              for    executable images and should be set to zero for
                              object files. */
  uint32_t VirtualAddress; /* For executable images, the address of the first
                              byte of the section relative to the image base
                              when the section is loaded into memory. For object
                              files, this field is the address of the first byte
                              before relocation is applied; for simplicity,
                              compilers should set this to zero. Otherwise, it
                              is an arbitrary value that is subtracted from
                              offsets during relocation. */
  uint32_t SizeOfRawData;  /* The size of the section (for object files) or the
                              size of the initialized  data on disk (for image
                              files). For executable images, this must be a
                              multiple  of FileAlignment from the optional header.
                              If this is less than VirtualSize,  the remainder of
                              the section is zero-filled. Because the
                              SizeOfRawData field  is rounded but the VirtualSize
                              field is not, it is possible for SizeOfRawData to
                              be  greater than VirtualSize as well. When a section
                              contains only uninitialized data,  this field should
                              be zero. */
  uint32_t PointerToRawData;     /* The file pointer to the first page of the
                                    section within the COFF file. For     executable
                                    images, this must be a multiple of FileAlignment
                                    from the optional header.     For object files, the
                                    value should be aligned on a 4-byte boundary for
                                    best performance.     When a section contains only
                                    uninitialized data, this field should be zero.
                                  */
  uint32_t PointerToRelocations; /* The file pointer to the beginning of
                                    relocation entries for the section. This is
                                    set to zero for executable images or if
                                    there are no relocations. */
  uint32_t
      PointerToLinenumbers; /* The file pointer to the beginning of line-number
                               entries for the section. This is set to zero if
                               there are no COFF line numbers. This value should
                               be zero for an image because COFF debugging
                               information is deprecated. */
  uint16_t NumberOfRelocations; /* The number of relocation entries for the
                                   section. This is set to zero for executable
                                   images. */
  uint16_t NumberOfLinenumbers; /* The number of line-number entries for the
                                   section. This value should be zero for an
                                   image because COFF debugging information is
                                   deprecated. */
  uint32_t Characteristics;
} PESectionHeader;

/* PE relocation. */
typedef struct PEreloc {
  uint32_t VirtualAddress; /* The address of the item to which relocation is
  applied. This is the offset from the beginning of the section, plus the value
  of the section's RVA/Offset field. See Section Table (Section Headers). For
  example, if the first byte of the section has an address of 0x10, the third
  byte has an address of 0x12.*/

  uint32_t SymbolTableIndex; /* A zero-based index into the symbol table. This
  symbol gives the address that is to be used for the relocation. If the
  specified symbol has section storage class, then the symbol's address is the
  address with the first section of the same name.*/

  uint16_t type;
} PEreloc;
/* Cannot use sizeof, because it pads up to the max. alignment. */
#define PEOBJ_RELOC_SIZE (sizeof(uint32_t) * 2 + sizeof(uint16_t))

/* PE symbol table entry. */
typedef struct PEsym {
  union {
    char ShortName[8]; /* An array of 8 bytes. This array is padded with nulls
                          on the right if the name is less than 8 bytes long. */
    uint32_t nameref[2];
  } n;
  uint32_t Value;        /* The value that is associated with the symbol. The
                            interpretation of this field depends on SectionNumber and
                            StorageClass. A typical meaning is the relocatable address.
                          */
  int16_t SectionNumber; /* The signed integer that identifies the section,
                            using a one-based index into the section table. Some
                            values have special meaning */
  uint16_t Type; /* A number that represents type. Microsoft tools set this
                    field to 0x20 (function) or 0x0 (not a function). For more
                    information */
  uint8_t StorageClass; /* An enumerated value that represents storage class. */
  uint8_t NumberOfAuxSymbols; /* The number of auxiliary symbol table entries
                                 that follow this record. */
} PEsym;
/* sizeof(PEsym) must be 18 */
#define PEOBJ_SYM_SIZE 18  // must be 18 bytes
/*
Zero or more auxiliary symbol-table records immediately follow each standard
symbol-table record. However, typically not more than one auxiliary symbol-table
record follows a standard symbol-table record (except for .file records with
long file names). Each auxiliary record is the same size as a standard
symbol-table record (18 bytes), but rather than define a new symbol, the
auxiliary record gives additional information on the last symbol defined. The
choice of which of several formats to use depends on the StorageClass field.
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

static inline int start_symbol_index(int NumberOfRelocatableSymbols) {
  // Following is zero based offset of the symbol
  // The symbol table is arranged thus (see emit_peobj()):
  // @feat.00 - 1 symbol
  // .text section header - 2 symbols
  // NumberOfRelocatableSymbols Relocatable symbols
  // .pdata section header - 2 symbols
  // .xdata section header - 2 symbols
  // Which gives following formula to get to ravi_vm_asm_begin (StartSymbol)
  return 1 + 2 + NumberOfRelocatableSymbols + 2 + 2;  // Points to StartSymbol
}

static inline int xdata_symbol_index(int NumberOfRelocatableSymbols) {
  // Following is zero based offset of the symbol
  // The symbol table is arranged thus (see emit_peobj()):
  // @feat.00 - 1 symbol
  // .text section header - 2 symbols
  // NumberOfRelocatableSymbols Relocatable symbols
  // .pdata section header - 2 symbols
  // .xdata section header
  return 1 + 2 + NumberOfRelocatableSymbols + 2;  // Points to .xdata
}

/* Emit Windows PE object file. */
void emit_peobj(BuildCtx *ctx)
{
  PEheader pehdr;
  PESectionHeader sectionHeaders[PEOBJ_NSECTIONS];
  uint32_t offset;
  int i, NumberOfRelocatableSymbols;
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
  /* The .text section will contain all the generated machine code. Note that this contains
     exported symbols as well as the bytecode assembly routines; any external functions
     referenced will count as requiring relocations */
  memcpy(sectionHeaders[PEOBJ_SECT_TEXT].name, ".text", sizeof(".text")-1);
  sectionHeaders[PEOBJ_SECT_TEXT].PointerToRawData = offset;
  offset += (sectionHeaders[PEOBJ_SECT_TEXT].SizeOfRawData = (uint32_t)ctx->CodeSize);
  sectionHeaders[PEOBJ_SECT_TEXT].PointerToRelocations = offset;
  offset += (sectionHeaders[PEOBJ_SECT_TEXT].NumberOfRelocations = (uint16_t)ctx->RelocSize) * PEOBJ_RELOC_SIZE; 
  /* Flags: 60 = read+execute, 50 = align16, 20 = code. */
  sectionHeaders[PEOBJ_SECT_TEXT].Characteristics = IMAGE_SCN_CNT_CODE | IMAGE_SCN_ALIGN_16BYTES | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_EXECUTE;

  /* The .pdata section contains a function table. Right now we only generate a single entry in the
     function table, pointing to our exported luaV_interp() function which is the VM entrypoint and
     meant to be a direct replacement of luaV_execute() */
  memcpy(sectionHeaders[PEOBJ_SECT_PDATA].name, ".pdata", sizeof(".pdata")-1);
  sectionHeaders[PEOBJ_SECT_PDATA].PointerToRawData = offset;
  offset += (sectionHeaders[PEOBJ_SECT_PDATA].SizeOfRawData = 1*12);	// We need one function table entry of 12 bytes (.pdata function table entry is made up of 3 * 4 byte fields)
  sectionHeaders[PEOBJ_SECT_PDATA].PointerToRelocations = offset;
  offset += (sectionHeaders[PEOBJ_SECT_PDATA].NumberOfRelocations = 3) * PEOBJ_RELOC_SIZE;  // The 3 relocations are for the 3 fields in the .pdata function table entry above
  /* Flags: 40 = read, 30 = align4, 40 = initialized data. */
  sectionHeaders[PEOBJ_SECT_PDATA].Characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_ALIGN_4BYTES | IMAGE_SCN_MEM_READ;

  /* The .xdata section contains the UNWIND_INFO structure for the exported function luaV_interp().
     As the bytecode assembly routines are never invoked via 'call' I believe they are not treated as 
     C functions, hence inside the VM, the only function that needs to have unwind information defined
     is the luaV_interp() function. (This is my current understanding) */
  memcpy(sectionHeaders[PEOBJ_SECT_XDATA].name, ".xdata", sizeof(".xdata")-1);
  sectionHeaders[PEOBJ_SECT_XDATA].PointerToRawData = offset;
  offset += (sectionHeaders[PEOBJ_SECT_XDATA].SizeOfRawData = 24);  /* This is the size of the UNWIND_INFO we write below for our generated code */
  sectionHeaders[PEOBJ_SECT_XDATA].PointerToRelocations = offset;
  offset += (sectionHeaders[PEOBJ_SECT_XDATA].NumberOfRelocations = 0) * PEOBJ_RELOC_SIZE; // No relocations
  /* Flags: 40 = read, 30 = align4, 40 = initialized data. */
  sectionHeaders[PEOBJ_SECT_XDATA].Characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_ALIGN_4BYTES | IMAGE_SCN_MEM_READ;

  /* Don't think we really need this ... it just records dynasm version */
  memcpy(sectionHeaders[PEOBJ_SECT_RDATA_Z].name, ".rdata$Z", sizeof(".rdata$Z")-1);
  sectionHeaders[PEOBJ_SECT_RDATA_Z].PointerToRawData = offset;
  offset += (sectionHeaders[PEOBJ_SECT_RDATA_Z].SizeOfRawData = (uint32_t)strlen(ctx->dasm_ident)+1);
  /* Flags: 40 = read, 30 = align4, 40 = initialized data. */
  sectionHeaders[PEOBJ_SECT_RDATA_Z].Characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_ALIGN_4BYTES | IMAGE_SCN_MEM_READ;

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
  NumberOfRelocatableSymbols = ctx->nrelocsym;
  pehdr.NumberOfSymbols = 1 + PEOBJ_NSECTIONS*2 + 1 + ctx->NumberOfSymbols + NumberOfRelocatableSymbols; // We count two symbols for each section header, 1 for @feat.00 and 1 for asm_start

  /* Write PE object header and all sections. */
  owrite(ctx, &pehdr, sizeof(PEheader));
  owrite(ctx, &sectionHeaders, sizeof(PESectionHeader)*PEOBJ_NSECTIONS);

  /* Write .text section. */
  host_endian.u = 1;
  if (host_endian.b != RAVI_ENDIAN_SELECT(1, 0)) {
    fprintf(stderr, "Error: different byte order for host and target\n");
    exit(1);
  }
  /* emit the machine code */
  owrite(ctx, ctx->code, ctx->CodeSize);
  /* emit all the symbols */
  for (i = 0; i < ctx->RelocSize; i++) {
    PEreloc reloc;
    reloc.VirtualAddress = (uint32_t)ctx->Reloc[i].RelativeOffset + PEOBJ_RELOC_OFS;
    reloc.SymbolTableIndex = 1+2+ctx->Reloc[i].sym;  /* The symbolss are after @feat.00 (1) and .text sectionheader symbols (2) in the symbol table, hence we add 3 to the symbol's offset */
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
    uint32_t pdata[3];  /* Start of .text, end of .text and .xdata. */
    pdata[0] = 0; 
    pdata[1] = (uint32_t)ctx->CodeSize; 
    pdata[2] = 0; // Offset into .xdata I think (TBC)
    owrite(ctx, &pdata, sizeof(pdata));

    /*
    Object files contain COFF relocations, which specify how the section data should be modified when placed in the image file and subsequently loaded into memory.
    */
    PEreloc reloc;
    /* Entry for pdata[0] */
    reloc.VirtualAddress = 0;
    reloc.SymbolTableIndex = start_symbol_index(NumberOfRelocatableSymbols); // Points to StartSymbol
    reloc.type = PEOBJ_RELOC_ADDR32NB;
    owrite(ctx, &reloc, PEOBJ_RELOC_SIZE);
    /* Entry for pdata[1] */
    reloc.VirtualAddress = 4; 
    reloc.SymbolTableIndex = start_symbol_index(NumberOfRelocatableSymbols); // Points to StartSymbol
    reloc.type = PEOBJ_RELOC_ADDR32NB;
    owrite(ctx, &reloc, PEOBJ_RELOC_SIZE);
    /* Entry for pdata[2] */
    reloc.VirtualAddress = 8; 
    reloc.SymbolTableIndex = xdata_symbol_index(NumberOfRelocatableSymbols); // Points to .xdata symbol
    reloc.type = PEOBJ_RELOC_ADDR32NB;
    owrite(ctx, &reloc, PEOBJ_RELOC_SIZE);
  }
  { /* Write .xdata section - this is the UNWIND_INFO data for luaV_interp() function */
    // We want to record following UNWIND_CODEes (see structure in #if block below) in reverse order
    // push rbp
    // push rdi; push rsi; push rbx
    // push r12; push r13; push r14; push r15; (see vm_x64.dasc)
#if 0
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
			  uint8_t OpInfo : 4;
		  };
		  uint16_t FrameOffset;
	  } UNWIND_CODE, *PUNWIND_CODE;

#define UNW_FLAG_EHANDLER  0x01  
#define UNW_FLAG_UHANDLER  0x02  
#define UNW_FLAG_CHAININFO 0x04  

	  typedef struct _UNWIND_INFO {
		  uint8_t Version : 3;
		  uint8_t Flags : 5;
		  uint8_t SizeOfProlog;
		  uint8_t CountOfCodes;
		  uint8_t FrameRegister : 4;
		  uint8_t FrameOffset : 4;
		  UNWIND_CODE UnwindCode[1];
		  /*  UNWIND_CODE MoreUnwindCode[((CountOfCodes + 1) & ~1) - 1];
		  *   union {
		  *       OPTIONAL ULONG ExceptionHandler;
		  *       OPTIONAL ULONG FunctionEntry;
		  *   };
		  *   OPTIONAL ULONG ExceptionData[]; */
	  } UNWIND_INFO, *PUNWIND_INFO;
#endif
    /* Note that the registers and order below are dependent upon the order in which the
       the registers were pushed in the prologue of luaV_interp() and are in the reverse order
       here. This tells the OS that in case of exceptions, how to restore the registers when
       unwinding the luaV_interp() function */
    uint16_t xdata[12];
    xdata[0] = 0x0001;  /* Ver. 1, no handler, prolog size 0. */
    xdata[1] = 0x0009;  /* Number of unwind codes, no frame pointer. */
    xdata[2] = 0x4200;  /* Stack offset 4*8+8 = aword*5. */
    xdata[3] = 0xF000;  /* push r15  */
    xdata[4] = 0xE000;  /* push r14  */
    xdata[5] = 0xD000;  /* push r13  */
    xdata[6] = 0xC000;  /* push r12  */
    xdata[7] = 0x3000;  /* Push rbx. */
    xdata[8] = 0x6000;  /* Push rsi. */
    xdata[9] = 0x7000;  /* Push rdi. */
    xdata[10] = 0x5000;  /* Push rbp. */
    xdata[11] = 0;  /* Alignment - is this needed? TBC */
    owrite(ctx, &xdata, sizeof(xdata));
  }

  /* Write .rdata$Z section. */
  owrite(ctx, ctx->dasm_ident, strlen(ctx->dasm_ident)+1);

  /* Write symbol table. */
  /* This is done in two passes - the first pass collects string sizes, and second pass outputs the values */
  /* Note that strtab being NULL triggers first pass behaviour (this is bad code but okay because we don't care here) */
  strtab = NULL;  /* 1st pass: collect string sizes. */
  for (;;) {
    strtabofs = 4;
    /* Note the order in which the symbols are emitted - this order is important for the
       various indices that are computed */
    /* Mark as SafeSEH compliant. */
    emit_peobj_sym(ctx, "@feat.00", 1,
		   PEOBJ_SECT_ABS, IMAGE_SYM_TYPE_NULL, PEOBJ_SCL_STATIC);

    emit_peobj_sym_sect(ctx, sectionHeaders, PEOBJ_SECT_TEXT);
    for (i = 0; i < NumberOfRelocatableSymbols; i++)
      emit_peobj_sym(ctx, ctx->RelocatableSymbolNames[i], 0,
		     PEOBJ_SECT_UNDEF, IMAGE_SYM_TYPE_FUNC, IMAGE_SYM_CLASS_EXTERNAL);

    emit_peobj_sym_sect(ctx, sectionHeaders, PEOBJ_SECT_PDATA);
    emit_peobj_sym_sect(ctx, sectionHeaders, PEOBJ_SECT_XDATA);

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
