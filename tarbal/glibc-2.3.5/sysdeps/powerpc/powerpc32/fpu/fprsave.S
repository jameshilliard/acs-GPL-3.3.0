/* Copyright (C) 2000, 2001 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

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

/*
    Floating Point Registers (FPRs) save routine
*/

#include <sysdep.h>

ENTRY(_savefpr_all)
                ASM_GLOBAL_DIRECTIVE C_TEXT(_savef14)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savefpr_14)
C_TEXT(_savef14):
C_TEXT(_savefpr_14):	stfd	fp14,-144(r1)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savef15)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savefpr_15)
C_TEXT(_savef15):
C_TEXT(_savefpr_15):	stfd	fp15,-136(r1)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savef16)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savefpr_16)
C_TEXT(_savef16):
C_TEXT(_savefpr_16):	stfd	fp16,-128(r1)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savef17)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savefpr_17)
C_TEXT(_savef17):
C_TEXT(_savefpr_17):	stfd	fp17,-120(r1)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savef18)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savefpr_18)
C_TEXT(_savef18):
C_TEXT(_savefpr_18):	stfd	fp18,-112(r1)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savef19)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savefpr_19)
C_TEXT(_savef19):
C_TEXT(_savefpr_19):	stfd	fp19,-104(r1)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savef20)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savefpr_20)
C_TEXT(_savef20):
C_TEXT(_savefpr_20):	stfd	fp20,-96(r1)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savef21)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savefpr_21)
C_TEXT(_savef21):
C_TEXT(_savefpr_21):	stfd	fp21,-88(r1)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savef22)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savefpr_22)
C_TEXT(_savef22):
C_TEXT(_savefpr_22):	stfd	fp22,-80(r1)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savef23)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savefpr_23)
C_TEXT(_savef23):
C_TEXT(_savefpr_23):	stfd	fp23,-72(r1)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savef24)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savefpr_24)
C_TEXT(_savef24):
C_TEXT(_savefpr_24):	stfd	fp24,-64(r1)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savef25)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savefpr_25)
C_TEXT(_savef25):
C_TEXT(_savefpr_25):	stfd	fp25,-56(r1)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savef26)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savefpr_26)
C_TEXT(_savef26):
C_TEXT(_savefpr_26):	stfd	fp26,-48(r1)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savef27)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savefpr_27)
C_TEXT(_savef27):
C_TEXT(_savefpr_27):	stfd	fp27,-40(r1)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savef28)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savefpr_28)
C_TEXT(_savef28):
C_TEXT(_savefpr_28):	stfd	fp28,-32(r1)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savef29)
		ASM_GLOBAL_DIRECTIVE C_TEXT(_savefpr_29)
C_TEXT(_savef29):
C_TEXT(_savefpr_29):	stfd	fp29,-24(r1)	#save f29
			stfd	fp30,-16(r1)	#save f30
			stfd	fp31,-8(r1)	#save f31
			stw	r0,8(r1)	#save LR in callers frame
			blr			#return
