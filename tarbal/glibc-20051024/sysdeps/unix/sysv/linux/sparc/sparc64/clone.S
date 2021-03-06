/* Copyright (C) 1997, 2000 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Richard Henderson (rth@tamu.edu).

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

/* clone() is even more special than fork() as it mucks with stacks
   and invokes a function in the right context after its all over.  */

#include <asm/errno.h>
#include <asm/unistd.h>
#include <tcb-offsets.h>

#define CLONE_VM	0x00000100
#define CLONE_THREAD	0x00010000

/* int clone(int (*fn)(void *arg), void *child_stack, int flags, void *arg,
	     pid_t *ptid, void *tls, pid_t *ctid); */

	.register	%g2,#scratch
	.register	%g3,#scratch

	.text
	.align	4
	.globl	__clone
	.type	__clone,@function

__clone:
	save	%sp, -192, %sp

	/* sanity check arguments */
	brz,pn	%i0, 99f		/* fn non-NULL? */
	 mov	%i0, %g2
	brz,pn	%i1, 99f		/* child_stack non-NULL? */
	 mov	%i2, %o0		/* clone flags */

	/* The child_stack is the top of the stack, allocate one
	   whole stack frame from that as this is what the kernel
	   expects.  Also, subtract STACK_BIAS.  */
	sub	%i1, 192 + 0x7ff, %o1
	mov	%i3, %g3
	mov	%i2, %g4

	mov	%i4,%o2			/* PTID */
	mov	%i5,%o3			/* TLS */
	ldx	[%fp+0x7ff+176],%o4	/* CTID */

	/* Do the system call */
	set	__NR_clone, %g1
	ta	0x6d
	bcs,pn	%xcc, 99f
	 nop
	brnz,pn	%o1, __thread_start
	 nop
	ret
	 restore %o0, %g0, %o0
99:
#ifndef _LIBC_REENTRANT
#ifdef PIC
	call	1f
	sethi	%hi(_GLOBAL_OFFSET_TABLE_-(99b-.)), %l7
1:	or	%l7, %lo(_GLOBAL_OFFSET_TABLE_-(99b-.)), %l7
	add	%l7, %o7, %l7
	set	EINVAL, %i0
	sethi	%hi(errno), %g2
	or	%g2, %lo(errno), %g2
	st	%i0, [%l7+%g2]
#else
	sethi	%hi(errno), %g2
	set	EINVAL, %i0
	st	%i0, [%g2+%lo(errno)]
#endif
#else
	call	__errno_location
	 nop
	st	%i0, [%o0]
#endif
	ret
	 restore %g0,-1,%o0
	.size __clone, .-__clone

	.type __thread_start,@function
__thread_start:
#ifdef RESET_PID
	sethi	%hi(CLONE_THREAD), %l0
	andcc	%g4, %l0, %g0
	bne,pt	%icc, 1f
	 andcc	%g4, CLONE_VM, %g0
	bne,a,pn %icc, 2f
	 mov	-1,%o0
	set	__NR_getpid,%g1
	ta	0x6d
2:	st	%o0,[%g7 + PID]
	st	%o0,[%g7 + TID]
1:
#endif
	mov	%g0, %fp	/* terminate backtrace */
	call	%g2
	 mov	%g3,%o0
	call	_exit,0
	 nop
	.size __thread_start, .-__thread_start

weak_alias (__clone, clone)
