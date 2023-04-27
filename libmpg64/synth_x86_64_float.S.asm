#line 1 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/synth_x86_64_float.S"








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

#line 10 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/synth_x86_64_float.S"



















#line 30 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/synth_x86_64_float.S"









	.section	.rodata


#line 43 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/synth_x86_64_float.S"
	.balign 32
scale_x86_64:
	.long   939524096
	.long   939524096
	.long   939524096
	.long   939524096
	.text
	.balign 16
.globl INT123_synth_1to1_real_x86_64_asm
INT123_synth_1to1_real_x86_64_asm:

	movq		%rcx, %r10
	subq		$120, %rsp 
	movaps		%xmm6, (%rsp)
	movaps		%xmm7, 16(%rsp)
	movaps		%xmm8, 32(%rsp)
	movaps		%xmm9, 48(%rsp)
	movaps		%xmm10, 64(%rsp)
	movaps		%xmm11, 80(%rsp)
	movaps		%xmm15, 96(%rsp)
#line 64 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/synth_x86_64_float.S"

	leaq		scale_x86_64(%rip), %rax
	movaps		(%rax), %xmm15
	
	andq		$0xf, %r9
	shlq		$2, %r9
	leaq		64(%r10), %r10
	subq		%r9, %r10

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
	mulps		(%rdx), %xmm8
	mulps		16(%rdx), %xmm1
	mulps		32(%rdx), %xmm2
	mulps		48(%rdx), %xmm3
	mulps		64(%rdx), %xmm9
	mulps		80(%rdx), %xmm5
	mulps		96(%rdx), %xmm6
	mulps		112(%rdx), %xmm7
	
	addps		%xmm1, %xmm8
	addps		%xmm2, %xmm3
	addps		%xmm5, %xmm9
	addps		%xmm7, %xmm6
	addps		%xmm3, %xmm8
	addps		%xmm6, %xmm9
	leaq		256(%r10), %r10
	leaq		128(%rdx), %rdx
	
	movups		(%r10), %xmm10
	movups		16(%r10), %xmm1
	movups		32(%r10), %xmm2
	movups		48(%r10), %xmm3
	movups		128(%r10), %xmm11
	movups		144(%r10), %xmm5
	movups		160(%r10), %xmm6
	movups		176(%r10), %xmm7
	mulps		(%rdx), %xmm10
	mulps		16(%rdx), %xmm1
	mulps		32(%rdx), %xmm2
	mulps		48(%rdx), %xmm3
	mulps		64(%rdx), %xmm11
	mulps		80(%rdx), %xmm5
	mulps		96(%rdx), %xmm6
	mulps		112(%rdx), %xmm7
	
	addps		%xmm1, %xmm10
	addps		%xmm2, %xmm3
	addps		%xmm5, %xmm11
	addps		%xmm7, %xmm6
	addps		%xmm3, %xmm10
	addps		%xmm6, %xmm11
	leaq		256(%r10), %r10
	leaq		128(%rdx), %rdx
	
	movaps		%xmm8, %xmm0
	movaps		%xmm10, %xmm1
	unpcklps	%xmm9, %xmm8
	unpcklps	%xmm11, %xmm10
	unpckhps	%xmm9, %xmm0
	unpckhps	%xmm11, %xmm1
	movaps		%xmm8, %xmm2
	movaps		%xmm0, %xmm3
	movlhps		%xmm10, %xmm8
	movhlps		%xmm2, %xmm10
	movlhps		%xmm1, %xmm0
	movhlps		%xmm3, %xmm1
	subps		%xmm10, %xmm8
	subps		%xmm1, %xmm0
	addps		%xmm8, %xmm0
	
	movups		(%r8), %xmm1
	movups		16(%r8), %xmm2
	mulps		%xmm15, %xmm0
	shufps		$0xdd, %xmm2, %xmm1
	movaps		%xmm0, %xmm2
	unpcklps	%xmm1, %xmm0
	unpckhps	%xmm1, %xmm2
	movups		%xmm0, (%r8)
	movups		%xmm2, 16(%r8)
	
	leaq		32(%r8), %r8
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
	mulps		(%rdx), %xmm8
	mulps		16(%rdx), %xmm1
	mulps		32(%rdx), %xmm2
	mulps		48(%rdx), %xmm3
	mulps		-64(%rdx), %xmm9
	mulps		-48(%rdx), %xmm5
	mulps		-32(%rdx), %xmm6
	mulps		-16(%rdx), %xmm7
	
	addps		%xmm1, %xmm8
	addps		%xmm2, %xmm3
	addps		%xmm5, %xmm9
	addps		%xmm7, %xmm6
	addps		%xmm3, %xmm8
	addps		%xmm6, %xmm9
	leaq		256(%r10), %r10
	leaq		-128(%rdx), %rdx
	
	movups		(%r10), %xmm10
	movups		16(%r10), %xmm1
	movups		32(%r10), %xmm2
	movups		48(%r10), %xmm3
	movups		128(%r10), %xmm11
	movups		144(%r10), %xmm5
	movups		160(%r10), %xmm6
	movups		176(%r10), %xmm7
	mulps		(%rdx), %xmm10
	mulps		16(%rdx), %xmm1
	mulps		32(%rdx), %xmm2
	mulps		48(%rdx), %xmm3
	mulps		-64(%rdx), %xmm11
	mulps		-48(%rdx), %xmm5
	mulps		-32(%rdx), %xmm6
	mulps		-16(%rdx), %xmm7
	
	addps		%xmm1, %xmm10
	addps		%xmm2, %xmm3
	addps		%xmm5, %xmm11
	addps		%xmm7, %xmm6
	addps		%xmm3, %xmm10
	addps		%xmm6, %xmm11
	leaq		256(%r10), %r10
	leaq		-128(%rdx), %rdx
	
	movaps		%xmm8, %xmm0
	movaps		%xmm10, %xmm1
	unpcklps	%xmm9, %xmm8
	unpcklps	%xmm11, %xmm10
	unpckhps	%xmm9, %xmm0
	unpckhps	%xmm11, %xmm1
	movaps		%xmm8, %xmm2
	movaps		%xmm0, %xmm3
	movlhps		%xmm10, %xmm8
	movhlps		%xmm2, %xmm10
	movlhps		%xmm1, %xmm0
	movhlps		%xmm3, %xmm1
	addps		%xmm10, %xmm8
	addps		%xmm1, %xmm0
	addps		%xmm8, %xmm0
	
	movups		(%r8), %xmm1
	movups		16(%r8), %xmm2
	mulps		%xmm15, %xmm0
	shufps		$0xdd, %xmm2, %xmm1
	movaps		%xmm0, %xmm2
	unpcklps	%xmm1, %xmm0
	unpckhps	%xmm1, %xmm2
	movups		%xmm0, (%r8)
	movups		%xmm2, 16(%r8)
	
	leaq		32(%r8), %r8
	decl		%ecx
	jnz			1b
	
	xorl		%eax, %eax
	

	movaps		(%rsp), %xmm6
	movaps		16(%rsp), %xmm7
	movaps		32(%rsp), %xmm8
	movaps		48(%rsp), %xmm9
	movaps		64(%rsp), %xmm10
	movaps		80(%rsp), %xmm11
	movaps		96(%rsp), %xmm15
	addq		$120, %rsp
#line 257 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/synth_x86_64_float.S"
	ret


