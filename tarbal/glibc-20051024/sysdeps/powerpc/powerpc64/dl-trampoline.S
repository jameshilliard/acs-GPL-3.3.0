/* PLT trampolines.  PPC64 version.
   Copyright (C) 2005 Free Software Foundation, Inc.
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

#include <sysdep.h>
#include <rtld-global-offsets.h>


	.section ".text"
/* On entry r0 contains the index of the PLT entry we need to fixup
   and r11 contains the link_map (from PLT0+16).  The link_map becomes
   parm1 (r3) and the index (r0) need to be converted to an offset
   (index * 24) in parm2 (r4).  */

EALIGN(_dl_runtime_resolve, 4, 0)
/* We need to save the registers used to pass parameters, ie. r3 thru
   r10; the registers are saved in a stack frame.  */
	stdu	r1,-128(r1)
	std	r3,48(r1)
	mr	r3,r11
	std	r4,56(r1)
	sldi	r4,r0,1
	std	r5,64(r1)
	add	r4,r4,r0
	std	r6,72(r1)
	sldi	r4,r4,3
	std	r7,80(r1)
	mflr	r0
	std	r8,88(r1)
/* Store the LR in the LR Save area of the previous frame.  */
	std	r0,128+16(r1)
	mfcr	r0
	std	r9,96(r1)
	std	r10,104(r1)
/* I'm almost certain we don't have to save cr...  be safe.  */
	std	r0,8(r1)
	bl	JUMPTARGET(_dl_fixup)
/* Put the registers back.  */
	ld	r0,128+16(r1)
	ld	r10,104(r1)
	ld	r9,96(r1)
	ld	r8,88(r1)
	ld	r7,80(r1)
	mtlr	r0
	ld	r0,8(r1)
	ld	r6,72(r1)
	ld	r5,64(r1)
	ld	r4,56(r1)
	mtcrf	0xFF,r0
/* Load the target address, toc and static chain reg from the function
   descriptor returned by fixup.  */
	ld	r0,0(r3)
	ld	r2,8(r3)
	mtctr	r0
	ld	r11,16(r3)
	ld	r3,48(r1)
/* Unwind the stack frame, and jump.  */
	addi	r1,r1,128
	bctr
END(_dl_runtime_resolve)

	/* Stack layout:
	  +592   previous backchain
	  +584   spill_r31
	  +576   spill_r30
	  +560   v1
	  +552   fp4
	  +544   fp3
	  +536   fp2
	  +528   fp1
	  +520   r4
	  +512   r3
	   return values
          +504   free
	  +496   stackframe
	  +488   lr
	  +480   r1
	  +464   v13
	  +448   v12
	  +432   v11
	  +416   v10
	  +400   v9
	  +384   v8
	  +368   v7
	  +352   v6
	  +336   v5
	  +320   v4
	  +304   v3
	  +288   v2
	 * VMX Parms in V2-V13, V0-V1 are scratch
	  +284   vrsave
	  +280   free
	  +272   fp13
	  +264   fp12
	  +256   fp11
	  +248   fp10
	  +240   fp9
	  +232   fp8
	  +224   fp7
	  +216   fp6
	  +208   fp5
	  +200   fp4
	  +192   fp3
	  +184   fp2
	  +176   fp1
	 * FP Parms in FP1-FP13, FP0 is a scratch register
	  +168   r10
	  +160   r9
	  +152   r8
	  +144   r7
	  +136   r6
	  +128   r5
	  +120   r4
	  +112   r3
	 * Integer parms in R3-R10, R0 is scratch, R1 SP, R2 is TOC
	  +104   parm8
	  +96    parm7
	  +88    parm6
	  +80    parm5
	  +72    parm4
	  +64    parm3
	  +56    parm2
	  +48    parm1
	 * Parameter save area, Allocated by the call, at least 8 double words
	  +40    TOC save area
	  +32    Reserved for linker
	  +24    Reserved for compiler
	  +16    LR save area
	  +8     CR save area
	r1+0     stack back chain
	*/
#define FRAME_SIZE 592
#define INT_RTN 512
#define FPR_RTN 528
#define VR_RTN 560
#define STACK_FRAME 496
#define CALLING_LR 488
#define CALLING_SP 480
#define INT_PARMS 112
#define FPR_PARMS 176
#define VR_PARMS 288
#define VR_VRSAVE 284
	.section	".toc","aw"
.LC__dl_hwcap:
# ifdef SHARED
	.tc _rtld_global_ro[TC],_rtld_global_ro
# else
	.tc _dl_hwcap[TC],_dl_hwcap
# endif
	.section ".text"

	.machine	"altivec"
/* On entry r0 contains the index of the PLT entry we need to fixup
   and r11 contains the link_map (from PLT0+16).  The link_map becomes
   parm1 (r3) and the index (r0) needs to be converted to an offset
   (index * 24) in parm2 (r4).  */
#ifndef PROF
EALIGN(_dl_profile_resolve, 4, 0)
/* Spill r30, r31 to preserve the link_map* and reloc_addr, in case we
   need to call _dl_call_pltexit.  */
	std	r31,-8(r1)
	std	r30,-16(r1)
/* We need to save the registers used to pass parameters, ie. r3 thru
   r10; the registers are saved in a stack frame.  */
	stdu	r1,-FRAME_SIZE(r1)
	std	r3,INT_PARMS+0(r1)
	mr	r3,r11
	std	r4,INT_PARMS+8(r1)
	sldi	r4,r0,1		/* index * 2 */
	std	r5,INT_PARMS+16(r1)
	add	r4,r4,r0	/* index * 3 */
	std	r6,INT_PARMS+24(r1)
	sldi	r4,r4,3		/* index * 24  == PLT offset */
	mflr	r5
	std	r7,INT_PARMS+32(r1)
	std	r8,INT_PARMS+40(r1)
/* Store the LR in the LR Save area of the previous frame.  */
/* XXX Do we have to do this?  */
	la	r8,FRAME_SIZE(r1)
	std	r5,FRAME_SIZE+16(r1)
	std	r5,CALLING_LR(r1)
	mfcr	r0
	std	r9,INT_PARMS+48(r1)
	std	r10,INT_PARMS+56(r1)
	std	r8,CALLING_SP(r1)
/* I'm almost certain we don't have to save cr...  be safe.  */
	std	r0,8(r1)
	ld	r12,.LC__dl_hwcap@toc(r2)
#ifdef SHARED
	/* Load _rtld-global._dl_hwcap.  */
	ld	r12,RTLD_GLOBAL_RO_DL_HWCAP_OFFSET(r12)
#else
	ld	r12,0(r12) /* Load extern _dl_hwcap.  */
#endif
	andis.  r0,r12,(PPC_FEATURE_HAS_ALTIVEC >> 16)
	beq	L(saveFP)
	la	r10,(VR_PARMS+0)(r1)
	la	r9,(VR_PARMS+16)(r1)
	li	r11,32
	li	r12,64
	stvx	v2,0,r10
	stvx	v3,0,r9

	stvx	v4,r11,r10
	stvx	v5,r11,r9
	addi	r11,r11,64

	stvx	v6,r12,r10
	stvx	v7,r12,r9
	addi	r12,r12,64

	stvx	v8,r11,r10
	stvx	v9,r11,r9
	addi	r11,r11,64

	stvx	v10,r12,r10
	stvx	v11,r12,r9
	mfspr	r0,VRSAVE

	stvx	v12,r11,r10
	stvx	v13,r11,r9
L(saveFP):
	stw	r0,VR_VRSAVE(r1)
/* Save floating registers.  */
	stfd	fp1,FPR_PARMS+0(r1)
	stfd	fp2,FPR_PARMS+8(r1)
	stfd	fp3,FPR_PARMS+16(r1)
	stfd	fp4,FPR_PARMS+24(r1)
	stfd	fp5,FPR_PARMS+32(r1)
	stfd	fp6,FPR_PARMS+40(r1)
	stfd	fp7,FPR_PARMS+48(r1)
	stfd	fp8,FPR_PARMS+56(r1)
	stfd	fp9,FPR_PARMS+64(r1)
	stfd	fp10,FPR_PARMS+72(r1)
	stfd	fp11,FPR_PARMS+80(r1)
	li	r0,-1
	stfd	fp12,FPR_PARMS+88(r1)
	stfd	fp13,FPR_PARMS+96(r1)
/* Load the extra parameters.  */
	addi	r6,r1,INT_PARMS
	addi	r7,r1,STACK_FRAME
/* Save  link_map* and reloc_addr parms for later.  */
	mr	r31,r3
	mr	r30,r4
	std	r0,0(r7)
	bl	JUMPTARGET(_dl_profile_fixup)
	nop
/* Test *framesizep > 0 to see if need to do pltexit processing.  */
	ld	r0,STACK_FRAME(r1)
/* Put the registers back.  */
	lwz	r12,VR_VRSAVE(r1)
	cmpdi	cr1,r0,0
	cmpdi	cr0,r12,0
	bgt	cr1,L(do_pltexit)
	la	r10,(VR_PARMS+0)(r1)
	la	r9,(VR_PARMS+16)(r1)
/* VRSAVE must be non-zero if VMX is present and VRs are in use. */
	beq	L(restoreFXR)
	li	r11,32
	li	r12,64
	lvx	v2,0,r10
	lvx	v3,0,r9

	lvx	v4,r11,r10
	lvx	v5,r11,r9
	addi	r11,r11,64

	lvx	v6,r12,r10
	lvx	v7,r12,r9
	addi	r12,r12,64

	lvx	v8,r11,r10
	lvx	v9,r11,r9
	addi	r11,r11,64

	lvx	v10,r12,r10
	lvx	v11,r12,r9

	lvx	v12,r11,r10
	lvx	v13,r11,r9
L(restoreFXR):
	ld	r0,FRAME_SIZE+16(r1)
	ld	r10,INT_PARMS+56(r1)
	ld	r9,INT_PARMS+48(r1)
	ld	r8,INT_PARMS+40(r1)
	ld	r7,INT_PARMS+32(r1)
	mtlr	r0
	ld	r0,8(r1)
	ld	r6,INT_PARMS+24(r1)
	ld	r5,INT_PARMS+16(r1)
	ld	r4,INT_PARMS+8(r1)
	mtcrf	0xFF,r0
/* Load the target address, toc and static chain reg from the function
   descriptor returned by fixup.  */
	ld	r0,0(r3)
	ld	r2,8(r3)
	ld	r11,16(r3)
	ld	r3,INT_PARMS+0(r1)
	mtctr	r0
/* Load the floating point registers.  */
	lfd	fp1,FPR_PARMS+0(r1)
	lfd	fp2,FPR_PARMS+8(r1)
	lfd	fp3,FPR_PARMS+16(r1)
	lfd	fp4,FPR_PARMS+24(r1)
	lfd	fp5,FPR_PARMS+32(r1)
	lfd	fp6,FPR_PARMS+40(r1)
	lfd	fp7,FPR_PARMS+48(r1)
	lfd	fp8,FPR_PARMS+56(r1)
	lfd	fp9,FPR_PARMS+64(r1)
	lfd	fp10,FPR_PARMS+72(r1)
	lfd	fp11,FPR_PARMS+80(r1)
	lfd	fp12,FPR_PARMS+88(r1)
	lfd	fp13,FPR_PARMS+96(r1)
/* Unwind the stack frame, and jump.  */
	ld	r31,584(r1)
	ld	r30,576(r1)
	addi	r1,r1,FRAME_SIZE
	bctr
L(do_pltexit):
	la	r10,(VR_PARMS+0)(r1)
	la	r9,(VR_PARMS+16)(r1)
	beq	L(restoreFXR2)
	li	r11,32
	li	r12,64
	lvx	v2,0,r10
	lvx	v3,0,r9

	lvx	v4,r11,r10
	lvx	v5,r11,r9
	addi	r11,r11,64

	lvx	v6,r12,r10
	lvx	v7,r12,r9
	addi	r12,r12,64

	lvx	v8,r11,r10
	lvx	v9,r11,r9
	addi	r11,r11,64

	lvx	v10,r12,r10
	lvx	v11,r12,r9

	lvx	v12,r11,r10
	lvx	v13,r11,r9
L(restoreFXR2):
	ld	r0,FRAME_SIZE+16(r1)
	ld	r10,INT_PARMS+56(r1)
	ld	r9,INT_PARMS+48(r1)
	ld	r8,INT_PARMS+40(r1)
	ld	r7,INT_PARMS+32(r1)
	mtlr	r0
	ld	r0,8(r1)
	ld	r6,INT_PARMS+24(r1)
	ld	r5,INT_PARMS+16(r1)
	ld	r4,INT_PARMS+8(r1)
	mtcrf	0xFF,r0
/* Load the target address, toc and static chain reg from the function
   descriptor returned by fixup.  */
	ld	r0,0(r3)
	std	r2,40(r1)
	ld	r2,8(r3)
	ld	r11,16(r3)
	ld	r3,INT_PARMS+0(r1)
	mtctr	r0
/* Load the floating point registers.  */
	lfd	fp1,FPR_PARMS+0(r1)
	lfd	fp2,FPR_PARMS+8(r1)
	lfd	fp3,FPR_PARMS+16(r1)
	lfd	fp4,FPR_PARMS+24(r1)
	lfd	fp5,FPR_PARMS+32(r1)
	lfd	fp6,FPR_PARMS+40(r1)
	lfd	fp7,FPR_PARMS+48(r1)
	lfd	fp8,FPR_PARMS+56(r1)
	lfd	fp9,FPR_PARMS+64(r1)
	lfd	fp10,FPR_PARMS+72(r1)
	lfd	fp11,FPR_PARMS+80(r1)
	lfd	fp12,FPR_PARMS+88(r1)
	lfd	fp13,FPR_PARMS+96(r1)
/* Call the target function.  */
	bctrl
	ld	r2,40(r1)
	lwz	r12,VR_VRSAVE(r1)
/* But return here and store the return values.  */
	std	r3,INT_RTN(r1)
	std	r4,INT_RTN+8(r1)
	stfd	fp1,FPR_PARMS+0(r1)
	stfd	fp2,FPR_PARMS+8(r1)
	cmpdi	cr0,r12,0
	la	r10,VR_RTN(r1)
	stfd	fp3,FPR_PARMS+16(r1)
	stfd	fp4,FPR_PARMS+24(r1)
	mr	r3,r31
	mr	r4,r30
	beq	L(callpltexit)
	stvx	v2,0,r10
L(callpltexit):
	addi	r5,r1,INT_PARMS
	addi	r6,r1,INT_RTN
	bl	JUMPTARGET(_dl_call_pltexit)
	nop
/* Restore the return values from target function.  */
	lwz	r12,VR_VRSAVE(r1)
	ld	r3,INT_RTN(r1)
	ld	r4,INT_RTN+8(r1)
	lfd	fp1,FPR_PARMS+0(r1)
	lfd	fp2,FPR_PARMS+8(r1)
	cmpdi	cr0,r12,0
	la	r10,VR_RTN(r1)
	lfd	fp3,FPR_PARMS+16(r1)
	lfd	fp4,FPR_PARMS+24(r1)
	beq	L(pltexitreturn)
	lvx	v2,0,r10
L(pltexitreturn):
	ld	r0,FRAME_SIZE+16(r1)
	ld	r31,584(r1)
	ld	r30,576(r1)
	mtlr	r0
	ld	r1,0(r1)
	blr
END(_dl_profile_resolve)
#endif
