/*
 *    Stack-less Just-In-Time compiler
 *
 *    Copyright 2009-2012 Zoltan Herczeg (hzmester@freemail.hu). All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice, this list of
 *      conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright notice, this list
 *      of conditions and the following disclaimer in the documentation and/or other materials
 *      provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDER(S) OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _SLJIT_CONFIG_INTERNAL_H_
#define _SLJIT_CONFIG_INTERNAL_H_

/*
   SLJIT defines the following architecture dependent types and macros:

   Macros for feature detection (boolean):
     SLJIT_32BIT_ARCHITECTURE : 32 bit architecture
     SLJIT_64BIT_ARCHITECTURE : 64 bit architecture
     SLJIT_LITTLE_ENDIAN : little endian architecture
     SLJIT_BIG_ENDIAN : big endian architecture
     SLJIT_UNALIGNED : allows unaligned memory accesses for non-fpu operations (only!)
     SLJIT_INDIRECT_CALL : see SLJIT_FUNC_OFFSET() for more information

   Constants:
     SLJIT_NUM_REGS : number of available regs
     SLJIT_NUM_SCRATCH_REGS : number of available scratch regs
     SLJIT_NUM_SAVED_REGS : number of available saved regs
     SLJIT_NUM_FLOAT_REGS : number of available floating point regs
     SLJIT_NUM_SCRATCH_FLOAT_REGS : number of available floating point scratch regs
     SLJIT_NUM_SAVED_FLOAT_REGS : number of available floating point saved regs
     SLJIT_WORD_SHIFT : the shift required to apply when accessing a long/unsigned long array by index
     SLJIT_DOUBLE_SHIFT : the shift required to apply when accessing
                          a double precision floating point array by index
     SLJIT_SINGLE_SHIFT : the shift required to apply when accessing
                          a single precision floating point array by index
     SLJIT_LOCALS_OFFSET : local space starting offset (SLJIT_SP + SLJIT_LOCALS_OFFSET)
     SLJIT_RETURN_ADDRESS_OFFSET : a return instruction always adds this offset to the return address

   Other macros:
     SLJIT_CALL : C calling convention define for both calling JIT form C and C callbacks for JIT
     SLJIT_W(number) : defining 64 bit constants on 64 bit architectures (compiler independent helper)
*/

/*****************/
/* Sanity check. */
/*****************/

#if !((defined SLJIT_CONFIG_X86_32 && SLJIT_CONFIG_X86_32) \
	|| (defined SLJIT_CONFIG_X86_64 && SLJIT_CONFIG_X86_64) \
	|| (defined SLJIT_CONFIG_ARM_V5 && SLJIT_CONFIG_ARM_V5) \
	|| (defined SLJIT_CONFIG_ARM_V7 && SLJIT_CONFIG_ARM_V7) \
	|| (defined SLJIT_CONFIG_ARM_THUMB2 && SLJIT_CONFIG_ARM_THUMB2) \
	|| (defined SLJIT_CONFIG_ARM_64 && SLJIT_CONFIG_ARM_64) \
	|| (defined SLJIT_CONFIG_PPC_32 && SLJIT_CONFIG_PPC_32) \
	|| (defined SLJIT_CONFIG_PPC_64 && SLJIT_CONFIG_PPC_64) \
	|| (defined SLJIT_CONFIG_MIPS_32 && SLJIT_CONFIG_MIPS_32) \
	|| (defined SLJIT_CONFIG_MIPS_64 && SLJIT_CONFIG_MIPS_64) \
	|| (defined SLJIT_CONFIG_SPARC_32 && SLJIT_CONFIG_SPARC_32) \
	|| (defined SLJIT_CONFIG_TILEGX && SLJIT_CONFIG_TILEGX) \
	|| (defined SLJIT_CONFIG_AUTO && SLJIT_CONFIG_AUTO) \
	|| (defined SLJIT_CONFIG_UNSUPPORTED && SLJIT_CONFIG_UNSUPPORTED))
#error "An architecture must be selected"
#endif

#if (defined SLJIT_CONFIG_X86_32 && SLJIT_CONFIG_X86_32) \
	+ (defined SLJIT_CONFIG_X86_64 && SLJIT_CONFIG_X86_64) \
	+ (defined SLJIT_CONFIG_ARM_V5 && SLJIT_CONFIG_ARM_V5) \
	+ (defined SLJIT_CONFIG_ARM_V7 && SLJIT_CONFIG_ARM_V7) \
	+ (defined SLJIT_CONFIG_ARM_THUMB2 && SLJIT_CONFIG_ARM_THUMB2) \
	+ (defined SLJIT_CONFIG_ARM_64 && SLJIT_CONFIG_ARM_64) \
	+ (defined SLJIT_CONFIG_PPC_32 && SLJIT_CONFIG_PPC_32) \
	+ (defined SLJIT_CONFIG_PPC_64 && SLJIT_CONFIG_PPC_64) \
	+ (defined SLJIT_CONFIG_TILEGX && SLJIT_CONFIG_TILEGX) \
	+ (defined SLJIT_CONFIG_MIPS_32 && SLJIT_CONFIG_MIPS_32) \
	+ (defined SLJIT_CONFIG_MIPS_64 && SLJIT_CONFIG_MIPS_64) \
	+ (defined SLJIT_CONFIG_SPARC_32 && SLJIT_CONFIG_SPARC_32) \
	+ (defined SLJIT_CONFIG_AUTO && SLJIT_CONFIG_AUTO) \
	+ (defined SLJIT_CONFIG_UNSUPPORTED && SLJIT_CONFIG_UNSUPPORTED) >= 2
#error "Multiple architectures are selected"
#endif

/********************************************************/
/* Automatic CPU detection (requires compiler support). */
/********************************************************/

#if (defined SLJIT_CONFIG_AUTO && SLJIT_CONFIG_AUTO)

#ifndef _WIN32

#if defined(__i386__) || defined(__i386)
#define SLJIT_CONFIG_X86_32 1
#elif defined(__x86_64__)
#define SLJIT_CONFIG_X86_64 1
#elif defined(__arm__) || defined(__ARM__)
#ifdef __thumb2__
#define SLJIT_CONFIG_ARM_THUMB2 1
#elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__)
#define SLJIT_CONFIG_ARM_V7 1
#else
#define SLJIT_CONFIG_ARM_V5 1
#endif
#elif defined (__aarch64__)
#define SLJIT_CONFIG_ARM_64 1
#elif defined(__ppc64__) || defined(__powerpc64__) || defined(_ARCH_PPC64) || (defined(_POWER) && defined(__64BIT__))
#define SLJIT_CONFIG_PPC_64 1
#elif defined(__ppc__) || defined(__powerpc__) || defined(_ARCH_PPC) || defined(_ARCH_PWR) || defined(_ARCH_PWR2) || defined(_POWER)
#define SLJIT_CONFIG_PPC_32 1
#elif defined(__mips__) && !defined(_LP64)
#define SLJIT_CONFIG_MIPS_32 1
#elif defined(__mips64)
#define SLJIT_CONFIG_MIPS_64 1
#elif defined(__sparc__) || defined(__sparc)
#define SLJIT_CONFIG_SPARC_32 1
#elif defined(__tilegx__)
#define SLJIT_CONFIG_TILEGX 1
#else
/* Unsupported architecture */
#define SLJIT_CONFIG_UNSUPPORTED 1
#endif

#else /* !_WIN32 */

#if defined(_M_X64) || defined(__x86_64__)
#define SLJIT_CONFIG_X86_64 1
#elif defined(_ARM_)
#define SLJIT_CONFIG_ARM_V5 1
#else
#define SLJIT_CONFIG_X86_32 1
#endif

#endif /* !WIN32 */
#endif /* SLJIT_CONFIG_AUTO */

#if (defined SLJIT_CONFIG_UNSUPPORTED && SLJIT_CONFIG_UNSUPPORTED)
#undef SLJIT_EXECUTABLE_ALLOCATOR
#endif

/******************************/
/* CPU family type detection. */
/******************************/

#if (defined SLJIT_CONFIG_ARM_V5 && SLJIT_CONFIG_ARM_V5) || (defined SLJIT_CONFIG_ARM_V7 && SLJIT_CONFIG_ARM_V7) \
	|| (defined SLJIT_CONFIG_ARM_THUMB2 && SLJIT_CONFIG_ARM_THUMB2)
#define SLJIT_CONFIG_ARM_32 1
#endif

#if (defined SLJIT_CONFIG_X86_32 && SLJIT_CONFIG_X86_32) || (defined SLJIT_CONFIG_X86_64 && SLJIT_CONFIG_X86_64)
#define SLJIT_CONFIG_X86 1
#elif (defined SLJIT_CONFIG_ARM_32 && SLJIT_CONFIG_ARM_32) || (defined SLJIT_CONFIG_ARM_64 && SLJIT_CONFIG_ARM_64)
#define SLJIT_CONFIG_ARM 1
#elif (defined SLJIT_CONFIG_PPC_32 && SLJIT_CONFIG_PPC_32) || (defined SLJIT_CONFIG_PPC_64 && SLJIT_CONFIG_PPC_64)
#define SLJIT_CONFIG_PPC 1
#elif (defined SLJIT_CONFIG_MIPS_32 && SLJIT_CONFIG_MIPS_32) || (defined SLJIT_CONFIG_MIPS_64 && SLJIT_CONFIG_MIPS_64)
#define SLJIT_CONFIG_MIPS 1
#elif (defined SLJIT_CONFIG_SPARC_32 && SLJIT_CONFIG_SPARC_32) || (defined SLJIT_CONFIG_SPARC_64 && SLJIT_CONFIG_SPARC_64)
#define SLJIT_CONFIG_SPARC 1
#endif

/***************************/
/* Compiler helper macros. */
/***************************/

/*********************************/
/* Type of public API functions. */
/*********************************/

/****************************/
/* Instruction cache flush. */
/****************************/

#ifndef SLJIT_CACHE_FLUSH

#if (defined SLJIT_CONFIG_X86 && SLJIT_CONFIG_X86)

/* Not required to implement on archs with unified caches. */
#define SLJIT_CACHE_FLUSH(from, to)

#elif defined __APPLE__

/* Supported by all macs since Mac OS 10.5.
   However, it does not work on non-jailbroken iOS devices,
   although the compilation is successful. */

#define SLJIT_CACHE_FLUSH(from, to) \
	sys_icache_invalidate((char*)(from), (char*)(to) - (char*)(from))

#elif defined __ANDROID__

/* Android lacks __clear_cache; instead, cacheflush should be used. */

#define SLJIT_CACHE_FLUSH(from, to) \
    cacheflush((long)(from), (long)(to), 0)

#elif (defined SLJIT_CONFIG_PPC && SLJIT_CONFIG_PPC)

/* The __clear_cache() implementation of GCC is a dummy function on PowerPC. */
#define SLJIT_CACHE_FLUSH(from, to) \
	ppc_cache_flush((from), (to))

#elif (defined SLJIT_CONFIG_SPARC_32 && SLJIT_CONFIG_SPARC_32)

/* The __clear_cache() implementation of GCC is a dummy function on Sparc. */
#define SLJIT_CACHE_FLUSH(from, to) \
	sparc_cache_flush((from), (to))

#else

/* Calls __ARM_NR_cacheflush on ARM-Linux. */
#define SLJIT_CACHE_FLUSH(from, to) \
	__clear_cache((char*)(from), (char*)(to))

#endif

#endif /* !SLJIT_CACHE_FLUSH */

/*
 * Used to maintain sanity/brevity. Admittedly a BSDism.
 */
typedef unsigned char	u_char;
typedef signed char	s_char;

/******************************************************/
/* Byte/half/int/word/single/double type definitions. */
/******************************************************/

#if !defined(_LP64) && !defined(__LP64__)
#error "long must be size of pointer"
#endif

#if (defined SLJIT_CONFIG_UNSUPPORTED && SLJIT_CONFIG_UNSUPPORTED)
/* Just to have something. */
#define SLJIT_WORD_SHIFT 0
#elif !(defined SLJIT_CONFIG_X86_64 && SLJIT_CONFIG_X86_64) \
	&& !(defined SLJIT_CONFIG_ARM_64 && SLJIT_CONFIG_ARM_64) \
	&& !(defined SLJIT_CONFIG_PPC_64 && SLJIT_CONFIG_PPC_64) \
	&& !(defined SLJIT_CONFIG_MIPS_64 && SLJIT_CONFIG_MIPS_64) \
	&& !(defined SLJIT_CONFIG_TILEGX && SLJIT_CONFIG_TILEGX)
#define SLJIT_32BIT_ARCHITECTURE 1
#define SLJIT_WORD_SHIFT 2
#else
#define SLJIT_64BIT_ARCHITECTURE 1
#define SLJIT_WORD_SHIFT 3
#endif

/* Shift for pointer sized data. */
#define SLJIT_POINTER_SHIFT SLJIT_WORD_SHIFT

/* Shift for double precision sized data. */
#define SLJIT_DOUBLE_SHIFT 3
#define SLJIT_SINGLE_SHIFT 2

#ifndef SLJIT_W

/* Defining long constants. */
#if (defined SLJIT_64BIT_ARCHITECTURE && SLJIT_64BIT_ARCHITECTURE)
#define SLJIT_W(w)	(w##ll)
#else
#define SLJIT_W(w)	(w)
#endif

#endif /* !SLJIT_W */

/*************************/
/* Endianness detection. */
/*************************/

#if !defined(SLJIT_BIG_ENDIAN) && !defined(SLJIT_LITTLE_ENDIAN)

/* These macros are mostly useful for the applications. */
#if (defined SLJIT_CONFIG_PPC_32 && SLJIT_CONFIG_PPC_32) \
	|| (defined SLJIT_CONFIG_PPC_64 && SLJIT_CONFIG_PPC_64)

#ifdef __LITTLE_ENDIAN__
#define SLJIT_LITTLE_ENDIAN 1
#else
#define SLJIT_BIG_ENDIAN 1
#endif

#elif (defined SLJIT_CONFIG_MIPS_32 && SLJIT_CONFIG_MIPS_32) \
	|| (defined SLJIT_CONFIG_MIPS_64 && SLJIT_CONFIG_MIPS_64)

#ifdef __MIPSEL__
#define SLJIT_LITTLE_ENDIAN 1
#else
#define SLJIT_BIG_ENDIAN 1
#endif

#elif (defined SLJIT_CONFIG_SPARC_32 && SLJIT_CONFIG_SPARC_32)

#define SLJIT_BIG_ENDIAN 1

#else
#define SLJIT_LITTLE_ENDIAN 1
#endif

#endif /* !defined(SLJIT_BIG_ENDIAN) && !defined(SLJIT_LITTLE_ENDIAN) */

/* Sanity check. */
#if (defined SLJIT_BIG_ENDIAN && SLJIT_BIG_ENDIAN) && (defined SLJIT_LITTLE_ENDIAN && SLJIT_LITTLE_ENDIAN)
#error "Exactly one endianness must be selected"
#endif

#if !(defined SLJIT_BIG_ENDIAN && SLJIT_BIG_ENDIAN) && !(defined SLJIT_LITTLE_ENDIAN && SLJIT_LITTLE_ENDIAN)
#error "Exactly one endianness must be selected"
#endif

#ifndef SLJIT_UNALIGNED

#if (defined SLJIT_CONFIG_X86_32 && SLJIT_CONFIG_X86_32) \
	|| (defined SLJIT_CONFIG_X86_64 && SLJIT_CONFIG_X86_64) \
	|| (defined SLJIT_CONFIG_ARM_V7 && SLJIT_CONFIG_ARM_V7) \
	|| (defined SLJIT_CONFIG_ARM_THUMB2 && SLJIT_CONFIG_ARM_THUMB2) \
	|| (defined SLJIT_CONFIG_ARM_64 && SLJIT_CONFIG_ARM_64) \
	|| (defined SLJIT_CONFIG_PPC_32 && SLJIT_CONFIG_PPC_32) \
	|| (defined SLJIT_CONFIG_PPC_64 && SLJIT_CONFIG_PPC_64)
#define SLJIT_UNALIGNED 1
#endif

#endif /* !SLJIT_UNALIGNED */

/*****************************************************************************************/
/* Calling convention of functions generated by SLJIT or called from the generated code. */
/*****************************************************************************************/

#ifndef SLJIT_CALL

#if (defined SLJIT_CONFIG_X86_32 && SLJIT_CONFIG_X86_32)

#if defined(__GNUC__) && !defined(__APPLE__)

#define SLJIT_CALL __attribute__ ((fastcall))
#define SLJIT_X86_32_FASTCALL 1

#elif defined(_MSC_VER)

#define SLJIT_CALL __fastcall
#define SLJIT_X86_32_FASTCALL 1

#elif defined(__BORLANDC__)

#define SLJIT_CALL __msfastcall
#define SLJIT_X86_32_FASTCALL 1

#else /* Unknown compiler. */

/* The cdecl attribute is the default. */
#define SLJIT_CALL

#endif

#else /* Non x86-32 architectures. */

#define SLJIT_CALL

#endif /* SLJIT_CONFIG_X86_32 */

#endif /* !SLJIT_CALL */

#ifndef SLJIT_INDIRECT_CALL
#if ((defined SLJIT_CONFIG_PPC_64 && SLJIT_CONFIG_PPC_64) && (defined SLJIT_BIG_ENDIAN && SLJIT_BIG_ENDIAN)) \
	|| ((defined SLJIT_CONFIG_PPC_32 && SLJIT_CONFIG_PPC_32) && defined _AIX)
/* It seems certain ppc compilers use an indirect addressing for functions
   which makes things complicated. */
#define SLJIT_INDIRECT_CALL 1
#endif
#endif /* SLJIT_INDIRECT_CALL */

/* The offset which needs to be substracted from the return address to
determine the next executed instruction after return. */
#ifndef SLJIT_RETURN_ADDRESS_OFFSET
#if (defined SLJIT_CONFIG_SPARC_32 && SLJIT_CONFIG_SPARC_32)
#define SLJIT_RETURN_ADDRESS_OFFSET 8
#else
#define SLJIT_RETURN_ADDRESS_OFFSET 0
#endif
#endif /* SLJIT_RETURN_ADDRESS_OFFSET */

/***************************************************/
/* Functions of the built-in executable allocator. */
/***************************************************/

#if (defined SLJIT_EXECUTABLE_ALLOCATOR && SLJIT_EXECUTABLE_ALLOCATOR)
void* sljit_malloc_exec(unsigned long size);
void sljit_free_exec(void* ptr);
void sljit_free_unused_memory_exec(void);
#define SLJIT_MALLOC_EXEC(size) sljit_malloc_exec(size)
#define SLJIT_FREE_EXEC(ptr) sljit_free_exec(ptr)
#endif

/**********************************************/
/* Registers and locals offset determination. */
/**********************************************/

#if (defined SLJIT_CONFIG_X86_32 && SLJIT_CONFIG_X86_32)

#define SLJIT_NUM_REGS 10
#define SLJIT_NUM_SAVED_REGS 7
#if (defined SLJIT_X86_32_FASTCALL && SLJIT_X86_32_FASTCALL)
#define SLJIT_LOCALS_OFFSET_BASE ((2 + 4) * sizeof(long))
#else
/* Maximum 3 arguments are passed on the stack, +1 for double alignment. */
#define SLJIT_LOCALS_OFFSET_BASE ((3 + 1 + 4) * sizeof(long))
#endif /* SLJIT_X86_32_FASTCALL */

#elif (defined SLJIT_CONFIG_X86_64 && SLJIT_CONFIG_X86_64)

#ifndef _WIN64
#define SLJIT_NUM_REGS 12
#define SLJIT_NUM_SAVED_REGS 6
#define SLJIT_LOCALS_OFFSET_BASE (sizeof(long))
#else
#define SLJIT_NUM_REGS 12
#define SLJIT_NUM_SAVED_REGS 8
#define SLJIT_LOCALS_OFFSET_BASE ((4 + 2) * sizeof(long))
#endif /* _WIN64 */

#elif (defined SLJIT_CONFIG_ARM_V5 && SLJIT_CONFIG_ARM_V5) || (defined SLJIT_CONFIG_ARM_V7 && SLJIT_CONFIG_ARM_V7)

#define SLJIT_NUM_REGS 11
#define SLJIT_NUM_SAVED_REGS 8
#define SLJIT_LOCALS_OFFSET_BASE 0

#elif (defined SLJIT_CONFIG_ARM_THUMB2 && SLJIT_CONFIG_ARM_THUMB2)

#define SLJIT_NUM_REGS 11
#define SLJIT_NUM_SAVED_REGS 7
#define SLJIT_LOCALS_OFFSET_BASE 0

#elif (defined SLJIT_CONFIG_ARM_64 && SLJIT_CONFIG_ARM_64)

#define SLJIT_NUM_REGS 25
#define SLJIT_NUM_SAVED_REGS 10
#define SLJIT_LOCALS_OFFSET_BASE (2 * sizeof(long))

#elif (defined SLJIT_CONFIG_PPC && SLJIT_CONFIG_PPC)

#define SLJIT_NUM_REGS 22
#define SLJIT_NUM_SAVED_REGS 17
#if (defined SLJIT_CONFIG_PPC_64 && SLJIT_CONFIG_PPC_64) || (defined _AIX)
#define SLJIT_LOCALS_OFFSET_BASE ((6 + 8) * sizeof(long))
#elif (defined SLJIT_CONFIG_PPC_32 && SLJIT_CONFIG_PPC_32)
/* Add +1 for double alignment. */
#define SLJIT_LOCALS_OFFSET_BASE ((3 + 1) * sizeof(long))
#else
#define SLJIT_LOCALS_OFFSET_BASE (3 * sizeof(long))
#endif /* SLJIT_CONFIG_PPC_64 || _AIX */

#elif (defined SLJIT_CONFIG_MIPS && SLJIT_CONFIG_MIPS)

#define SLJIT_NUM_REGS 17
#define SLJIT_NUM_SAVED_REGS 8
#if (defined SLJIT_CONFIG_MIPS_32 && SLJIT_CONFIG_MIPS_32)
#define SLJIT_LOCALS_OFFSET_BASE (4 * sizeof(long))
#else
#define SLJIT_LOCALS_OFFSET_BASE 0
#endif

#elif (defined SLJIT_CONFIG_SPARC && SLJIT_CONFIG_SPARC)

#define SLJIT_NUM_REGS 18
#define SLJIT_NUM_SAVED_REGS 14
#if (defined SLJIT_CONFIG_SPARC_32 && SLJIT_CONFIG_SPARC_32)
/* Add +1 for double alignment. */
#define SLJIT_LOCALS_OFFSET_BASE ((23 + 1) * sizeof(long))
#endif

#elif (defined SLJIT_CONFIG_UNSUPPORTED && SLJIT_CONFIG_UNSUPPORTED)

#define SLJIT_NUM_REGS 0
#define SLJIT_NUM_SAVED_REGS 0
#define SLJIT_LOCALS_OFFSET_BASE 0

#endif

#define SLJIT_LOCALS_OFFSET (SLJIT_LOCALS_OFFSET_BASE)

#define SLJIT_NUM_SCRATCH_REGS \
	(SLJIT_NUM_REGS - SLJIT_NUM_SAVED_REGS)

#define SLJIT_NUM_FLOAT_REGS 6
#if (defined SLJIT_CONFIG_X86_64 && SLJIT_CONFIG_X86_64) && (defined _WIN64)
#define SLJIT_NUM_SAVED_FLOAT_REGS 1
#else
#define SLJIT_NUM_SAVED_FLOAT_REGS 0
#endif

#define SLJIT_NUM_SCRATCH_FLOAT_REGS \
	(SLJIT_NUM_FLOAT_REGS - SLJIT_NUM_SAVED_FLOAT_REGS)

/*************************************/
/* Debug and verbose related macros. */
/*************************************/

#if (defined SLJIT_VERBOSE && SLJIT_VERBOSE)
#include <stdio.h>
#endif

#if (defined SLJIT_DEBUG && SLJIT_DEBUG)

#if !defined(SLJIT_ASSERT) || !defined(SLJIT_ASSERT_STOP)

/* SLJIT_HALT_PROCESS must halt the process. */
#ifndef SLJIT_HALT_PROCESS
#include <stdlib.h>

#define SLJIT_HALT_PROCESS() \
	abort();
#endif /* !SLJIT_HALT_PROCESS */

#include <stdio.h>

#endif /* !SLJIT_ASSERT || !SLJIT_ASSERT_STOP */

/* Feel free to redefine these two macros. */
#ifndef SLJIT_ASSERT

#define SLJIT_ASSERT(x) \
	do { \
		if (!(x)) { \
			printf("Assertion failed at " __FILE__ ":%d\n", __LINE__); \
			SLJIT_HALT_PROCESS(); \
		} \
	} while (0)

#endif /* !SLJIT_ASSERT */

#ifndef SLJIT_ASSERT_STOP

#define SLJIT_ASSERT_STOP() \
	do { \
		printf("Should never been reached " __FILE__ ":%d\n", __LINE__); \
		SLJIT_HALT_PROCESS(); \
	} while (0)

#endif /* !SLJIT_ASSERT_STOP */

#else /* (defined SLJIT_DEBUG && SLJIT_DEBUG) */

/* Forcing empty, but valid statements. */
#undef SLJIT_ASSERT
#undef SLJIT_ASSERT_STOP

#define SLJIT_ASSERT(x) \
	do { } while (0)
#define SLJIT_ASSERT_STOP() \
	do { } while (0)

#endif /* (defined SLJIT_DEBUG && SLJIT_DEBUG) */

#ifndef SLJIT_COMPILE_ASSERT

/* Should be improved eventually. */
#define SLJIT_COMPILE_ASSERT(x, description) \
	SLJIT_ASSERT(x)

#endif /* !SLJIT_COMPILE_ASSERT */

#endif
