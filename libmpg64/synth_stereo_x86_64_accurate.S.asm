#line 1 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/synth_stereo_x86_64_accurate.S"








#line 1 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"












#line 1 "C:/CProjects/mpg123/build264/src\\config.h"



































































































































































#line 165 "C:/CProjects/mpg123/build264/src\\config.h"











#line 14 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"
#line 1 "C:/CProjects/mpg123/ports/cmake/../../src\\intsym.h"




#line 1 "C:/CProjects/mpg123/build264/src\\config.h"



































































































































































#line 165 "C:/CProjects/mpg123/build264/src\\config.h"











#line 6 "C:/CProjects/mpg123/ports/cmake/../../src\\intsym.h"









































































































































































































































































































































































#line 368 "C:/CProjects/mpg123/ports/cmake/../../src\\intsym.h"
#line 15 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"



#line 22 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"





#line 28 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"











































#line 72 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"







#line 80 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"


#line 84 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"


#line 87 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"




























































#line 148 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"



#line 152 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"

#line 154 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"





#line 160 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"






#line 167 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"









#line 177 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"

#line 179 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"



#line 183 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"








#line 192 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"









#line 202 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"

#line 204 "c:\\cprojects\\mpg123\\src\\libmpg123\\mangle.h"

#line 10 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/synth_stereo_x86_64_accurate.S"



















#line 30 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/synth_stereo_x86_64_accurate.S"











	.section	.rodata


#line 45 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/synth_stereo_x86_64_accurate.S"
	.balign 32
maxmin_s16:
	.long   1191181824
	.long   1191181824
	.long   1191181824
	.long   1191181824
	.long   -956301312
	.long   -956301312
	.long   -956301312
	.long   -956301312
	.text
	.balign 16
.globl INT123_synth_1to1_s_x86_64_accurate_asm
INT123_synth_1to1_s_x86_64_accurate_asm:

	movl		40(%rsp), %eax 
	pushq		%rsi
	subq		$176, %rsp 
	movaps		%xmm6, 16(%rsp)
	movaps		%xmm7, 32(%rsp)
	movaps		%xmm8, 48(%rsp)
	movaps		%xmm9, 64(%rsp)
	movaps		%xmm10, 80(%rsp)
	movaps		%xmm11, 96(%rsp)
	movaps		%xmm12, 112(%rsp)
	movaps		%xmm13, 128(%rsp)
	movaps		%xmm14, 144(%rsp)
	movaps		%xmm15, 160(%rsp)


#line 76 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/synth_stereo_x86_64_accurate.S"
	
	leaq		maxmin_s16(%rip), %r10
	leaq		16(%r10), %r11
	xorps		%xmm0, %xmm0
	movaps		%xmm0, (%rsp)
	

	shlq		$32, %rax
	shrq		$30, %rax
	movq		%rcx, %rsi





#line 92 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/synth_stereo_x86_64_accurate.S"
	leaq		64(%rsi), %rsi
	subq		%rax, %rsi

	movl		$4, %ecx
	
	.balign 16
1:
	movups		(%rsi), %xmm8
	movups		16(%rsi), %xmm1
	movups		32(%rsi), %xmm2
	movups		48(%rsi), %xmm3
	movups		128(%rsi), %xmm9
	movups		144(%rsi), %xmm5
	movups		160(%rsi), %xmm6
	movups		176(%rsi), %xmm7
	movaps		%xmm8, %xmm0
	movaps		%xmm1, %xmm4
	movaps		%xmm2, %xmm10
	movaps		%xmm3, %xmm11
	movaps		%xmm9, %xmm12
	movaps		%xmm5, %xmm13
	movaps		%xmm6, %xmm14
	movaps		%xmm7, %xmm15
	mulps		(%rdx), %xmm8
	mulps		16(%rdx), %xmm1
	mulps		32(%rdx), %xmm2
	mulps		48(%rdx), %xmm3
	mulps		64(%rdx), %xmm9
	mulps		80(%rdx), %xmm5
	mulps		96(%rdx), %xmm6
	mulps		112(%rdx), %xmm7
	mulps		(%r8), %xmm0
	mulps		16(%r8), %xmm4
	mulps		32(%r8), %xmm10
	mulps		48(%r8), %xmm11
	mulps		64(%r8), %xmm12
	mulps		80(%r8), %xmm13
	mulps		96(%r8), %xmm14
	mulps		112(%r8), %xmm15
	
	addps		%xmm1, %xmm8
	addps		%xmm2, %xmm3
	addps		%xmm4, %xmm0
	addps		%xmm11, %xmm10
	addps		%xmm5, %xmm9
	addps		%xmm7, %xmm6
	addps		%xmm13, %xmm12
	addps		%xmm15, %xmm14
	addps		%xmm3, %xmm8
	addps		%xmm6, %xmm9
	addps		%xmm10, %xmm0
	addps		%xmm12, %xmm14
	movaps		%xmm0, %xmm12
	movaps		%xmm14, %xmm13
	leaq		256(%rsi), %rsi
	leaq		128(%rdx), %rdx
	leaq		128(%r8), %r8
	
	movups		(%rsi), %xmm10
	movups		16(%rsi), %xmm1
	movups		32(%rsi), %xmm2
	movups		48(%rsi), %xmm3
	movups		128(%rsi), %xmm11
	movups		144(%rsi), %xmm5
	movups		160(%rsi), %xmm6
	movups		176(%rsi), %xmm7
	movaps		%xmm10, %xmm0
	movaps		%xmm1, %xmm4
	movaps		%xmm2, %xmm14
	movaps		%xmm3, %xmm15
	mulps		(%rdx), %xmm10
	mulps		16(%rdx), %xmm1
	mulps		32(%rdx), %xmm2
	mulps		48(%rdx), %xmm3
	mulps		(%r8), %xmm0
	mulps		16(%r8), %xmm4
	mulps		32(%r8), %xmm14
	mulps		48(%r8), %xmm15
	addps		%xmm1, %xmm10
	addps		%xmm2, %xmm3
	addps		%xmm4, %xmm0
	addps		%xmm15, %xmm14
	movaps		%xmm11, %xmm1
	movaps		%xmm5, %xmm2
	movaps		%xmm6, %xmm4
	movaps		%xmm7, %xmm15
	mulps		64(%rdx), %xmm11
	mulps		80(%rdx), %xmm5
	mulps		96(%rdx), %xmm6
	mulps		112(%rdx), %xmm7
	mulps		64(%r8), %xmm1
	mulps		80(%r8), %xmm2
	mulps		96(%r8), %xmm4
	mulps		112(%r8), %xmm15
	addps		%xmm5, %xmm11
	addps		%xmm7, %xmm6
	addps		%xmm2, %xmm1
	addps		%xmm15, %xmm4
	
	addps		%xmm3, %xmm10
	addps		%xmm6, %xmm11
	addps		%xmm0, %xmm14
	addps		%xmm4, %xmm1
	movaps		%xmm1, %xmm15
	leaq		256(%rsi), %rsi
	leaq		128(%rdx), %rdx
	leaq		128(%r8), %r8
	
	movaps		%xmm8, %xmm0
	movaps		%xmm10, %xmm1
	movaps		%xmm12, %xmm4
	movaps		%xmm14, %xmm5
	unpcklps	%xmm9, %xmm8
	unpcklps	%xmm11, %xmm10
	unpckhps	%xmm9, %xmm0
	unpckhps	%xmm11, %xmm1
	unpcklps	%xmm13, %xmm12
	unpcklps	%xmm15, %xmm14
	unpckhps	%xmm13, %xmm4
	unpckhps	%xmm15, %xmm5
	movaps		%xmm8, %xmm2
	movaps		%xmm0, %xmm3
	movaps		%xmm12, %xmm6
	movaps		%xmm4, %xmm7
	movlhps		%xmm10, %xmm8
	movhlps		%xmm2, %xmm10
	movlhps		%xmm1, %xmm0
	movhlps		%xmm3, %xmm1
	movlhps		%xmm14, %xmm12
	movhlps		%xmm6, %xmm14
	movlhps		%xmm5, %xmm4
	movhlps		%xmm7, %xmm5
	subps		%xmm10, %xmm8
	subps		%xmm1, %xmm0
	subps		%xmm14, %xmm12
	subps		%xmm5, %xmm4
	addps		%xmm8, %xmm0
	addps		%xmm12, %xmm4
	
	movaps		%xmm0, %xmm2
	movaps		%xmm0, %xmm3
	movaps		%xmm4, %xmm5
	movaps		%xmm4, %xmm6
	cmpnleps	(%r10), %xmm2
	cmpltps		(%r11), %xmm3
	cmpnleps	(%r10), %xmm5
	cmpltps		(%r11), %xmm6
	cvtps2dq	%xmm0, %xmm0
	cvtps2dq	%xmm4, %xmm4
	movaps		%xmm0, %xmm1
	unpcklps	%xmm4, %xmm0
	unpckhps	%xmm4, %xmm1
	packssdw	%xmm1, %xmm0
	movups		%xmm0, (%r9)
	
	packssdw	%xmm5, %xmm2
	packssdw	%xmm6, %xmm3
	psrlw		$15, %xmm2
	psrlw		$15, %xmm3
	paddw		%xmm3, %xmm2
	paddw		(%rsp), %xmm2
	movaps		%xmm2, (%rsp)
	
	leaq		16(%r9), %r9
	decl		%ecx
	jnz			1b
	
	movl		$4, %ecx
	
	.balign 16
1:
	movups		(%rsi), %xmm8
	movups		16(%rsi), %xmm1
	movups		32(%rsi), %xmm2
	movups		48(%rsi), %xmm3
	movups		128(%rsi), %xmm9
	movups		144(%rsi), %xmm5
	movups		160(%rsi), %xmm6
	movups		176(%rsi), %xmm7
	movaps		%xmm8, %xmm0
	movaps		%xmm1, %xmm4
	movaps		%xmm2, %xmm10
	movaps		%xmm3, %xmm11
	movaps		%xmm9, %xmm12
	movaps		%xmm5, %xmm13
	movaps		%xmm6, %xmm14
	movaps		%xmm7, %xmm15
	mulps		(%rdx), %xmm8
	mulps		16(%rdx), %xmm1
	mulps		32(%rdx), %xmm2
	mulps		48(%rdx), %xmm3
	mulps		-64(%rdx), %xmm9
	mulps		-48(%rdx), %xmm5
	mulps		-32(%rdx), %xmm6
	mulps		-16(%rdx), %xmm7
	mulps		(%r8), %xmm0
	mulps		16(%r8), %xmm4
	mulps		32(%r8), %xmm10
	mulps		48(%r8), %xmm11
	mulps		-64(%r8), %xmm12
	mulps		-48(%r8), %xmm13
	mulps		-32(%r8), %xmm14
	mulps		-16(%r8), %xmm15
	
	addps		%xmm1, %xmm8
	addps		%xmm2, %xmm3
	addps		%xmm4, %xmm0
	addps		%xmm11, %xmm10
	addps		%xmm5, %xmm9
	addps		%xmm7, %xmm6
	addps		%xmm13, %xmm12
	addps		%xmm15, %xmm14
	addps		%xmm3, %xmm8
	addps		%xmm6, %xmm9
	addps		%xmm10, %xmm0
	addps		%xmm12, %xmm14
	movaps		%xmm0, %xmm12
	movaps		%xmm14, %xmm13
	leaq		256(%rsi), %rsi
	leaq		-128(%rdx), %rdx
	leaq		-128(%r8), %r8
	
	movups		(%rsi), %xmm10
	movups		16(%rsi), %xmm1
	movups		32(%rsi), %xmm2
	movups		48(%rsi), %xmm3
	movups		128(%rsi), %xmm11
	movups		144(%rsi), %xmm5
	movups		160(%rsi), %xmm6
	movups		176(%rsi), %xmm7
	movaps		%xmm10, %xmm0
	movaps		%xmm1, %xmm4
	movaps		%xmm2, %xmm14
	movaps		%xmm3, %xmm15
	mulps		(%rdx), %xmm10
	mulps		16(%rdx), %xmm1
	mulps		32(%rdx), %xmm2
	mulps		48(%rdx), %xmm3
	mulps		(%r8), %xmm0
	mulps		16(%r8), %xmm4
	mulps		32(%r8), %xmm14
	mulps		48(%r8), %xmm15
	addps		%xmm1, %xmm10
	addps		%xmm2, %xmm3
	addps		%xmm4, %xmm0
	addps		%xmm15, %xmm14
	movaps		%xmm11, %xmm1
	movaps		%xmm5, %xmm2
	movaps		%xmm6, %xmm4
	movaps		%xmm7, %xmm15
	mulps		-64(%rdx), %xmm11
	mulps		-48(%rdx), %xmm5
	mulps		-32(%rdx), %xmm6
	mulps		-16(%rdx), %xmm7
	mulps		-64(%r8), %xmm1
	mulps		-48(%r8), %xmm2
	mulps		-32(%r8), %xmm4
	mulps		-16(%r8), %xmm15
	addps		%xmm5, %xmm11
	addps		%xmm7, %xmm6
	addps		%xmm2, %xmm1
	addps		%xmm15, %xmm4
	
	addps		%xmm3, %xmm10
	addps		%xmm6, %xmm11
	addps		%xmm0, %xmm14
	addps		%xmm4, %xmm1
	movaps		%xmm1, %xmm15
	leaq		256(%rsi), %rsi
	leaq		-128(%rdx), %rdx
	leaq		-128(%r8), %r8
	
	movaps		%xmm8, %xmm0
	movaps		%xmm10, %xmm1
	movaps		%xmm12, %xmm4
	movaps		%xmm14, %xmm5
	unpcklps	%xmm9, %xmm8
	unpcklps	%xmm11, %xmm10
	unpckhps	%xmm9, %xmm0
	unpckhps	%xmm11, %xmm1
	unpcklps	%xmm13, %xmm12
	unpcklps	%xmm15, %xmm14
	unpckhps	%xmm13, %xmm4
	unpckhps	%xmm15, %xmm5
	movaps		%xmm8, %xmm2
	movaps		%xmm0, %xmm3
	movaps		%xmm12, %xmm6
	movaps		%xmm4, %xmm7
	movlhps		%xmm10, %xmm8
	movhlps		%xmm2, %xmm10
	movlhps		%xmm1, %xmm0
	movhlps		%xmm3, %xmm1
	movlhps		%xmm14, %xmm12
	movhlps		%xmm6, %xmm14
	movlhps		%xmm5, %xmm4
	movhlps		%xmm7, %xmm5
	addps		%xmm10, %xmm8
	addps		%xmm1, %xmm0
	addps		%xmm14, %xmm12
	addps		%xmm5, %xmm4
	addps		%xmm8, %xmm0
	addps		%xmm12, %xmm4
	
	movaps		%xmm0, %xmm2
	movaps		%xmm0, %xmm3
	movaps		%xmm4, %xmm5
	movaps		%xmm4, %xmm6
	cmpnleps	(%r10), %xmm2
	cmpltps		(%r11), %xmm3
	cmpnleps	(%r10), %xmm5
	cmpltps		(%r11), %xmm6
	cvtps2dq	%xmm0, %xmm0
	cvtps2dq	%xmm4, %xmm4
	movaps		%xmm0, %xmm1
	unpcklps	%xmm4, %xmm0
	unpckhps	%xmm4, %xmm1
	packssdw	%xmm1, %xmm0
	movups		%xmm0, (%r9)
	
	packssdw	%xmm5, %xmm2
	packssdw	%xmm6, %xmm3
	psrlw		$15, %xmm2
	psrlw		$15, %xmm3
	paddw		%xmm3, %xmm2
	paddw		(%rsp), %xmm2
	movaps		%xmm2, (%rsp)
	
	leaq		16(%r9), %r9
	decl		%ecx
	jnz			1b
	
	movaps		(%rsp), %xmm4
	movhlps		%xmm4, %xmm0
	paddw		%xmm4, %xmm0
	pshuflw		$0x55, %xmm0, %xmm1
	pshuflw		$0xaa, %xmm0, %xmm2
	pshuflw		$0xff, %xmm0, %xmm3
	paddw		%xmm1, %xmm0
	paddw		%xmm2, %xmm0
	paddw		%xmm3, %xmm0
	
	movd		%xmm0, %eax
	andl		$0xffff, %eax
	

	movaps		16(%rsp), %xmm6
	movaps		32(%rsp), %xmm7
	movaps		48(%rsp), %xmm8
	movaps		64(%rsp), %xmm9
	movaps		80(%rsp), %xmm10
	movaps		96(%rsp), %xmm11
	movaps		112(%rsp), %xmm12
	movaps		128(%rsp), %xmm13
	movaps		144(%rsp), %xmm14
	movaps		160(%rsp), %xmm15
	addq		$176, %rsp
	popq		%rsi


#line 452 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/synth_stereo_x86_64_accurate.S"
	ret


