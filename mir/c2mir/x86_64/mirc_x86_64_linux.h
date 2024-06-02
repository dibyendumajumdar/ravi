/* This file is a part of MIR project.
   Copyright (C) 2019-2024 Vladimir Makarov <vmakarov.gcc@gmail.com>.
*/

static char x86_64_mirc[]
  = "#define __amd64 1\n"
    "#define __amd64__ 1\n"
    "#define _LP64 1\n"
    "#define __LP64__ 1\n"
    "#define __x86_64 1\n"
    "#define __x86_64__ 1\n"
    "\n"
    "#define __SIZEOF_DOUBLE__ 8\n"
    "#define __SIZEOF_FLOAT__ 4\n"
    "#define __SIZEOF_INT__ 4\n"
    "#define __SIZEOF_LONG_DOUBLE__ 16\n"
    "#define __SIZEOF_LONG_LONG__ 8\n"
    "#define __SIZEOF_LONG__ 8\n"
    "#define __SIZEOF_POINTER__ 8\n"
    "#define __SIZEOF_PTRDIFF_T__ 8\n"
    "#define __SIZEOF_SHORT__ 2\n"
    "#define __SIZEOF_SIZE_T__ 8\n"
    "\n"
    "#define __BYTE_ORDER__ 1234\n"
    "#define __ORDER_LITTLE_ENDIAN__ 1234\n"
    "#define __ORDER_BIG_ENDIAN__ 4321\n"
    "\n"
    "/* Some GCC predefined macros: */\n"
    "#define __SIZE_TYPE__ unsigned long\n"
    "#define __PTRDIFF_TYPE__ long\n"
    "#define __INTMAX_TYPE__ long\n"
    "#define __UINTMAX_TYPE__ unsigned long\n"
    "#define __INT8_TYPE__ signed char\n"
    "#define __INT16_TYPE__ short\n"
    "#define __INT32_TYPE__ int\n"
    "#define __INT64_TYPE__ long\n"
    "#define __UINT8_TYPE__ unsigned char\n"
    "#define __UINT16_TYPE__ unsigned short\n"
    "#define __UINT32_TYPE__ unsigned int\n"
    "#define __UINT64_TYPE__ unsigned long\n"
    "#define __INTPTR_TYPE__ long\n"
    "#define __UINTPTR_TYPE__ unsigned long\n"
    "\n"
    "#define __CHAR_BIT__ 8\n"
    "#define __INT8_MAX__ 127\n"
    "#define __INT16_MAX__ 32767\n"
    "#define __INT32_MAX__ 2147483647\n"
    "#define __INT64_MAX__ 9223372036854775807l\n"
    "#define __UINT8_MAX__ (__INT8_MAX__ * 2u + 1u)\n"
    "#define __UINT16_MAX__ (__INT16_MAX__ * 2u + 1u)\n"
    "#define __UINT32_MAX__ (__INT32_MAX__ * 2u + 1u)\n"
    "#define __UINT64_MAX__ (__INT64_MAX__ * 2u + 1u)\n"
#if defined(__linux__)
    "#define __WCHAR_MAX__ 0x7fffffff\n"
#else
    "#define __WCHAR_MAX__ 2147483647\n"
#endif
    "#define __WCHAR_MIN__ (-__WCHAR_MAX__ - 1)\n"
    "#define __SCHAR_MAX__ __INT8_MAX__\n"
    "#define __SHRT_MAX__ __INT16_MAX__\n"
    "#define __INT_MAX__ __INT32_MAX__\n"
    "#define __LONG_MAX__ __INT64_MAX__\n"
    "#define __LONG_LONG_MAX__ __INT64_MAX__\n"
    "#define __SIZE_MAX__ __UINT64_MAX__\n"
    "#define __PTRDIFF_MAX__ __INT64_MAX__\n"
    "#define __INTMAX_MAX__ __INT64_MAX__\n"
    "#define __UINTMAX_MAX__ __UINT64_MAX__\n"
    "#define __INTPTR_MAX__ __INT64_MAX__\n"
    "#define __UINTPTR_MAX__ __UINT64_MAX__\n"
    "\n"
    "#define __FLT_MIN_EXP__ (-125)\n"
    "#define __FLT_MAX_EXP__ 128\n"
    "#define __FLT_DIG__ 6\n"
    "#define __FLT_DECIMAL_DIG__ 9\n"
    "#define __FLT_MANT_DIG__ 24\n"
    "#define __FLT_MIN__ 1.17549435082228750796873653722224568e-38F\n"
    "#define __FLT_MAX__ 3.40282346638528859811704183484516925e+38F\n"
    "#define __FLT_EPSILON__ 1.19209289550781250000000000000000000e-7F\n"
    "\n"
    "#define __DBL_MIN_EXP__ (-1021)\n"
    "#define __DBL_MAX_EXP__ 1024\n"
    "#define __DBL_DIG__ 15\n"
    "#define __DBL_DECIMAL_DIG__ 17\n"
    "#define __DBL_MANT_DIG__ 53\n"
    "#define __DBL_MAX__ ((double) 1.79769313486231570814527423731704357e+308L)\n"
    "#define __DBL_MIN__ ((double) 2.22507385850720138309023271733240406e-308L)\n"
    "#define __DBL_EPSILON__ ((double) 2.22044604925031308084726333618164062e-16L)\n"
    "\n"
    "#define __LDBL_MIN_EXP__ (-16381)\n"
    "#define __LDBL_MAX_EXP__ 16384\n"
    "#define __LDBL_DIG__ 18\n"
    "#define __LDBL_DECIMAL_DIG__ 21\n"
    "#define __LDBL_MANT_DIG__ 64\n"
    "#define __LDBL_MAX__ 1.18973149535723176508575932662800702e+4932L\n"
    "#define __LDBL_MIN__ 3.36210314311209350626267781732175260e-4932L\n"
    "#define __LDBL_EPSILON__ 1.08420217248550443400745280086994171e-19L\n"
    "\n"
    "typedef unsigned short char16_t;\n"
    "typedef unsigned int char32_t;\n"
    "\n"
#if defined(__linux__)
    "#define __gnu_linux__ 1\n"
    "#define __linux 1\n"
    "#define __linux__ 1\n"
    "#define linux 1\n"
    "#define __unix 1\n"
    "#define __unix__ 1\n"
#ifndef __GNU_LIBRARY__
    "typedef struct {\n"
    "  unsigned int gp_offset;\n"
    "  unsigned int fp_offset;\n"
    "  void *overflow_arg_area;\n"
    "  void *reg_save_area;\n"
    "} __builtin_va_list[1];\n"
    "typedef __builtin_va_list va_list;\n"
    "#define __DEFINED_va_list\n"
#endif
#elif defined(__APPLE__)
    "#define __APPLE__ 1\n"
    "#define __DARWIN_OS_INLINE static inline\n"
    "typedef struct {\n"
    "  unsigned int gp_offset;\n"
    "  unsigned int fp_offset;\n"
    "  void *overflow_arg_area;\n"
    "  void *reg_save_area;\n"
    "} __darwin_va_list[1];\n"
#else
#error Uknown OS
#endif
    "\n"
    "void *alloca (unsigned long);\n";
