#line 1 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/synth_stereo_x86_64_float.S"








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

#line 10 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/synth_stereo_x86_64_float.S"



















#line 30 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/synth_stereo_x86_64_float.S"









	.section	.rodata


#line 43 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/synth_stereo_x86_64_float.S"
	.balign 32
scale_x86_64:
	.long   939524096
	.long   939524096
	.long   939524096
	.long   939524096
	.text
	.balign 16
.globl INT123_synth_1to1_real_s_x86_64_asm
INT123_synth_1to1_real_s_x86_64_asm:

	movl		40(%rsp), %eax 
	subq		$168, %rsp 
	movaps		%xmm6, (%rsp)
	movaps		%xmm7, 16(%rsp)
	movaps		%xmm8, 32(%rsp)
	movaps		%xmm9, 48(%rsp)
	movaps		%xmm10, 64(%rsp)
	movaps		%xmm11, 80(%rsp)
	movaps		%xmm12, 96(%rsp)
	movaps		%xmm13, 112(%rsp)
	movaps		%xmm14, 128(%rsp)
	movaps		%xmm15, 144(%rsp)
#line 67 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/synth_stereo_x86_64_float.S"

	leaq		scale_x86_64(%rip), %r11
	

	shlq		$32, %rax
	shrq		$30, %rax
	movq		%rcx, %r10





#line 80 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/synth_stereo_x86_64_float.S"
	leaq		64(%r10), %r10
	subq		%rax, %r10

	movl		$4, %ecx
	
	.balign 16
1:
	movups		(%r10), %xmm8
	movups		16(%r10), %xmm1
	movups		32(%r10), %xmm2
	movups		48(%r10), %xmm3
	movups		128(%r10), %xmm9
	movups		144(%r10), %xmm5
	movups		160(%r10), %xmm6
	movups		176(%r10), %xmm7
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
	leaq		256(%r10), %r10
	leaq		128(%rdx), %rdx
	leaq		128(%r8), %r8
	
	movups		(%r10), %xmm10
	movups		16(%r10), %xmm1
	movups		32(%r10), %xmm2
	movups		48(%r10), %xmm3
	movups		128(%r10), %xmm11
	movups		144(%r10), %xmm5
	movups		160(%r10), %xmm6
	movups		176(%r10), %xmm7
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
	leaq		256(%r10), %r10
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
	
	mulps		(%r11), %xmm0
	mulps		(%r11), %xmm4
	movaps		%xmm0, %xmm1
	unpcklps	%xmm4, %xmm0
	unpckhps	%xmm4, %xmm1
	movups		%xmm0, (%r9)
	movups		%xmm1, 16(%r9)
	
	leaq		32(%r9), %r9
	decl		%ecx
	jnz			1b
	
	movl		$4, %ecx
	
	.balign 16
1:
	movups		(%r10), %xmm8
	movups		16(%r10), %xmm1
	movups		32(%r10), %xmm2
	movups		48(%r10), %xmm3
	movups		128(%r10), %xmm9
	movups		144(%r10), %xmm5
	movups		160(%r10), %xmm6
	movups		176(%r10), %xmm7
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
	leaq		256(%r10), %r10
	leaq		-128(%rdx), %rdx
	leaq		-128(%r8), %r8
	
	movups		(%r10), %xmm10
	movups		16(%r10), %xmm1
	movups		32(%r10), %xmm2
	movups		48(%r10), %xmm3
	movups		128(%r10), %xmm11
	movups		144(%r10), %xmm5
	movups		160(%r10), %xmm6
	movups		176(%r10), %xmm7
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
	leaq		256(%r10), %r10
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
	
	mulps		(%r11), %xmm0
	mulps		(%r11), %xmm4
	movaps		%xmm0, %xmm1
	unpcklps	%xmm4, %xmm0
	unpckhps	%xmm4, %xmm1
	movups		%xmm0, (%r9)
	movups		%xmm1, 16(%r9)
	
	leaq		32(%r9), %r9
	decl		%ecx
	jnz			1b
	
	xorl		%eax, %eax
	

	movaps		(%rsp), %xmm6
	movaps		16(%rsp), %xmm7
	movaps		32(%rsp), %xmm8
	movaps		48(%rsp), %xmm9
	movaps		64(%rsp), %xmm10
	movaps		80(%rsp), %xmm11
	movaps		96(%rsp), %xmm12
	movaps		112(%rsp), %xmm13
	movaps		128(%rsp), %xmm14
	movaps		144(%rsp), %xmm15
	addq		$168, %rsp
#line 394 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/synth_stereo_x86_64_float.S"
	ret


