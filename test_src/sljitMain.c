/*
 *    Stack-less Just-In-Time compiler
 *
 *    Copyright 2009-2010 Zoltan Herczeg (hzmester@freemail.hu). All rights reserved.
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

#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "sljitLir.h"

void sljit_test(int argc, char* argv[]);

union executable_code {
	void* code;
	long (SLJIT_CALL *func)(long* a);
};
typedef union executable_code executable_code;

void devel(void)
{
	executable_code code;
	struct sljit_compiler *compiler;
	long buf[4] = {5, 12, 0, 0};

	if ((compiler = sljit_create_compiler()) == NULL)
		errx(-1, "out of memory");

#if (defined SLJIT_VERBOSE && SLJIT_VERBOSE)
	sljit_compiler_verbose(compiler, stdout);
#endif
	sljit_emit_enter(compiler, 0, 1, 4, 5, 4, 0, 2 * sizeof(long));

	sljit_emit_return(compiler, SLJIT_MOV, SLJIT_RETURN_REG, 0);

	code.code = sljit_generate_code(compiler);
	sljit_free_compiler(compiler);

	printf("Code at: %p\n", (void*)SLJIT_FUNC_OFFSET(code.code));

	printf("Function returned with %ld\n", (long)code.func((long*)buf));
	printf("buf[0] = %ld\n", (long)buf[0]);
	printf("buf[1] = %ld\n", (long)buf[1]);
	printf("buf[2] = %ld\n", (long)buf[2]);
	printf("buf[3] = %ld\n", (long)buf[3]);
	sljit_free_code(code.code);
}

int main(int argc, char* argv[])
{
	/* devel(); */
	sljit_test(argc, argv);

	return 0;
}
