/*
** Target architecture selection.
** Copyright (C) 2005-2017 Mike Pall. See Copyright Notice in luajit.h
*/

#ifndef RAVI_ARCH_H
#define RAVI_ARCH_H

#include "lua.h"

/* Target endianess. */
#define RAVI__ARCH_LE	0
#define RAVI__ARCH_BE	1

/* Target architectures. */
#define RAVI__ARCH_X86		1
#define RAVI__ARCH_x86		1
#define RAVI__ARCH_X64		2
#define RAVI__ARCH_x64		2
#define RAVI__ARCH_ARM		3
#define RAVI__ARCH_arm		3
#define RAVI__ARCH_ARM64	4
#define RAVI__ARCH_arm64	4
#define RAVI__ARCH_PPC		5
#define RAVI__ARCH_ppc		5
#define RAVI__ARCH_MIPS	6
#define RAVI__ARCH_mips	6
#define RAVI__ARCH_MIPS32	6
#define RAVI__ARCH_mips32	6
#define RAVI__ARCH_MIPS64	7
#define RAVI__ARCH_mips64	7

/* Target OS. */
#define RAVI__OS_OTHER		0
#define RAVI__OS_WINDOWS	1
#define RAVI__OS_LINUX		2
#define RAVI__OS_OSX		3
#define RAVI__OS_BSD		4
#define RAVI__OS_POSIX		5

/* Select native target if no target defined. */
#ifndef RAVI__TARGET
	#if defined(__i386) || defined(__i386__) || defined(_M_IX86)
		#define RAVI__TARGET	RAVI__ARCH_X86
	#elif defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined(_M_AMD64)
		#define RAVI__TARGET	RAVI__ARCH_X64
	#elif defined(__arm__) || defined(__arm) || defined(__ARM__) || defined(__ARM)
		#error "No support for this architecture (yet)"
		#define RAVI__TARGET	RAVI__ARCH_ARM
	#elif defined(__aarch64__)
		#error "No support for this architecture (yet)"
		#define RAVI__TARGET	RAVI__ARCH_ARM64
	#elif defined(__ppc__) || defined(__ppc) || defined(__PPC__) || defined(__PPC) || defined(__powerpc__) || defined(__powerpc) || defined(__POWERPC__) || defined(__POWERPC) || defined(_M_PPC)
		#error "No support for this architecture (yet)"
		#define RAVI__TARGET	RAVI__ARCH_PPC
	#elif defined(__mips64__) || defined(__mips64) || defined(__MIPS64__) || defined(__MIPS64)
		#error "No support for this architecture (yet)"
		#define RAVI__TARGET	RAVI__ARCH_MIPS64
	#elif defined(__mips__) || defined(__mips) || defined(__MIPS__) || defined(__MIPS)
		#error "No support for this architecture (yet)"
		#define RAVI__TARGET	RAVI__ARCH_MIPS32
	#else
		#error "No support for this architecture (yet)"
	#endif
#endif

/* Select native OS if no target OS defined. */
#ifndef RAVI__OS
	#if defined(_WIN32) && !defined(_XBOX_VER)
		#define RAVI__OS	RAVI__OS_WINDOWS
	#elif defined(__linux__)
		#define RAVI__OS	RAVI__OS_LINUX
	#elif defined(__MACH__) && defined(__APPLE__)
		#define RAVI__OS	RAVI__OS_OSX
	#elif (defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || \
       defined(__NetBSD__) || defined(__OpenBSD__) || \
       defined(__DragonFly__)) && !defined(__ORBIS__)
		#define RAVI__OS	RAVI__OS_BSD
	#elif (defined(__sun__) && defined(__svr4__)) || defined(__HAIKU__)
		#define RAVI__OS	RAVI__OS_POSIX
	#elif defined(__CYGWIN__)
		#define RAVI_TARGET_CYGWIN	1
		#define RAVI__OS	RAVI__OS_POSIX
	#else
		#define RAVI__OS	RAVI__OS_OTHER
	#endif
#endif

/* Set target OS properties. */
#if RAVI__OS == RAVI__OS_WINDOWS
	#define RAVI_OS_NAME	"Windows"
#elif RAVI__OS == RAVI__OS_LINUX
	#define RAVI_OS_NAME	"Linux"
#elif RAVI__OS == RAVI__OS_OSX
	#define RAVI_OS_NAME	"OSX"
#elif RAVI__OS == RAVI__OS_BSD
	#define RAVI_OS_NAME	"BSD"
#elif RAVI__OS == RAVI__OS_POSIX
	#define RAVI_OS_NAME	"POSIX"
#else
	#define RAVI_OS_NAME	"Other"
#endif

#define RAVI_TARGET_WINDOWS	(RAVI__OS == RAVI__OS_WINDOWS)
#define RAVI_TARGET_LINUX		(RAVI__OS == RAVI__OS_LINUX)
#define RAVI_TARGET_OSX		(RAVI__OS == RAVI__OS_OSX)
#define RAVI_TARGET_IOS		(RAVI_TARGET_OSX && (RAVI__TARGET == RAVI__ARCH_ARM || RAVI__TARGET == RAVI__ARCH_ARM64))
#define RAVI_TARGET_POSIX		(RAVI__OS > RAVI__OS_WINDOWS)
#define RAVI_TARGET_DLOPEN	RAVI_TARGET_POSIX

/* Set target architecture properties. */
#if RAVI__TARGET == RAVI__ARCH_X86

	#define RAVI_ARCH_NAME		"x86"
	#define RAVI_ARCH_BITS		32
	#define RAVI_ARCH_ENDIAN		RAVI__ARCH_LE
	#if RAVI_TARGET_WINDOWS || RAVI_TARGET_CYGWIN
		#define RAVI_ABI_WIN		1
	#else
		#define RAVI_ABI_WIN		0
	#endif
	#define RAVI_TARGET_X86		1
	#define RAVI_TARGET_X86ORX64	1
	#define RAVI_TARGET_EHRETREG	0
	#define RAVI_TARGET_MASKSHIFT	1
	#define RAVI_TARGET_MASKROT	1
	#define RAVI_TARGET_UNALIGNED	1

#elif RAVI__TARGET == RAVI__ARCH_X64

	#define RAVI_ARCH_NAME		"x64"
	#define RAVI_ARCH_BITS		64
	#define RAVI_ARCH_ENDIAN		RAVI__ARCH_LE
	#if RAVI_TARGET_WINDOWS || RAVI_TARGET_CYGWIN
		#define RAVI_ABI_WIN		1
	#else
		#define RAVI_ABI_WIN		0
	#endif
	#define RAVI_TARGET_X64		1
	#define RAVI_TARGET_X86ORX64	1
	#define RAVI_TARGET_EHRETREG	0
	#define RAVI_TARGET_JUMPRANGE	31	/* +-2^31 = +-2GB */
	#define RAVI_TARGET_MASKSHIFT	1
	#define RAVI_TARGET_MASKROT	1
	#define RAVI_TARGET_UNALIGNED	1

#else
	#error "No target architecture defined"
#endif

#ifndef RAVI_PAGESIZE
#define RAVI_PAGESIZE		4096
#endif

#if RAVI_ARCH_ENDIAN == RAVI__ARCH_BE
#define RAVI_LE			0
#define RAVI_BE			1
#define RAVI_ENDIAN_SELECT(le, be)	be
#define RAVI_ENDIAN_LOHI(lo, hi)		hi lo
#else
#define RAVI_LE			1
#define RAVI_BE			0
#define RAVI_ENDIAN_SELECT(le, be)	le
#define RAVI_ENDIAN_LOHI(lo, hi)		lo hi
#endif

#if RAVI_ARCH_BITS == 32
#define RAVI_32			1
#define RAVI_64			0
#else
#define RAVI_32			0
#define RAVI_64			1
#endif

#ifndef RAVI_TARGET_UNALIGNED
#define RAVI_TARGET_UNALIGNED	0
#endif

#endif
