/* Copyright (C) 2001 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by David Mosberger-Tang <davidm@hpl.hp.com>.

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

/* Constants shared between setcontext() and getcontext().  Don't
   install this header file.  */

#define SIG_BLOCK	0
#define SIG_UNBLOCK	1
#define SIG_SETMASK	2

#define SC_FLGS 0x000
#define SC_LINK 0x004
#define SC_STCK 0x008
#define SC_STSZ 0x010
#define SC_PSW  0x018
#define SC_GPRS 0x020
#define SC_ACRS 0x060
#define SC_FPC  0x0A0
#define SC_FPRS 0x0A8
#define SC_MASK 0x128

/* Tests run in stdlib/tst-ucontext-off.  */
#define TESTS \
  TEST (uc_flags, SC_FLGS);				\
  TEST (uc_link, SC_LINK);				\
  TEST (uc_stack.ss_sp, SC_STCK);			\
  TEST (uc_stack.ss_size, SC_STSZ);			\
  TEST (uc_mcontext.psw, SC_PSW);			\
  TEST (uc_mcontext.gregs, SC_GPRS);			\
  TEST (uc_mcontext.aregs, SC_ACRS);			\
  TEST (uc_mcontext.fpregs.fpc, SC_FPC);		\
  TEST (uc_mcontext.fpregs.fprs, SC_FPRS);		\
  TEST (uc_sigmask, SC_MASK);
