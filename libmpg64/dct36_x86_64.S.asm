#line 1 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/dct36_x86_64.S"








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

#line 10 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/dct36_x86_64.S"

















#line 28 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/dct36_x86_64.S"




	

	.section	.rodata


#line 38 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/dct36_x86_64.S"
	.balign 16
dct36_x86_64_COS9:
	.long 0x3f5db3d7
	.long 0x3f5db3d7
	.long 0x3f000000
	.long 0x3f000000
	.long 0x3f7c1c5c
	.long 0x3f7c1c5c
	.long 0x3f708fb2
	.long 0x3f708fb2
	.long 0x3f248dbb
	.long 0x3f248dbb
	.long 0x3e31d0d4
	.long 0x3e31d0d4
	.long 0x3eaf1d44
	.long 0x3eaf1d44
	.long 0x3f441b7d
	.long 0x3f441b7d
	.balign 16
dct36_x86_64_tfcos36:
	.long 0x3f007d2b
	.long 0x3f0483ee
	.long 0x3f0d3b7d
	.long 0x3f1c4257
	.long 0x40b79454
	.long 0x3ff746ea
	.long 0x3f976fd9
	.long 0x3f5f2944
	.long 0x3f3504f3
	.balign 16
dct36_x86_64_mask:
	.long 0,0xffffffff,0,0xffffffff
	.balign 16
dct36_x86_64_sign:
	.long 0x80000000,0x80000000,0x80000000,0x80000000
	.text
	.balign 16
	.globl INT123_dct36_x86_64
INT123_dct36_x86_64:

	push		%rbp
	mov			%rsp, %rbp
	sub			$160, %rsp
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
	movq		48(%rbp), %r10
#line 93 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/dct36_x86_64.S"
	lea			dct36_x86_64_COS9(%rip), %rax
	lea			dct36_x86_64_tfcos36(%rip), %r11
	
	xorps		%xmm5, %xmm5
	movups		(%rcx), %xmm1
	movups		16(%rcx), %xmm2
	movups		32(%rcx), %xmm3
	movups		48(%rcx), %xmm4
	movlps		64(%rcx), %xmm5
	xorps		%xmm6, %xmm6
	movaps		%xmm1, %xmm7
	shufps		$0x93, %xmm7, %xmm7
	movaps		%xmm2, %xmm8
	shufps		$0x93, %xmm8, %xmm8
	movaps		%xmm3, %xmm9
	shufps		$0x93, %xmm9, %xmm9
	movaps		%xmm4, %xmm10
	shufps		$0x93, %xmm10, %xmm10
	movaps		%xmm5, %xmm11
	shufps		$0xe1, %xmm11, %xmm11
	movss		%xmm10, %xmm11
	addps		%xmm11, %xmm5
	movss		%xmm9, %xmm10
	addps		%xmm10, %xmm4
	movss		%xmm8, %xmm9
	addps		%xmm9, %xmm3
	movss		%xmm7, %xmm8
	addps		%xmm8, %xmm2
	movss		%xmm6, %xmm7
	addps		%xmm7, %xmm1
	
	movaps		dct36_x86_64_mask(%rip), %xmm0
	movaps		%xmm4, %xmm6
	shufps		$0x4e, %xmm5, %xmm4
	movaps		%xmm3, %xmm7
	shufps		$0x4e, %xmm6, %xmm3
	andps		%xmm0, %xmm6
	addps		%xmm6, %xmm4
	movaps		%xmm2, %xmm6
	shufps		$0x4e, %xmm7, %xmm2
	andps		%xmm0, %xmm7
	addps		%xmm7, %xmm3
	movaps		%xmm1, %xmm7
	shufps		$0x4e, %xmm6, %xmm1
	andps		%xmm0, %xmm6
	addps		%xmm6, %xmm2
	movaps		%xmm7, %xmm6
	andps		%xmm0, %xmm7
	xorps		%xmm0, %xmm0
	addps		%xmm7, %xmm1
	movlhps		%xmm6, %xmm0








	
	movaps		%xmm2, %xmm5
	shufps		$0xe4, %xmm3, %xmm5
	shufps		$0xe4, %xmm4, %xmm3
	shufps		$0xe4, %xmm2, %xmm4
	movaps		%xmm5, %xmm2






	movaps		(%rax), %xmm15
	movaps		16(%rax), %xmm6
	movaps		32(%rax), %xmm7
	movaps		48(%rax), %xmm8
	mulps		%xmm15, %xmm5
	addps		%xmm0, %xmm5
	







	movaps		%xmm6, %xmm9
	movaps		%xmm7, %xmm12
	movaps		%xmm8, %xmm13
	mulps		%xmm1, %xmm9
	mulps		%xmm3, %xmm12
	mulps		%xmm4, %xmm13
	addps		%xmm5, %xmm9
	addps		%xmm13, %xmm12
	addps		%xmm9, %xmm12
	
	movaps		%xmm1, %xmm13
	subps		%xmm3, %xmm13
	movaps		%xmm0, %xmm10
	shufps		$0xe0, %xmm2, %xmm10
	movaps		%xmm0, %xmm14
	subps		%xmm10, %xmm14
	subps		%xmm4, %xmm13
	mulps		%xmm15, %xmm13
	addps		%xmm14, %xmm13
	
	movaps		%xmm7, %xmm9
	movaps		%xmm8, %xmm15
	movaps		%xmm6, %xmm14
	mulps		%xmm1, %xmm9
	mulps		%xmm3, %xmm15
	mulps		%xmm4, %xmm14
	subps		%xmm5, %xmm9
	subps		%xmm15, %xmm14
	addps		%xmm9, %xmm14
	
	mulps		%xmm1, %xmm8
	mulps		%xmm3, %xmm6
	mulps		%xmm4, %xmm7
	subps		%xmm5, %xmm8
	subps		%xmm7, %xmm6
	addps		%xmm6, %xmm8
	movaps		%xmm8, %xmm15
	
	movss		32(%r11), %xmm5
	subps		%xmm1, %xmm0
	subps		%xmm2, %xmm4
	addps		%xmm3, %xmm0
	addps		%xmm4, %xmm0
	shufps		$0xaf, %xmm0, %xmm0
	mulss		%xmm5, %xmm0
	movaps		%xmm0, %xmm11







	movaps		%xmm12, %xmm5
	unpckhps	%xmm13, %xmm5
	unpcklps	%xmm13, %xmm12
	movaps		%xmm14, %xmm6
	unpckhps	%xmm15, %xmm6
	unpcklps	%xmm15, %xmm14
	xorps		dct36_x86_64_sign(%rip), %xmm6








	movaps		%xmm12, %xmm0
	movlhps		%xmm14, %xmm12
	movhlps		%xmm0, %xmm14
	movaps		%xmm5, %xmm0
	movlhps		%xmm6, %xmm0
	movhlps		%xmm5, %xmm6
	movaps		%xmm6, %xmm15








	movaps		(%r11), %xmm6
	movaps		16(%r11), %xmm7
	movaps		%xmm15, %xmm10
	addps		%xmm14, %xmm15
	subps		%xmm14, %xmm10
	movaps		%xmm0, %xmm14
	addps		%xmm12, %xmm0
	subps		%xmm12, %xmm14
	mulps		%xmm6, %xmm15
	mulps		%xmm10, %xmm7









	movaps		%xmm15, %xmm1
	movups		108(%r9), %xmm2
	movups		92(%r9), %xmm3
	shufps		$0x1b, %xmm3, %xmm3
	movups		36(%r9), %xmm4
	movups		20(%r9), %xmm5
	shufps		$0x1b, %xmm5, %xmm5
	movaps		%xmm0, %xmm6
	addps		%xmm1, %xmm0
	subps		%xmm1, %xmm6
	mulps		%xmm0, %xmm2
	mulps		%xmm3, %xmm0
	mulps		%xmm6, %xmm4
	mulps		%xmm5, %xmm6
	movups		36(%rdx), %xmm1
	movups		20(%rdx), %xmm3
	shufps		$0x1b, %xmm6, %xmm6
	addps		%xmm4, %xmm1
	addps		%xmm6, %xmm3
	shufps		$0x1b, %xmm0, %xmm0
	movups		%xmm2, 36(%r8)
	movups		%xmm0, 20(%r8)
	movss		%xmm1, 32*36(%r10)
	movss		%xmm3, 32*20(%r10)
	movhlps		%xmm1, %xmm2
	movhlps		%xmm3, %xmm4
	movss		%xmm2, 32*44(%r10)
	movss		%xmm4, 32*28(%r10)
	shufps		$0xb1, %xmm1, %xmm1
	shufps		$0xb1, %xmm3, %xmm3
	movss		%xmm1, 32*40(%r10)
	movss		%xmm3, 32*24(%r10)
	movhlps		%xmm1, %xmm2
	movhlps		%xmm3, %xmm4
	movss		%xmm2, 32*48(%r10)
	movss		%xmm4, 32*32(%r10)
	
	movhlps		%xmm11, %xmm0
	movaps		%xmm11, %xmm1
	movss		124(%r9), %xmm2
	movss		88(%r9), %xmm3
	movss		52(%r9), %xmm4
	movss		16(%r9), %xmm5
	movss		%xmm0, %xmm6
	addss		%xmm1, %xmm0
	subss		%xmm1, %xmm6
	mulss		%xmm0, %xmm2
	mulss		%xmm3, %xmm0
	mulss		%xmm6, %xmm4
	mulss		%xmm5, %xmm6
	addss		52(%rdx), %xmm4
	addss		16(%rdx), %xmm6
	movss		%xmm2, 52(%r8)
	movss		%xmm0, 16(%r8)
	movss		%xmm4, 32*52(%r10)
	movss		%xmm6, 32*16(%r10)
	
	movaps		%xmm14, %xmm0
	movaps		%xmm7, %xmm1
	movups		128(%r9), %xmm2
	movups		72(%r9), %xmm3
	shufps		$0x1b, %xmm2, %xmm2
	movlps		56(%r9), %xmm4
	movhps		64(%r9), %xmm4
	movups		(%r9), %xmm5
	shufps		$0x1b, %xmm4, %xmm4
	movaps		%xmm0, %xmm6
	addps		%xmm1, %xmm0
	subps		%xmm1, %xmm6
	mulps		%xmm0, %xmm2
	mulps		%xmm3, %xmm0
	mulps		%xmm6, %xmm4
	mulps		%xmm5, %xmm6
	movlps		56(%rdx), %xmm1
	movhps		64(%rdx), %xmm1
	movups		(%rdx), %xmm3
	shufps		$0x1b, %xmm4, %xmm4
	addps		%xmm6, %xmm3
	addps		%xmm4, %xmm1
	shufps		$0x1b, %xmm2, %xmm2
	movups		%xmm0, (%r8)
	movlps		%xmm2, 56(%r8)
	movhps		%xmm2, 64(%r8)
	movss		%xmm1, 32*56(%r10)
	movss		%xmm3, (%r10)
	movhlps		%xmm1, %xmm2
	movhlps		%xmm3, %xmm4
	movss		%xmm2, 32*64(%r10)
	movss		%xmm4, 32*8(%r10)
	shufps		$0xb1, %xmm1, %xmm1
	shufps		$0xb1, %xmm3, %xmm3
	movss		%xmm1, 32*60(%r10)
	movss		%xmm3, 32*4(%r10)
	movhlps		%xmm1, %xmm2
	movhlps		%xmm3, %xmm4
	movss		%xmm2, 32*68(%r10)
	movss		%xmm4, 32*12(%r10)
	

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
	mov			%rbp, %rsp
	pop			%rbp
#line 392 "C:/CProjects/mpg123/ports/cmake/src/libmpg123/../../../../src/libmpg123/dct36_x86_64.S"
	ret


