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

/* ------------------------------------------------------------------------ */
/*  Locks                                                                   */
/* ------------------------------------------------------------------------ */

#if (defined SLJIT_EXECUTABLE_ALLOCATOR && SLJIT_EXECUTABLE_ALLOCATOR) || (defined SLJIT_UTIL_GLOBAL_LOCK && SLJIT_UTIL_GLOBAL_LOCK)

#if (defined SLJIT_SINGLE_THREADED && SLJIT_SINGLE_THREADED)

#if (defined SLJIT_EXECUTABLE_ALLOCATOR && SLJIT_EXECUTABLE_ALLOCATOR)

static __inline void allocator_grab_lock(void)
{
	/* Always successful. */
}

static __inline void allocator_release_lock(void)
{
	/* Always successful. */
}

#endif /* SLJIT_EXECUTABLE_ALLOCATOR */

#if (defined SLJIT_UTIL_GLOBAL_LOCK && SLJIT_UTIL_GLOBAL_LOCK)

void SLJIT_CALL sljit_grab_lock(void)
{
	/* Always successful. */
}

void SLJIT_CALL sljit_release_lock(void)
{
	/* Always successful. */
}

#endif /* SLJIT_UTIL_GLOBAL_LOCK */

#elif defined(_WIN32) /* SLJIT_SINGLE_THREADED */

#include "windows.h"

#if (defined SLJIT_EXECUTABLE_ALLOCATOR && SLJIT_EXECUTABLE_ALLOCATOR)

static HANDLE allocator_mutex = 0;

static __inline void allocator_grab_lock(void)
{
	/* No idea what to do if an error occures. Static mutexes should never fail... */
	if (!allocator_mutex)
		allocator_mutex = CreateMutex(NULL, TRUE, NULL);
	else
		WaitForSingleObject(allocator_mutex, INFINITE);
}

static __inline void allocator_release_lock(void)
{
	ReleaseMutex(allocator_mutex);
}

#endif /* SLJIT_EXECUTABLE_ALLOCATOR */

#if (defined SLJIT_UTIL_GLOBAL_LOCK && SLJIT_UTIL_GLOBAL_LOCK)

static HANDLE global_mutex = 0;

void SLJIT_CALL sljit_grab_lock(void)
{
	/* No idea what to do if an error occures. Static mutexes should never fail... */
	if (!global_mutex)
		global_mutex = CreateMutex(NULL, TRUE, NULL);
	else
		WaitForSingleObject(global_mutex, INFINITE);
}

void SLJIT_CALL sljit_release_lock(void)
{
	ReleaseMutex(global_mutex);
}

#endif /* SLJIT_UTIL_GLOBAL_LOCK */

#else /* _WIN32 */

#if (defined SLJIT_EXECUTABLE_ALLOCATOR && SLJIT_EXECUTABLE_ALLOCATOR)

#include <pthread.h>

static pthread_mutex_t allocator_mutex = PTHREAD_MUTEX_INITIALIZER;

static __inline void allocator_grab_lock(void)
{
	pthread_mutex_lock(&allocator_mutex);
}

static __inline void allocator_release_lock(void)
{
	pthread_mutex_unlock(&allocator_mutex);
}

#endif /* SLJIT_EXECUTABLE_ALLOCATOR */

#if (defined SLJIT_UTIL_GLOBAL_LOCK && SLJIT_UTIL_GLOBAL_LOCK)

#include <pthread.h>

static pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;

void SLJIT_CALL sljit_grab_lock(void)
{
	pthread_mutex_lock(&global_mutex);
}

void SLJIT_CALL sljit_release_lock(void)
{
	pthread_mutex_unlock(&global_mutex);
}

#endif /* SLJIT_UTIL_GLOBAL_LOCK */

#endif /* _WIN32 */

/* ------------------------------------------------------------------------ */
/*  Stack                                                                   */
/* ------------------------------------------------------------------------ */

#if (defined SLJIT_UTIL_STACK && SLJIT_UTIL_STACK) || (defined SLJIT_EXECUTABLE_ALLOCATOR && SLJIT_EXECUTABLE_ALLOCATOR)

#ifdef _WIN32
#include "windows.h"
#else
/* Provides mmap function. */
#include <sys/mman.h>
/* For detecting the page size. */
#include <unistd.h>

#ifndef MAP_ANON

#include <fcntl.h>

/* Some old systems does not have MAP_ANON. */
static int dev_zero = -1;

#if (defined SLJIT_SINGLE_THREADED && SLJIT_SINGLE_THREADED)

static __inline int open_dev_zero(void)
{
	dev_zero = open("/dev/zero", O_RDWR);
	return dev_zero < 0;
}

#else /* SLJIT_SINGLE_THREADED */

#include <pthread.h>

static pthread_mutex_t dev_zero_mutex = PTHREAD_MUTEX_INITIALIZER;

static __inline int open_dev_zero(void)
{
	pthread_mutex_lock(&dev_zero_mutex);
	dev_zero = open("/dev/zero", O_RDWR);
	pthread_mutex_unlock(&dev_zero_mutex);
	return dev_zero < 0;
}

#endif /* SLJIT_SINGLE_THREADED */

#endif

#endif

#endif /* SLJIT_UTIL_STACK || SLJIT_EXECUTABLE_ALLOCATOR */

#if (defined SLJIT_UTIL_STACK && SLJIT_UTIL_STACK)

/* Planning to make it even more clever in the future. */
static long uintptr_tage_align = 0;

struct sljit_stack* SLJIT_CALL sljit_allocate_stack(unsigned long limit, unsigned long max_limit)
{
	struct sljit_stack *stack;
	union {
		void *ptr;
		unsigned long uw;
	} base;
#ifdef _WIN32
	SYSTEM_INFO si;
#endif

	if (limit > max_limit || limit < 1)
		return NULL;

#ifdef _WIN32
	if (!uintptr_tage_align) {
		GetSystemInfo(&si);
		uintptr_tage_align = si.dwPageSize - 1;
	}
#else
	if (!uintptr_tage_align) {
		uintptr_tage_align = sysconf(_SC_PAGESIZE);
		/* Should never happen. */
		if (uintptr_tage_align < 0)
			uintptr_tage_align = 4096;
		uintptr_tage_align--;
	}
#endif

	/* Align limit and max_limit. */
	max_limit = (max_limit + uintptr_tage_align) & ~uintptr_tage_align;

	stack = malloc(sizeof(struct sljit_stack));
	if (!stack)
		return NULL;

#ifdef _WIN32
	base.ptr = VirtualAlloc(NULL, max_limit, MEM_RESERVE, PAGE_READWRITE);
	if (!base.ptr) {
		free(stack);
		return NULL;
	}
	stack->base = base.uw;
	stack->limit = stack->base;
	stack->max_limit = stack->base + max_limit;
	if (sljit_stack_resize(stack, stack->base + limit)) {
		sljit_free_stack(stack);
		return NULL;
	}
#else
#ifdef MAP_ANON
	base.ptr = mmap(NULL, max_limit, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
#else
	if (dev_zero < 0) {
		if (open_dev_zero()) {
			free(stack);
			return NULL;
		}
	}
	base.ptr = mmap(NULL, max_limit, PROT_READ | PROT_WRITE, MAP_PRIVATE, dev_zero, 0);
#endif
	if (base.ptr == MAP_FAILED) {
		free(stack);
		return NULL;
	}
	stack->base = base.uw;
	stack->limit = stack->base + limit;
	stack->max_limit = stack->base + max_limit;
#endif
	stack->top = stack->base;
	return stack;
}

#undef PAGE_ALIGN

void SLJIT_CALL sljit_free_stack(struct sljit_stack* stack)
{
#ifdef _WIN32
	VirtualFree((void*)stack->base, 0, MEM_RELEASE);
#else
	munmap((void*)stack->base, stack->max_limit - stack->base);
#endif
	free(stack);
}

long SLJIT_CALL sljit_stack_resize(struct sljit_stack* stack, unsigned long new_limit)
{
	unsigned long aligned_old_limit;
	unsigned long aligned_new_limit;

	if ((new_limit > stack->max_limit) || (new_limit < stack->base))
		return -1;
#ifdef _WIN32
	aligned_new_limit = (new_limit + uintptr_tage_align) & ~uintptr_tage_align;
	aligned_old_limit = (stack->limit + uintptr_tage_align) & ~uintptr_tage_align;
	if (aligned_new_limit != aligned_old_limit) {
		if (aligned_new_limit > aligned_old_limit) {
			if (!VirtualAlloc((void*)aligned_old_limit, aligned_new_limit - aligned_old_limit, MEM_COMMIT, PAGE_READWRITE))
				return -1;
		}
		else {
			if (!VirtualFree((void*)aligned_new_limit, aligned_old_limit - aligned_new_limit, MEM_DECOMMIT))
				return -1;
		}
	}
	stack->limit = new_limit;
	return 0;
#else
	if (new_limit >= stack->limit) {
		stack->limit = new_limit;
		return 0;
	}
	aligned_new_limit = (new_limit + uintptr_tage_align) & ~uintptr_tage_align;
	aligned_old_limit = (stack->limit + uintptr_tage_align) & ~uintptr_tage_align;
	/* If madvise is available, we release the unnecessary space. */
#if defined(MADV_DONTNEED)
	if (aligned_new_limit < aligned_old_limit)
		madvise((void*)aligned_new_limit, aligned_old_limit - aligned_new_limit, MADV_DONTNEED);
#elif defined(POSIX_MADV_DONTNEED)
	if (aligned_new_limit < aligned_old_limit)
		posix_madvise((void*)aligned_new_limit, aligned_old_limit - aligned_new_limit, POSIX_MADV_DONTNEED);
#endif
	stack->limit = new_limit;
	return 0;
#endif
}

#endif /* SLJIT_UTIL_STACK */

#endif
